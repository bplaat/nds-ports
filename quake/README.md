# Quake for Nintendo DS

A port of id Software's [GPL Quake source](https://github.com/id-Software/Quake)
(WinQuake) to the Nintendo DS and DSi, rendered by the DS 3D hardware.

```sh
./build.sh                  # fetch, patch and build build/quake.nds
./build.sh dev              # fetch and patch, then edit and commit in build/Quake-*/
./build.sh export-patches   # write those commits back to patches/
./build.sh clean            # remove build/
```

## Game data

`assets/` is Quake's base directory: game data goes in `assets/id1/` and mods
in their own directory next to it, like `assets/hipnotic/`.

| File           | Game                                                            |
| -------------- | --------------------------------------------------------------- |
| `id1/pak0.pak` | Quake shareware (episode 1), downloaded and embedded by default |
| `id1/pak1.pak` | Quake registered (retail), episodes 2 to 4                      |

The freely redistributable shareware v1.06 is fetched from archive.org,
checksum verified and embedded in the ROM through NitroFS, so the `.nds` is
always playable, even without an SD card. Add your own `pak1.pak` next to it
in `assets/id1/` to embed it, or put it in `/data/quake/id1/` on the SD card.
Both are searched, the SD card last.

With an SD card the config and savegames go to `/data/quake/id1/`. Extra
command line options such as `-game hipnotic` or `+map e1m2` can be put in
`/data/quake/args.txt`. The 4 MiB of the DS fit the id maps, the mission
packs and big mods need the 16 MiB of the DSi.

## Controls

| Button | In game                                     | In menus                   |
| ------ | ------------------------------------------- | -------------------------- |
| D-pad  | Move and turn                               | Navigate                   |
| A      | Fire                                        | Select / yes               |
| B      | Jump                                        | Back                       |
| X      | Next weapon                                 | Backspace                  |
| Y      | Run (hold)                                  |                            |
| L / R  | Strafe                                      | Scroll the console         |
| START  | Menu                                        | Close menu                 |
| SELECT | Scores                                      | Complete a console command |
| Stylus | Drag to look around, tap buttons or weapons |                            |

The buttons send the keys of Quake's default bindings, so they can be
rebound in the options menu.

The touch screen shows the status bar, with the level name, kills, secrets
and time above it and buttons to jump, show the scores, center the view and
open the console. Tapping a weapon icon of the status bar selects it. With
the console open a keyboard covers the lower half, so console commands like
`god` or `map e1m5` can be typed. Messages, the console and the menus are on
the touch screen too, the top screen always shows the 3D view.
`crosshair 1` shows a crosshair and `vid_showfps 1` the frame rate.

## How it works on the DS

- The top screen is drawn by the 3D hardware: vertices are 16-bit fixed
  point world coordinates and surfaces are drawn as quad strips zig-zagging
  across their polygons, half as many polygons as triangles.
- Textures stay 8-bit with Quake's palette in the texture palette, so the
  palette shifts for damage, pickups and water cost nothing. They go to VRAM
  banks A, B and D when a map loads, at the largest mip level that fits, and
  are scaled by the texture matrix. No texture stays in main RAM.
- The light maps become vertex light for each light style, so flickering
  lights still animate. Dynamic lights from muzzle flashes, rockets and
  explosions light the world too. The texture palette is twice as bright, so
  the light can overbright like in the software renderer.
- The sky has both layers, projected on a flattened sphere like GLQuake, and
  the weapon gets the front of the depth range, so it doesn't poke into walls.
- The 2D drawing stays Quake's own, in a 320x200 buffer scaled to the touch
  screen by the affine background hardware and smoothed by a second, half
  pixel offset layer (`vid_smooth`). Only the changed rows are copied.
- To fit in 4 MiB, maps are read from the file one lump at a time, their
  vertices, edges and light maps are freed once the hardware vertices are
  built, skins are read from the model files instead of kept in RAM, and the
  DS gets 448 edicts instead of 600.
- For speed the world is traversed in fixed point, the local server runs at
  20 Hz with the client interpolating in between, `memcpy`, `memset` and the
  collision code run from ITCM and the trigonometry is done in float.
- Quake runs in a thread with its stack in main RAM, the ARM9 stack is in the
  16 KiB DTCM and Quake keeps arrays of up to 32 KiB on the stack.
- Quake's mixer paints a stereo ring buffer, split into two ring buffers that
  loop on hardware channels panned left and right.

In melonDS emulating a DS the shareware demo plays at 40 fps on average, 25
in its heaviest second, and e1m1 plays at 37 fps. The DSi runs its ARM9 at
twice the speed.
