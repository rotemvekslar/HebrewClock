#pragma once

#include <WiFi.h>
#include <Arduino.h>
#include "time.h"

struct NetworkModule {

    bool connectWiFi(const char* ssid, const char* password) {
        Serial.print("Connecting to WiFi");
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20) {
            delay(500);
            Serial.print(".");
            retries++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi Connected!");
            return true;
        } else {
            Serial.println("\nError: Could not connect to WiFi.");
            return false;
        }
    }

    void disconnectWiFi() {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        Serial.println("WiFi turned OFF to save battery.");
    }

    void syncNTP(const char* ssid, const char* password) {
        if (connectWiFi(ssid, password)) {
            Serial.println("Syncing time...");
            configTzTime("IST-2IDT,M3.5.5/2,M10.5.0/2", "pool.ntp.org", "time.google.com");
            struct tm timeinfo;
            while (!getLocalTime(&timeinfo) || timeinfo.tm_year < (2026 - 1900)) {
                delay(500);
            }
        }
        
        disconnectWiFi();
    }

};

NetworkModule network;
