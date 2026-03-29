#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "display.h"
#include "network.h"

struct SystemModule {

    void reset() {
        config.reset();
        network.resetWiFi();
    }

    String getSettingsSummary() {
        return "System Ready\n\nWiFi: " + network.lastSSID + 
               "\nNTP: " + config.ntpServer + 
               "\nTZ: " + config.timezone;
    }



    void handleFactoryReset() {
        Serial.println("Checking for Factory Reset (Press & Hold BOOT)...");
        uint32_t startTime = millis();
        bool resetTriggered = false;
        
        pinMode(9, INPUT_PULLUP);
        while (millis() - startTime < 3000) {
            if (digitalRead(9) == LOW) {
                display.showStatusMessage("Keep holding BOOT\nto reset...");
                delay(5000);
                if (digitalRead(9) == LOW) {
                    resetTriggered = true;
                    break;
                }
            }
            delay(100);
        }

        if (resetTriggered) {
            Serial.println("Factory Reset Triggered!");
            display.showStatusMessage("Resetting Settings...");
            reset();
            network.startSetupMode();
        }
    }

};

SystemModule sys;
