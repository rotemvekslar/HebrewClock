#include "src/network.h"
#include "src/power.h"
#include "src/display.h"
#include "src/clock/clock_logic.h"
#include "arduino_private.h"

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;

uint8_t hh, mm;

void setup() {
    Serial.begin(115200);
    
    network.syncNTP(ssid, password);

    struct tm timeinfo;
    getLocalTime(&timeinfo);
    hh = timeinfo.tm_hour;
    mm = timeinfo.tm_min;

    display.epaper.init(115200, true, 2, false); 
    display.epaper.setRotation(0); 
    timeLogic.updateClockDisplay(true);

    // Safety feature: Do not go into light sleep during the first 60 seconds after booting.
    // This gives you a generous 1-minute window to easily upload new code without MASHING the reset button!
    delay(60000);
}

void loop() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        static int lastMin = -1;
        static int lastSyncDay = -1;
        
        if (timeinfo.tm_hour == 3 && timeinfo.tm_mday != lastSyncDay) {
            network.syncNTP(ssid, password);
            lastSyncDay = timeinfo.tm_mday;
            getLocalTime(&timeinfo);
        }

        if (timeinfo.tm_min != lastMin) {
            hh = timeinfo.tm_hour;
            mm = timeinfo.tm_min;
            bool fullRefresh = (lastMin == -1 || mm == 0); 
            lastMin = mm;
            
            Serial.printf("Real Time Update: %02d:%02d\n", hh, mm);
            timeLogic.updateClockDisplay(fullRefresh);
        }
        
        power.sleepUntilNextMinute(timeinfo.tm_sec);
    } else {
        delay(1000);
    }
}