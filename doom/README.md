# DOOM for Nintendo DS

A port of [doomgeneric](https://github.com/ozkl/doomgeneric), a portable
fork of Chocolate Doom, to the Nintendo DS and DSi.

```sh
./build.sh                  # fetch, patch and build build/doom.nds
./build.sh dev              # fetch and patch, then edit and commit in build/doomgeneric-*/
./build.sh export-patches   # write those commits back to patches/
./build.sh clean            # remove build/
```

## Game data

| File                      | Game                                               |
| ------------------------- | -------------------------------------------------- |
| `doom.wad`                | The Ultimate DOOM / DOOM registered (retail)       |
| `doom2.wad`               | DOOM II: Hell on Earth                             |
| `tnt.wad`, `plutonia.wad` | Final DOOM                                         |
| `doom1.wad`               | DOOM shareware, downloaded and embedded by default |

The freely redistributable shareware (episode 1, v1.9) is fetched from
archive.org, checksum verified and embedded in the ROM through NitroFS, so the
`.nds` is always playable, even without an SD card. Add your own retail WADs
next to it in `assets/` to embed them, or put them in `/data/doom/` on the SD
card (the folder next to the `.nds` and the SD root also work).

Every `.wad` found is listed in a picker that shows each game's title screen.
IWADs are games, other WADs are mods (PWADs) and start on a matching IWAD:
DOOM II for MAPxx mods, DOOM for ExMy mods (the shareware can't load mods).

With an SD card the config goes to `/data/doom/` and savegames to
`/data/doom/saves/<game>/doomsav0.dsg` and up, in Doom's own savegame format,
a mod keeps its own in `saves/<mod>.wad/`.
Without one you can still play, saving then shows a message instead. Extra
command line options such as `-skill 4` or `-warp 1 3` can be put in
`/data/doom/args.txt`.

## Controls

| Button | In game         | In menus     |
| ------ | --------------- | ------------ |
| D-pad  | Move and turn   | Navigate     |
| A      | Fire            | Select / yes |
| B      | Use / open      | Back / no    |
| X      | Next weapon     |              |
| Y      | Run (hold)      |              |
| L / R  | Strafe          |              |
| START  | Menu            | Close menu   |
| Stylus | Tap a weapon    |              |

Savegames are named after the level, there is no keyboard to type a name.
While playing, the main menu ends the game instead of quitting. Options has
Messages and Sound Volume, with sliders for effects and music, which start at
full volume; the DS volume slider sets the loudness. While Options is open the
touch screen shows the controls.

While playing, the top screen only shows the view. The touch screen shows the
classic status bar at the top, the map in the middle, following you, with the
level name, time, kills, items and secrets, and at the bottom stone cells like
the status bar's: the weapons, with their pickup picture (the fist and pistol
as held, scaled down), dark until you have them, slot number and ammo, and the
status bar's ammo table. The shareware has no plasma rifle and BFG, those show
their name. Outside of a level it shows the controls.

## How it works on the DS

- The top screen shows the middle 256x192 of Doom's 320x200 screen 1:1.
  The view fills it at high detail. Doom's
  320x200 had pixels 1.2 times taller than wide on a 4:3 monitor, so on the
  square pixels of the DS its vertical projection is 1.2 times the
  horizontal one: walls, sprites, the weapon, floors and the sky keep their
  proportions with the same 90 degree field of view.
- Menus, messages, the intermission stats and texts are drawn 1:1 inside the
  visible part, moved where they were outside it. Finale texts and HUD and
  menu messages wrap at words where they are wider, the three menu items
  wider than it are squeezed.
- Only frames with a picture made for the whole screen (title, credits, the
  intermission maps, end pictures) are scaled to the top screen, by the
  affine background hardware, smoothed by a second layer half a pixel
  further that is alpha blended on top.
- The status bar on the touch screen is drawn from its own graphics at the
  same positions, its ammo table didn't fit and is a cell of the weapons.
- Doom's palette lives in the hardware palette, so damage and pickup flashes
  cost nothing.
- The automap is Doom's own, which only draws the walls you have seen,
  drawn on the touch screen every other frame while you keep playing.
- Sound effects play on hardware channels 0-7, which do the resampling, volume
  and stereo panning.
- Music (MUS and MIDI) is played by a wavetable synth on hardware channels
  8-15, with waveforms per instrument family and synthesized drums, sequenced
  by a 140 Hz thread.
- The hot rendering loops and `memcpy`/`memset` run from ITCM, newlib's copy
  a byte at a time. Fixed point division and the wall scales use the
  hardware divider.
- In DSi mode all 16 MB of RAM is used for the zone, the DS uses its 4 MB.

In melonDS emulating a DS `-timedemo demo1` runs at 26.5 fps, with the map
always drawn, the earlier layout with a 320x168 view scaled down ran at 25.4.
