# Zone drawing for Practwist CT2

## Using it
Open the menu (`R + D-pad down`) and go to the **Warp To Zone** page (D-pad right twice). Two new options are there:

- **Draw Zones**: `Off` / `All` / `Near`
  - `All`: every zone in the current area.
  - `Near`: only the zone you're in plus the 2 zones on either side of it by index.
- **Zone Labels**: `Off` / `On`. Shows the zone number at the center of each box. The numbers match the HUD's `Zone: XX` readout and the names in `zones.c`. If Draw Zones is Off, the labels still show for every zone.

How it looks:
- Each zone is a see-through colored box with bright outlines.
- The zone you're in has white outlines and a white label.
- Outlines are drawn for the 16 zones closest to the camera, so a big area doesn't get too heavy to draw.
- Labels are capped at 10 per frame and always leave 16 characters of the game's 100-character text limit free.

Both options are saved by **Save Settings**. Older `ct2.bin` files load fine: the new values are checked at boot and reset to Off if they're invalid.

## Building
A new source file was added (`src/zone_draw.c`), so **re-run `configure.py`** before `ninja` to regenerate `build.ninja` and `asm/main.asm`.

## Files changed
| File | Change |
|---|---|
| `src/zone_draw.c` | New: reads the zone table, builds the display list, draws the labels, and holds the menu toggles |
| `src/main.c` | Saves the camera matrices in `func_80035E00_Hook`; calls `DrawZoneLabels()` from the text hook; new toggle defaults; checks the toggles after loading settings |
| `src/menu.c` | Two new options on page 2 |
| `include/ct2.h` | New toggle enum entries and prototypes; `gZones` size corrected to 81 |
| `asm/symbols.asm` | `func_80036CF0`, `gTextCharsMax`, `gTextCharsUsed` |
| `asm/patches.asm` | `jal` at `0x800371D4` changed to `ZoneRenderHook` |
| `asm/custom_asm.s` | **Bug fix in `zoneSetCheck`** (see below) |

`zone-drawing.diff` contains all of these changes as one diff.

## How it works
- **Zone table** (`0x801857F0`, 0x30-byte entries): the code follows the game's own check at `0x80054F10`. It starts at index 1 and stops at index 80, at an entry whose `unk0` is NULL, or at an entry whose `*(s16*)unk0 == 6`. Min/max are cut down to s16 and swapped if reversed. The test is `min <= pos <= max`, and the first matching index wins, so where boxes overlap, the lower number takes priority.
- **When it draws:** after the 3D world (`0x80036E30`) and before the orthographic HUD-model pass (`0x80036CF0`). It reloads the world camera's projection and view matrices, which were saved earlier in the frame. Then it draws the faces with `G_RM_ZB_XLU_SURF`, which is depth-tested, doesn't write depth, and has culling off so boxes are visible from inside. Afterwards it puts the geometry mode back to `ZBUFFER | SHADE | CULL_BACK`, which is what the game's own display list has at that point.
- **Outlines:** F3DEX2 can't draw lines, so each edge is a thin quad turned to face the camera. Its width grows with distance so it stays about 2–3 px wide on screen.
- **Memory:** the display list and vertices are double-buffered in the payload and written back from the CPU cache before use. The payload grows to about `0x80430740`, which is still below the savestate area at `0x80440000`.

Settings you can change are the `#define`s at the top of `zone_draw.c`: alphas, line width, the Near range, the outline/label caps, and the palette.

## Bug fix: `zoneSetCheck` damaged `t5`
`zoneSetCheck` does `LI t5, zoneLockout` and then jumps back to `0x80055124`. The game's next instruction there is `LH t5, 0xAE00 (t5)`, which expects `t5 = 0x800F0000` so it can read `gRespawnZone`. Instead, the game read whatever happened to be at `&zoneLockout - 0x5200`. Whether that value was 0 depended only on where the linker placed things.

When it was 0, the code that loads zone objects was skipped: the zone table stayed empty and **the player fell through the stage when it loaded**. This happened in testing whenever the payload grew, even with nothing but an unused 72 KB array added. The fix puts `LUI t5, 0x800F` in the delay slot of that `J`.

## Warp To Zone (rewritten)
`ZoneWarp` no longer sets `gRespawnZone` and pushes the player out of bounds. It now puts the player directly on the zone's respawn point, the same way the game's void-out respawn does (`0x8004E3BC`), minus the HP loss. The code is in `src/zone_warp.c`.

What it does:
1. Resets zone objects with `0x8005564C` and `0x8003C9DC`, like the game does. The game skips this in areas `0x15`–`0x1A`, and so does the warp.
2. Sets `gRespawnZone` to the target zone and puts the player on that zone's `respawnX/Y/Z`.
3. Zeroes velocity and clears the same flags as a respawn, and snaps the camera to the respawn camera position (eye = y+1000, z+1000).
4. Gives the normal 90-frame post-respawn blink (state 5), then sets `gZoneChange`.

The menu only queues the warp; it actually happens in `perFrameCFunction`, during game logic. Zone numbers that don't exist in the current area are ignored.

Note: a few respawn points sit inside a *lower-numbered* zone's box (for example, Sky Land zone 2's respawn point is inside zone 1). The game's zone check reassigns you to the lower zone on the next frame, exactly as it does after a real void-out.

Files: `src/zone_warp.c` (new), `src/menu.c` (`ZoneWarp`), `src/main.c` (calls `ZoneWarpUpdate()`), `include/ct2.h`, `asm/symbols.asm`.

## Version text on the logo
`src/version_text.c` draws `PRACTWIST V0.7.0` under the Japan System Supply logo. Change the version in `PRACTWIST_VERSION` in `include/ct2.h`.

The game's own debug text can't be used on that screen. The logo screen doesn't load the font texture (`0x37`), so `printDebugText` queues the characters but they draw invisible. Instead, the crash screen's 6×7 font is drawn as solid rectangles, one per run of lit pixels, at 2× scale, centered. This needs no texture, so it works under any renderer.

It runs from `ZoneRenderHook`, after the HUD-model pass, and only when `gGameMode == 18` and `gSelectionScreen` (`0x800F0934`) `== 1`. That value is 2 on the title screen, so the text doesn't stay up there.

## Savestate fixes (accurate emulators / console)
Tested in ares (built from source, LLE RSP + paraLLEl-RDP), reading state through its GDB server.

1. **Saves and loads happened while the RSP/RDP were still working.** I recorded the hardware registers at the moment of 15 saves. Every one had `SP_STATUS` with HALT clear (an RSP task running) and `DPC_STATUS = 0xA8` (RDP pipe busy). The old "wait" loops only checked DMA bits (`__osSpDeviceBusy`, and `__osDpDeviceBusy` checks only `0x100`), so they never waited for the task itself.
   - **Save** captured a scheduler that was still waiting for a "task done" from the save timeline.
   - **Load** overwrote RAM while the current frame's RSP/RDP task was still reading its display list and vertices from it.

   Whether this works depends on timing. mupen's HLE RSP finishes tasks instantly, so it never shows up there.

   **Fix:** `QuiesceRcpAndDisableInt()` in `main.c`. With interrupts still **on**, it waits until the RSP is halted, the RDP is idle (`DPC_STATUS & 0x170` clear), PI and SI are idle, and no SP/SI/PI/DP interrupt is pending. The OS delivers the completion messages to the scheduler as usual. The save/load thread has priority 255, so nothing else can start new work in the meantime. Only then are interrupts disabled for the copy. There's a ~250 ms timeout so a wedged RDP can't hang it; `gSavestateQuiesceTimeouts` counts timeouts and stayed 0 in testing.

2. **Loading is now instant.** D-pad right used to send the game through its boot sequence (game mode 0, then 2) and copy the snapshot a few frames later. That was a workaround for the hardware problem above. Now D-pad right starts the load thread from the same place D-pad left starts the save thread (`func_800293F0_Hook2`). Once RAM is restored, the main thread resumes right where it was when you saved. Pressing load with nothing saved does nothing.
   - Removed with it: the mode-2 pending-load code, `isLoadPending`, and the 6-frame display freeze on load.
   - The reboot window also allowed a save in the middle of the reset, which saved a half-booted game. That can't happen anymore, so saving works in any mode again.

3. Small cleanup in `loadstateMain`: it now writes back the D-cache first and then invalidates the I-cache, so restored overlay code is in RAM before stale instructions are dropped.

Results in ares:
- Randomized save/load/move/jump stress: the old build fell out of gameplay; the instant-load build stayed in gameplay for 60 steps with no quiesce timeouts.
- Warping far across the level and loading back: worked.

mupen still works.

Not reproduced: an actual crash screen in ares. If one still happens, the crash screen's thread/PC/cause line will show where.

## Something else I noticed (not changed)
The player's position is at `0x80187B00`, which is what the game's zone check reads. The `Player` struct's offsets are correct for a base of `0x80187290`; for example, `hp` at `+0x92C` is `0x80187BBC` = `playerHealth`. But `symbols.asm` defines `gPlayerActors` as `0x80187B00`, so `gPlayerActors[0].pos` reads `0x80188370`, which was all zeros in my RAM dumps.

That affects `PrintPosition` (the old `ZoneWarp` line is gone; `zone_warp.c` uses the correct address through `gPlayerRaw`). Changing the label to `0x80187290` would likely fix both. Check the other `gPlayerActors` uses first.

## Testing
Tested in mupen64plus (GLide64mk2, HLE RSP) with the attached `.bps`, built with mips-linux-gnu-gcc + armips:
- Sky Land: boxes match the level, and the HUD zone changes exactly at the drawn boundary.
- All / Near / labels modes.
- HUD models after a respawn.
- Pause menu: zones are hidden.
- Mod savestate save and load.
- FPS readout unchanged (28.9).

Not yet tested on real hardware or an accurate emulator like ares or parallel-RDP. That's the next thing to check.
