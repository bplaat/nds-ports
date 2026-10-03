# Wolfenstein 3D for Nintendo DS

A port of [Wolf4SDL](https://github.com/KS-Presto/Wolf4SDL), the portable
source port of id Software's Wolfenstein 3D, to the Nintendo DS and DSi.

```sh
./build.sh                  # fetch, patch and build target/wolf3d.nds
./build.sh dev              # fetch and patch, then edit and commit in target/Wolf4SDL-*/
./build.sh export-patches   # write those commits back to patches/
./build.sh clean            # remove target/
```

## Game data

| Files   | Game                                                               |
| ------- | ------------------------------------------------------------------ |
| `*.wl6` | Wolfenstein 3D v1.4, Apogee or GT/id/Activision (Steam, GOG) data  |
| `*.wl3` | Wolfenstein 3D v1.4, episodes 1 to 3                               |
| `*.wl1` | Wolfenstein 3D shareware v1.4, downloaded and embedded by default |

The freely redistributable shareware (episode 1) is fetched from archive.org,
checksum verified and embedded in the ROM through NitroFS, so the `.nds` is
always playable, even without an SD card. Add the data files of your own
full game (`audiohed`, `audiot`, `gamemaps`, `maphead`, `vgadict`,
`vgagraph`, `vgahead` and `vswap`) to `assets/` to embed them, or put them
in `/data/wolf3d/` on the SD card (the folder next to the `.nds` and the SD
root also work). The full game is played when it is found, it has the
shareware episode too. Spear of Destiny needs a build of its own and isn't
supported.

With an SD card the config and savegames go to `/data/wolf3d/`. Without one
you can still play, saving then shows a message instead. Wolf4SDL's command
line options such as `--goobers` or `--tedlevel 3 --hard` can be put in
`/data/wolf3d/args.txt`.

## Controls

| Button | In game                                      | In menus             |
| ------ | -------------------------------------------- | -------------------- |
| D-pad  | Move and turn                                | Navigate             |
| A      | Fire                                         | Select / yes         |
| B      | Open / use                                   | Back / no            |
| X      | Next weapon                                  | Backspace (in names) |
| Y      | Run (hold)                                   |                      |
| L / R  | Strafe                                       |                      |
| START  | Menu                                         | Back                 |
| SELECT | Zoom the map                                 |                      |
| Stylus | Drag to turn, tap the weapon and map buttons |                      |

The buttons are Wolfenstein's joystick, so A, B, X and Y can be rebound in
Control > Customize controls. New savegames are named after the episode and
floor, so saving is pressing A twice. To rename one, UP/DOWN change a letter
and LEFT/RIGHT move the cursor.

The touch screen shows the status bar, the explored part of the level as a
map around the player, with the walls in the colors of their textures and
the doors in the colors of their keys, and buttons for the four weapons. The
top screen shows the 3D view without a status bar, Change View in the menu
makes it smaller and faster.

## How it works on the DS

- Wolf4SDL runs on a small implementation of the parts of SDL 2 and
  SDL_mixer it uses (`nds/`), so its own code is barely changed. One build,
  with the Apogee v1.4 graphics layout, plays the shareware, the Apogee and
  the GT/id/Activision data.
- The 320x200 frame is copied by DMA into VRAM and scaled to 256x192 by the
  affine background hardware, and a second, alpha blended background layer
  sampling half a pixel further smooths it. The game palette is the
  hardware palette, so fades and the damage and pickup flashes cost nothing.
- The walls are drawn through a strip of 8 columns in DTCM that is copied
  into the frame in words, as drawing straight down the columns of a frame
  in main RAM misses the cache with every pixel. The ray caster, the scaler
  and the fixed point math run from ITCM.
- The AdLib music and sound effects play the OPL2 register writes on the
  sound hardware instead of emulating the chip sample by sample, which took
  most of the ARM9: every OPL channel is a hardware channel looping one
  period of the waveform its two FM operators make, at its pitch, with the
  envelopes, key scaling and levels of the MAME emulator done in software.
- The digitized sounds play as they are on hardware channels 0-5, with the
  positional stereo of Wolf4SDL. The PC speaker sounds are mixed into a
  stream on channel 6.
- Wolf4SDL keeps all game data in memory, about 1.6 MiB for the shareware,
  which leaves room for the full game in the 4 MiB of the DS.

In melonDS emulating a DS the first level plays at 38 fps with the full
screen view and 47 fps at the view size the PC version starts with. The DSi runs its ARM9 at twice the
speed.
