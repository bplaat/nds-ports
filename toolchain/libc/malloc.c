// A simple first-fit heap that grows with sbrk() into the free main RAM, by just what is needed
// as memory is tight: a game can malloc() all that is left after sbrk(0).

#include <errno.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "calico.h"

// calico's startup code sets the free main RAM
void* fake_heap_start;
void* fake_heap_end;
static char* heap_break;

void* sbrk(ptrdiff_t increment) {
    if (!heap_break)
        heap_break = fake_heap_start;
    if (increment > (char*)fake_heap_end - heap_break || increment < (char*)fake_heap_start - heap_break) {
        errno = ENOMEM;
        return (void*)-1;
    }
    char* previous = heap_break;
    heap_break += increment;
    return previous;
}

// Free chunks form a linked list sorted by address
typedef struct Chunk {
    size_t size;
    struct Chunk* next;
} Chunk;

// Stored just before every pointer we hand out
typedef struct {
    uintptr_t start;
    size_t size;
} Header;

static Chunk* free_list = NULL;

static uintptr_t align_up(uintptr_t value, size_t alignment) {
    return (value + alignment - 1) & ~(uintptr_t)(alignment - 1);
}

static void insert_chunk(uintptr_t start, size_t size) {
    Chunk** link = &free_list;
    while (*link && (uintptr_t)*link < start)
        link = &(*link)->next;

    Chunk* chunk = (Chunk*)start;
    chunk->size = size;
    chunk->next = *link;
    if (chunk->next && start + size == (uintptr_t)chunk->next) {
        chunk->size += chunk->next->size;
        chunk->next = chunk->next->next;
    }
    *link = chunk;

    // Merge with the previous chunk when they touch
    if (link != &free_list) {
        Chunk* previous = (Chunk*)((uintptr_t)link - offsetof(Chunk, next));
        if ((uintptr_t)previous + previous->size == start) {
            previous->size += chunk->size;
            previous->next = chunk->next;
        }
    }
}

// Grows the heap, the free chunk at its end counts as part of what is needed
static bool grow(size_t size) {
    uintptr_t start = (uintptr_t)sbrk(0);
    Chunk* last = free_list;
    while (last && last->next)
        last = last->next;
    if (last && (uintptr_t)last + last->size == start)
        size = size > last->size ? size - last->size : 8;
    size = align_up(start + size, 8) - start;
    if (sbrk((ptrdiff_t)size) == (void*)-1)
        return false;
    insert_chunk(start, size);
    return true;
}

static void* allocate(size_t alignment, size_t size) {
    // Bigger than all memory, also keeps the size calculations below from overflowing
    if (size > 0x10000000 || alignment > 0x10000000 || (alignment & (alignment - 1)) != 0)
        return NULL;
    if (alignment < 8)
        alignment = 8;
    size = align_up(size < sizeof(Chunk) ? sizeof(Chunk) : size, 8);
    for (int attempt = 0; attempt < 2; attempt++) {
        for (Chunk** link = &free_list; *link; link = &(*link)->next) {
            uintptr_t start = (uintptr_t)*link;
            size_t chunk_size = (*link)->size;
            uintptr_t pointer = align_up(start + sizeof(Header), alignment);
            uintptr_t end = pointer + size;
            if (end > start + chunk_size)
                continue;

            // Keep the leftover as a free chunk when it is big enough
            *link = (*link)->next;
            uintptr_t chunk_end = start + chunk_size;
            if (chunk_end - end >= 32)
                insert_chunk(end, chunk_end - end);
            else
                end = chunk_end;

            Header* header = (Header*)(pointer - sizeof(Header));
            header->start = start;
            header->size = end - start;
            return (void*)pointer;
        }
        // The heap grows at its end, where a free chunk may already be
        if (!grow(size + alignment + sizeof(Header)))
            return NULL;
    }
    return NULL;
}

void* memalign(size_t alignment, size_t size) {
    ArmIrqState state = armIrqLockByPsr();
    void* pointer = allocate(alignment, size);
    armIrqUnlockByPsr(state);
    return pointer;
}

void* aligned_alloc(size_t alignment, size_t size) {
    return memalign(alignment, size);
}

void* malloc(size_t size) {
    return memalign(8, size);
}

void* calloc(size_t count, size_t size) {
    if (size != 0 && count > SIZE_MAX / size)
        return NULL;
    void* pointer = malloc(count * size);
    if (pointer)
        memset(pointer, 0, count * size);
    return pointer;
}

void free(void* pointer) {
    if (!pointer)
        return;
    Header* header = (Header*)((uintptr_t)pointer - sizeof(Header));
    ArmIrqState state = armIrqLockByPsr();
    insert_chunk(header->start, header->size);
    armIrqUnlockByPsr(state);
}

void* realloc(void* pointer, size_t size) {
    if (!pointer)
        return malloc(size);
    Header* header = (Header*)((uintptr_t)pointer - sizeof(Header));
    size_t old_size = header->start + header->size - (uintptr_t)pointer;
    if (size <= old_size)
        return pointer;
    void* new_pointer = malloc(size);
    if (new_pointer) {
        memcpy(new_pointer, pointer, old_size);
        free(pointer);
    }
    return new_pointer;
}
