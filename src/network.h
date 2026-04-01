#pragma once

#include <WiFiManager.h>
#include <WiFi.h>
#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "time.h"
#include <esp_sntp.h>

struct NetworkModule {

    const char* portalSSID = "HebrewClock-Setup";
    String lastSSID = "Not Connected";


    bool connectWiFi() {
        WiFiManager wm;
        // Don't start a portal during a connection attempt
        wm.setEnableConfigPortal(false);
        wm.setConnectTimeout(15); // Try for 15 seconds

        Serial.println("Attempting WiFi Connection...");

        if (wm.autoConnect(portalSSID)) {
            lastSSID = WiFi.SSID();
            Serial.println("WiFi Connected Successfully!");
            return true;
        } else {

            Serial.println("WiFi Connection Failed. Portal may be required.");
            return false;
        }
    }

    void startSetupMode() {
        WiFiManager wm;
        
        // 1. Prepare custom input boxes
        WiFiManagerParameter custom_ntp("ntp", "NTP Server", config.ntpServer.c_str(), 40);
        WiFiManagerParameter custom_tz("tz", "Timezone Offset", config.timezone.c_str(), 40);
        
        // 2. Set up the instruction screen callback using a lambda
        wm.setAPCallback([this](WiFiManager *myWM) {
            Serial.println("Entered Configuration Mode");
            String msg = "Connect to WiFi:\n'" + String(portalSSID) + "'\nto configure.";
            display.showStatusMessage(msg.c_str());
        });

        // 3. Configure portal behavior
        wm.setConfigPortalTimeout(180); 
        wm.addParameter(&custom_ntp);
        wm.addParameter(&custom_tz);

        Serial.println("Starting Manual Setup Portal...");
        
        if (!wm.startConfigPortal(portalSSID)) {
            Serial.println("Portal Timeout - Continuing...");
        } else {
            // Save parameters only if the user finished the portal
            display.showStatusMessage("Settings Saved.\nRestarting...");
            config.save(custom_ntp.getValue(), custom_tz.getValue());
            delay(3000);
            ESP.restart();
        }
    }

    void resetWiFi() {

        WiFiManager wm;
        wm.resetSettings();
        Serial.println("WiFi Credentials Wiped.");
    }

    void disconnectWiFi() {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        Serial.println("WiFi turned OFF to save battery.");
    }

    void syncNTP() {
        if (connectWiFi()) {
            Serial.println("Syncing time...");
            sntp_set_sync_status(SNTP_SYNC_STATUS_RESET);
            configTzTime(config.timezone.c_str(), config.ntpServer.c_str(), "time.google.com");
            struct tm timeinfo;
            // Wait for time to be synced via NTP status or year check
            int retries = 0;
            while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
                if (retries > 40) {
                    Serial.println("Time Sync Failed via NTP!");
                    display.showStatusMessage("Time Sync Failed.\nRetrying later...");
                    delay(30000);
                    break; // Timeout after 20 seconds
                }
                delay(500);
                retries++;
            }
        } else {
            display.showStatusMessage("WiFi Connection Failed. Portal may be required.");
            delay(5000); // Give the user 5 seconds to read the error
        }
        
        disconnectWiFi();
    }

};

NetworkModule network;
