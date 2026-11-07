// One-time E-Ink panel cleaning sketch for the HebrewClock (GDEY075T7, 800x480).
//
// Purpose: remove accumulated ghosting / grey areas / smears caused by long-term
// partial refreshes and the panel being left powered between updates.
//
// How to use:
//   1. (Recommended) In the GxEPD2 library, open src/gdey/GxEPD2_750_GDEY075T7.h
//      and set:  static const bool useFastFullUpdate = false;
//      This makes full refreshes use the slow, temperature-compensated waveform,
//      which cleans much better than the fast one.
//   2. Open this sketch in Arduino IDE, select "XIAO ESP32-C3", upload.
//      (Press Reset on the clock first - the clock sketch gives a 60s upload window.)
//   3. Open Serial Monitor at 115200 to watch progress. Takes ~3-5 minutes.
//   4. When it prints "DONE", upload HebrewClock.ino again.
//
// Run it in a room-temperature environment (not cold) for best results.

#include <GxEPD2_BW.h>

// Same wiring as src/display.h
GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> epaper(
    GxEPD2_750_GDEY075T7(/*CS=*/ D1, /*DC=*/ D3, /*RST=*/ D0, /*BUSY=*/ D2));

const int CYCLES = 15;           // black/white cycles
const int SETTLE_MS = 1000;      // pause between refreshes

void fullFill(uint16_t color) {
    epaper.fillScreen(color);
    epaper.display(false);       // full refresh; GxEPD2 powers the panel off afterwards
    delay(SETTLE_MS);
}

// Checkerboard in both phases so every pixel gets driven in both directions
// relative to its neighbours (helps with edge ghosting around old letters).
void checker(bool invert, int cell) {
    epaper.fillScreen(GxEPD_WHITE);
    for (int y = 0; y < epaper.height(); y += cell) {
        for (int x = 0; x < epaper.width(); x += cell) {
            bool black = (((x / cell) + (y / cell)) & 1) ^ invert;
            if (black) epaper.fillRect(x, y, cell, cell, GxEPD_BLACK);
        }
    }
    epaper.display(false);
    delay(SETTLE_MS);
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\nE-Ink cleaning started");

    epaper.init(115200, true, 2, false);
    epaper.setRotation(0);

    for (int i = 1; i <= CYCLES; i++) {
        Serial.printf("Cycle %d/%d\n", i, CYCLES);
        fullFill(GxEPD_BLACK);
        fullFill(GxEPD_WHITE);
    }

    Serial.println("Checkerboard pass");
    for (int i = 0; i < 3; i++) {
        checker(false, 40);
        checker(true, 40);
    }

    // Finish on clean white, then put the controller into deep sleep
    Serial.println("Final white passes");
    fullFill(GxEPD_WHITE);
    fullFill(GxEPD_WHITE);
    epaper.hibernate();

    Serial.println("DONE - you can now re-upload HebrewClock.ino");
}

void loop() {
    delay(10000);
}
