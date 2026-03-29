#include "src/system.h"
#include "src/network.h"
#include "src/power.h"
#include "src/display.h"
#include "src/clock/clock_logic.h"
#include "src/time_sync.h"

uint8_t hh, mm;

void setup() {
    Serial.begin(115200);
    delay(1000); 
    
    config.begin();
    // Initialize hardware and perform a full screen wipe
    display.begin();

    sys.handleFactoryReset();
    
    if (!network.connectWiFi()) {
        network.startSetupMode();
    }

    network.syncNTP();

    // Show settings summary with a 60-second countdown (Fast partial updates)
    // This loop also saves time to reupload new code in the next 60 seconds - do not delete it!
    for (int seconds = 60; seconds >= 0; seconds -= 5) {
        String msg = sys.getSettingsSummary() + "\n\nClock starting in: " + String(seconds) + "s";
        display.showStatusMessage(msg.c_str()); 
        if (seconds > 0) delay(5000);
    }

    // Explicitly wipe the screen one last time to remove ghosting
    display.fullRefresh();


}

void loop() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        static int lastMin = -1;
        
        timeSync.syncPeriodically(timeinfo);

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