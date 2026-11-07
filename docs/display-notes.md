# E-Paper Display Notes

Investigation of the faded / striped display (November 2026), what was tried, the current
solution, and recommendations for a replacement panel.

## Hardware

- MCU: Seeed XIAO ESP32-C3, in the Seeed 7.5" ePaper panel enclosure, runs on battery
- Panel: 7.5" 800x480 black/white, UC8179 controller (GDEY075T7)
- Library: GxEPD2 1.6.8, **unmodified**
- Arduino IDE: board `XIAO_ESP32C3`, Partition Scheme **Huge APP (3MB No OTA/1MB SPIFFS)**.
  The sketch no longer fits the default 1.2MB partition since WiFiManager was added.

## Symptoms

After 1-2 months of good operation the display started showing faded areas, grey patches,
smears and vertical stripes. The time became hard to read.

- Fading also appeared in areas where no text is ever drawn.
- The stripes moved between refreshes. They were not fixed lines.
- Wall power was a little better than USB from a PC, but not good.

## What the code did at the time

- Every minute: partial refresh with `GxEPD2_750_GDEY075T7`. This uses the panel's OTP waveform
  with a **forced temperature of 110°C**, which makes it fast and quiet.
- Every hour: full refresh with `useFastFullUpdate = true`, which forces **90°C**.
- **No `powerOff()` after partial refreshes.** `GxEPD2_BW::display()` only powers the panel off
  after a full refresh (`GxEPD2_BW.h`: `if (!partial_update_mode) epd2.powerOff();`). The panel
  driving voltages therefore stayed on about 59 minutes of every hour, for months. The GxEPD2
  header comment on `powerOff()` says it "avoids screen fading over time".

## Experiments

| Test | Result |
|------|--------|
| One-time cleaning sketch (`tools/EpdClean`), fast waveform | Mostly white, grey patch top-centre remained |
| Cleaning sketch with slow full waveform (`useFastFullUpdate = false`) | Perfectly white, checkerboard sharp |
| Clock after cleaning, original settings | Faded and striped **immediately** |
| Full refresh, slow waveform at real temperature | ✅ Sharp and clean, but whole screen flashes for a few seconds |
| Full refresh, fast waveform at forced 90°C | ❌ Black not reached, smears, sometimes stays white |
| Partial refresh, OTP waveform at forced 110°C (original) | ❌ Faded and striped |
| Partial refresh, forced 90°C / 85°C | ❌ Screen stays white |
| Partial refresh, forced 80°C / 70°C | ✅ Sharp, but whole screen flashes (same as a full refresh) |
| Partial refresh, GxEPD2 register LUTs (`useFastPartialUpdateFromOTP = false`) | ❌ Worse: dots and stripes |
| `GxEPD2_750_T7` driver (GDEW075T7), partial refresh | ❌ Smears. Its full refresh is clean |
| Partial-window refresh with the clean OTP waveform (only the changed area) | ❌ Bad overall |
| Inverted colours (white text on black) | ❌ Bad |

**Conclusion:** on this panel only the slow full waveform at the real temperature still gives a
clean image. Every faster waveform fails, including ones that worked well in the first months.
So the panel itself has changed. The cause is not proven. It may be wear, or months of driving
voltage being left on.

## Current solution

- `src/display.h` uses the `GxEPD2_750_T7` driver. Its full refresh uses the panel's real
  temperature, so the GxEPD2 library can stay unmodified.
- `src/clock/clock_logic.h` does a **full refresh on every minute change**. The trade-off is a
  flash of a few seconds each minute, accepted in exchange for a sharp image.
- `powerOff()` is called after partial refreshes. The boot status screen and countdown still use
  partial refresh.
- Unrelated fix: the Israel DST rule is now `IST-2IDT,M3.4.4/26,M10.5.0` (`src/config.h`). A
  clock that saved the old string through the setup portal keeps it until a factory reset (hold
  BOOT for 5 seconds at power-on). The System Ready screen shows the active `TZ:`.

## Recommendations for a new panel

1. **Same type of panel:** 7.5" 800x480 B/W, UC8179 (GDEY075T7). No hardware or wiring changes
   are needed.
2. **First, test the original quiet mode.** Switch `src/display.h` back to
   `GxEPD2_750_GDEY075T7`, and in `clock_logic.h` use `display(!fullRefresh)` again. On a healthy
   panel, partial refresh should be quiet and sharp.
3. **Always power off after every refresh.** Keep `epaper.powerOff()` right after every
   `display(...)` call. This is the most important protection.
4. **Do a clean full refresh regularly.** Partial refreshes accumulate residue. Keep the hourly
   full refresh. Also consider a short cleaning cycle once a night (e.g. at 03:00, during the NTP
   sync): two or three black/white full refreshes. For that cleaning, use the slow
   real-temperature waveform rather than the forced-90°C fast one.
5. **Limit partial refreshes at boot.** The 60-second countdown does 13 partial refreshes. Update
   every 15 seconds instead of every 5.
6. **Watch for early signs.** If grey patches or stripes start to appear, run
   `tools/EpdClean/EpdClean.ino` once. Do not wait months.
7. **Battery.** Make sure the battery is healthy and charged. A weak supply visibly degrades
   refresh quality.

## Reference: GxEPD2 settings involved

In `libraries/GxEPD2/src/gdey/GxEPD2_750_GDEY075T7.h` / `.cpp`. All of these are currently at
their original values.

| Setting | Original | Meaning |
|---------|----------|---------|
| `.h` `useFastFullUpdate` | `true` | `true` = full refresh at forced 90°C (fast). `false` = real temperature (slow, clean) |
| `.h` `useFastPartialUpdateFromOTP` | `true` | `false` = GxEPD2's own register LUTs for partial refresh |
| `.cpp` `_Init_Part`: `_writeData(0x6E); // 110` | `0x6E` | Forced temperature used to select the partial waveform |
