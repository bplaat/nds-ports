# Toolchain

Everything the ports share to build Nintendo DS programs with plain clang and lld, without
devkitPro. `build.sh` builds it all in `target/`, the port build scripts run it for you:

- Clones devkitPro's [calico](https://github.com/devkitPro/calico) v1.2.0 (the system library, its
  startup code and linker scripts), [libnds](https://github.com/devkitPro/libnds) v2.0.2,
  [libdvm](https://github.com/devkitPro/libdvm) v2.1.0 with
  [FatFs](https://github.com/devkitPro/fatfs-mod) v0.15.3 (FAT on the SD card and NitroFS) and the
  [default ARM7 program](https://github.com/devkitPro/default-arm7) v0.8.4, applies their
  `<name>/patches/` as commits and builds them to `target/lib*.a`
- Builds our tiny C library in `libc/` to `target/libc9.a` and `target/libc7.a`
- Builds the default ARM7 program with the SD card, DLDI and sound drivers to `target/ds7.elf`
- Clones devkitPro's [ndstool](https://github.com/devkitPro/ndstool) v2.3.1 and builds it to
  `target/bin/ndstool`, which puts the programs, the icon and NitroFS in a `.nds`
- Checks out only the builtins of LLVM's [compiler-rt](https://github.com/llvm/llvm-project/tree/main/compiler-rt/lib/builtins)
  llvmorg-23.1.3 and builds the 64-bit integer math, division and software floating point the
  ARM CPUs can't do in instructions to `target/libcompiler-rt9.a` and `target/libcompiler-rt7.a`

```sh
./build.sh                 # fetch, patch and build
./build.sh clean           # remove target/
./build.sh export-patches  # write the commits in target/<name>/ back to <name>/patches/
./build.sh test            # build test/ and run it in melonDS
```

The ARM9 is an ARM946E-S (ARMv5TE) and the ARM7 an ARM7TDMI (ARMv4T), both without floating
point unit. Code is Thumb, except in objects named `*.32.o` (calico's convention) and `*.itcm.o`
(the ports' `ITCM_SOURCES`): those are ARM code the linker scripts put in fast memory, ITCM on
the ARM9. The ARM7 can't switch between ARM and Thumb in a direct call, so its code makes long
calls through a register.

## Files

- `libc/` is a tiny C library that replaces newlib: `memcpy`/`memmove`/`memset` as ARM code in
  ITCM that copy words (and never single bytes to halfword aligned VRAM), a first-fit `malloc`
  that grows into the free main RAM, buffered `stdio` on newlib's devoptab devices (FAT on the SD
  card, NitroFS and the libnds console), a `printf` family that prints floats with float
  precision, a small `sscanf`, float math accurate to about 1e-6, `setjmp` and the startup code
  calico calls. It only uses the calico functions listed in `libc/calico.h`.
- `nds.mk` has the make rules for the ports' `Makefile.nds`, it links with calico's `ds9.ld`
- `run.sh` starts a program in melonDS, `make run` in a port's source directory uses it
- `test/` is a DS program that tests the toolchain, see below

## Test

`./build.sh test` builds `test/` like a port and runs it in melonDS with its own settings (in
`test/target/`, a copy of the app on macOS), with an SD card that is the folder
`test/target/sd/`. It checks the startup code, the C library, compiler-rt, calico's threads and
thread local storage, ARM code in ITCM, FAT files through libdvm and NitroFS files packed by
ndstool. The program writes its results to `test.txt` on the SD card after every group, the
script quits melonDS so it writes the folder, prints the results and fails when a check failed or
the program didn't finish.

## Patches

To change them, edit and commit in `target/<name>/`, run `./build.sh` to try it and
`./build.sh export-patches` to write the commits back. The build stops instead of applying
changed patches over commits or edits in `target/<name>/` that aren't exported yet.

- calico: `0001` writes the assembly in the unified syntax LLVM takes, `0002` removes the
  newlib glue (reent structs, locks, pthreads) and `0003` makes the linker scripts work with
  lld, which doesn't know `ALIGN_WITH_INPUT`: the load addresses are explicit
- libnds: `0001` builds it with clang and without newlib, `0002` adds the `libversion.h` its
  Makefile generates
- compiler-rt: `0001` assembles the optimized floating point functions for ARMv5TE, `0002`
  returns from the division functions with `bx`, so they return to Thumb code on ARMv4T
