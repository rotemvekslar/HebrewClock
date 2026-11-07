#pragma once

#include <Preferences.h>
#include <Arduino.h>

// Israel time zone (POSIX TZ, same as the tz database footer for Asia/Jerusalem).
// DST starts on the Friday before the last Sunday of March at 02:00
// ("Thursday of the 4th week, 26:00" = that Friday 02:00) and ends on the last Sunday of October.
#define DEFAULT_TIMEZONE "IST-2IDT,M3.4.4/26,M10.5.0"

struct ConfigModule {
    Preferences preferences;

    String ntpServer;
    String timezone;

    void begin() {
        preferences.begin("hebrew-clock", false);
        ntpServer = preferences.getString("ntp_server", "time.google.com");
        timezone = preferences.getString("timezone", DEFAULT_TIMEZONE);
    }

    void save(String ntp, String tz) {
        ntpServer = ntp;
        timezone = tz;
        preferences.putString("ntp_server", ntpServer);
        preferences.putString("timezone", timezone);
    }

    void reset() {
        preferences.clear();
        ntpServer = "time.google.com";
        timezone = DEFAULT_TIMEZONE;
    }
};

ConfigModule config;
