#pragma once

#include <Arduino.h>
#include <esp_sleep.h>

struct PowerModule {

    void sleepUntilNextMinute(int current_second) {
        int seconds_remaining = 60 - current_second;
        
        // Intelligent Battery Saver: Puts the CPU silicon into Light Sleep.
        // This drops power from ~30mA to less than ~1mA between minute ticks.
        esp_sleep_enable_timer_wakeup(seconds_remaining * 1000000ULL);
        esp_light_sleep_start();
    }

};

PowerModule power;
