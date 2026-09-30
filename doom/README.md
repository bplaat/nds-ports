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
`/data/doom/saves/<game>/doomsav0.dsg` and up, in Doom's own savegame format.
Without one you can still play, saving then shows a message instead. Extra
command line options such as `-skill 4` or `-warp 1 3` can be put in
`/data/doom/args.txt`.

## Controls

| Button | In game                                            | In menus     |
| ------ | -------------------------------------------------- | ------------ |
| D-pad  | Move and turn                                      | Navigate     |
| A      | Fire                                               | Select / yes |
| B      | Use / open                                         | Back / no    |
| X      | Next weapon                                        |              |
| Y      | Run (hold)                                         |              |
| L / R  | Strafe                                             |              |
| START  | Menu                                               | Close menu   |
| SELECT | Automap on the touch screen                        |              |
| Stylus | Drag to turn, tap the weapon, map and zoom buttons |              |

New savegames are named after the level and time. To rename one, UP/DOWN
change the last letter, RIGHT adds a letter and LEFT or B removes one.
Sound always plays at full volume, use the DS volume slider.

## How it works on the DS

- The 320x200 frame is scaled to 256x192 by the affine background hardware,
  and a second, alpha blended background layer sampling half a pixel further
  smooths it.
- Doom's palette lives in the hardware palette, so damage and pickup flashes
  cost nothing.
- The automap is drawn live on the touch screen while you keep playing on the
  top screen. Without it the touch screen shows level stats, ammo and keys.
- Sound effects play on hardware channels 0-7, which do the resampling, volume
  and stereo panning.
- Music (MUS and MIDI) is played by a wavetable synth on hardware channels
  8-15, with waveforms per instrument family and synthesized drums, sequenced
  by a 140 Hz thread.
- The hot rendering loops run from ITCM, and fixed point division uses the
  hardware divider.
- In DSi mode all 16 MB of RAM is used for the zone, the DS uses its 4 MB.
