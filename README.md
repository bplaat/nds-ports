# NDS Ports

PC games ported to the Nintendo DS (and DSi), built with
[devkitPro](https://devkitpro.org) (devkitARM + libnds/calico). Like the
[SerenityOS ports tree](https://github.com/SerenityOS/serenity/tree/master/Ports),
every port fetches a pinned upstream source release and applies patch files on
top, so the upstream code stays untouched in this repository.

| Port                     | Upstream                                                                  | Status   |
| ------------------------ | ------------------------------------------------------------------------- | -------- |
| [DOOM](doom)             | [doomgeneric](https://github.com/ozkl/doomgeneric) (Chocolate Doom based) | Playable |
| [Quake](quake)           | [id Software's Quake](https://github.com/id-Software/Quake) (WinQuake)    | Playable |
| [Wolfenstein 3D](wolf3d) | [Wolf4SDL](https://github.com/KS-Presto/Wolf4SDL)                         | Playable |

Every port is a self-contained directory:

| Path        | Contents                                                    |
| ----------- | ----------------------------------------------------------- |
| `README.md` | Game data, controls and how the port works                  |
| `build.sh`  | Fetches, patches and builds `target/<port>.nds`             |
| `icon.bmp`  | The 32x32 icon shown in DS menus                            |
| `patches/`  | The patch series on top of upstream                         |
| `assets/`   | Game data embedded in the ROM (gitignored, never commit it) |
| `target/`   | Sources, work files and the resulting ROM (gitignored)      |

To build a port, install devkitPro with the `nds-dev` group and run its
`build.sh`, for example `doom/build.sh`.

To change a port, run `./build.sh dev`, edit and commit in the patched source
tree in `target/`, then `./build.sh export-patches` writes the commits back to
`patches/`.

In melonDS, disable the JIT recompiler (Config > Emu settings > CPU): its
[JIT has a CPU bug](https://github.com/melonDS-emu/melonDS/issues/2622) that
hangs every libnds 2 / calico homebrew on a white screen at boot. Real
hardware and melonDS without JIT are fine.
