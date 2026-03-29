#pragma once

#include <Arduino.h>
#include <time.h>
#include "network.h"

struct TimeSyncModule {
    void syncPeriodically(struct tm& timeinfo) {
        static int lastSyncHour = -1;
        
        if (timeinfo.tm_hour % 6 == 0 && timeinfo.tm_hour != lastSyncHour) {
            network.syncNTP();
            lastSyncHour = timeinfo.tm_hour;
            getLocalTime(&timeinfo); // Update timeinfo reference after sync
        }
    }
};

TimeSyncModule timeSync;
