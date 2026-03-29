#pragma once

#include <Preferences.h>
#include <Arduino.h>

struct ConfigModule {
    Preferences preferences;

    String ntpServer;
    String timezone;

    void begin() {
        preferences.begin("hebrew-clock", false);
        ntpServer = preferences.getString("ntp_server", "time.google.com");
        timezone = preferences.getString("timezone", "IST-2IDT,M3.5.5/2,M10.5.0/2");
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
        timezone = "IST-2IDT,M3.5.5/2,M10.5.0/2";
    }
};

ConfigModule config;
