#pragma once

#include "../display.h"
#include "words.h"

extern uint8_t hh, mm;

struct ClockLogicModule {

    const unsigned char* getHourBitmap(uint8_t hour, size_t &outSize) {
        switch(hour) {
            case 1: outSize = sizeof(epd_bitmap_one); return epd_bitmap_one;
            case 2: outSize = sizeof(epd_bitmap_two); return epd_bitmap_two;
            case 3: outSize = sizeof(epd_bitmap_three); return epd_bitmap_three;
            case 4: outSize = sizeof(epd_bitmap_four); return epd_bitmap_four;
            case 5: outSize = sizeof(epd_bitmap_five); return epd_bitmap_five;
            case 6: outSize = sizeof(epd_bitmap_six); return epd_bitmap_six;
            case 7: outSize = sizeof(epd_bitmap_seven); return epd_bitmap_seven;
            case 8: outSize = sizeof(epd_bitmap_eight); return epd_bitmap_eight;
            case 9: outSize = sizeof(epd_bitmap_nine); return epd_bitmap_nine;
            case 10: outSize = sizeof(epd_bitmap_ten); return epd_bitmap_ten;
            default: outSize = 0; return NULL;
        }
    }

    const unsigned char* getToHourBitmap(uint8_t hour, size_t &outSize) {
        switch(hour) {
            case 1: outSize = sizeof(epd_bitmap_to_one); return epd_bitmap_to_one;
            case 2: outSize = sizeof(epd_bitmap_to_two); return epd_bitmap_to_two;
            case 3: outSize = sizeof(epd_bitmap_to_three); return epd_bitmap_to_three;
            case 4: outSize = sizeof(epd_bitmap_to_four); return epd_bitmap_to_four;
            case 5: outSize = sizeof(epd_bitmap_to_five); return epd_bitmap_to_five;
            case 6: outSize = sizeof(epd_bitmap_to_six); return epd_bitmap_to_six;
            case 7: outSize = sizeof(epd_bitmap_to_seven); return epd_bitmap_to_seven;
            case 8: outSize = sizeof(epd_bitmap_to_eight); return epd_bitmap_to_eight;
            case 9: outSize = sizeof(epd_bitmap_to_nine); return epd_bitmap_to_nine;
            case 10: outSize = sizeof(epd_bitmap_to_ten); return epd_bitmap_to_ten;
            default: outSize = 0; return NULL;
        }
    }

    const unsigned char* getAndOnesBitmap(uint8_t units, size_t &outSize) {
        switch(units) {
            case 1: outSize = sizeof(epd_bitmap_and_one); return epd_bitmap_and_one;
            case 2: outSize = sizeof(epd_bitmap_and_two); return epd_bitmap_and_two;
            case 3: outSize = sizeof(epd_bitmap_and_three); return epd_bitmap_and_three;
            case 4: outSize = sizeof(epd_bitmap_and_four); return epd_bitmap_and_four;
            case 5: outSize = sizeof(epd_bitmap_and_five); return epd_bitmap_and_five;
            case 6: outSize = sizeof(epd_bitmap_and_six); return epd_bitmap_and_six;
            case 7: outSize = sizeof(epd_bitmap_and_seven); return epd_bitmap_and_seven;
            case 8: outSize = sizeof(epd_bitmap_and_eight); return epd_bitmap_and_eight;
            case 9: outSize = sizeof(epd_bitmap_and_nine); return epd_bitmap_and_nine;
            default: outSize = 0; return NULL;
        }
    }

    void updateClockDisplay(bool fullRefresh = false) {
        display.epaper.fillScreen(GxEPD_WHITE);
        display.total_word_count = 0; 
        size_t tempSize;

        uint8_t current_hh = hh % 12;
        if (current_hh == 0) current_hh = 12;
        uint8_t next_hh = (current_hh % 12) + 1;

        if (mm == 45 || mm == 50 || mm == 55) {
            if (mm == 45) display.addWord(epd_bitmap_quarter, sizeof(epd_bitmap_quarter));
            else if (mm == 50) display.addWord(epd_bitmap_ten_teen, sizeof(epd_bitmap_ten_teen));
            else if (mm == 55) display.addWord(epd_bitmap_five_minutes, sizeof(epd_bitmap_five_minutes));
            
            if (next_hh == 11) { display.addWord(epd_bitmap_to_one, sizeof(epd_bitmap_to_one)); display.addWord(epd_bitmap_ten_teen, sizeof(epd_bitmap_ten_teen)); }
            else if (next_hh == 12) { display.addWord(epd_bitmap_to_two, sizeof(epd_bitmap_to_two)); display.addWord(epd_bitmap_ten_teen, sizeof(epd_bitmap_ten_teen)); }
            else { 
                const unsigned char* bmp = getToHourBitmap(next_hh, tempSize);
                display.addWord(bmp, tempSize); 
            }
        } 
        else {
            if (current_hh == 11) { display.addWord(epd_bitmap_one, sizeof(epd_bitmap_one)); display.addWord(epd_bitmap_ten_teen, sizeof(epd_bitmap_ten_teen)); }
            else if (current_hh == 12) { display.addWord(epd_bitmap_two, sizeof(epd_bitmap_two)); display.addWord(epd_bitmap_ten_teen, sizeof(epd_bitmap_ten_teen)); }
            else { 
                const unsigned char* bmp = getHourBitmap(current_hh, tempSize);
                display.addWord(bmp, tempSize);
            }

            if (mm != 0) {
                if (mm == 5) display.addWord(epd_bitmap_and_five_minutes, sizeof(epd_bitmap_and_five_minutes));
                else if (mm == 10) display.addWord(epd_bitmap_and_ten_teen, sizeof(epd_bitmap_and_ten_teen)); 
                else if (mm == 15) display.addWord(epd_bitmap_and_quarter, sizeof(epd_bitmap_and_quarter)); 
                else if (mm == 20) display.addWord(epd_bitmap_and_twenty, sizeof(epd_bitmap_and_twenty));
                else if (mm == 30) display.addWord(epd_bitmap_and_half, sizeof(epd_bitmap_and_half)); 
                else if (mm == 40) display.addWord(epd_bitmap_and_forty, sizeof(epd_bitmap_and_forty));
                else if (mm == 1) display.addWord(epd_bitmap_and_min, sizeof(epd_bitmap_and_min));
                else if (mm == 2) { display.addWord(epd_bitmap_and_two_min, sizeof(epd_bitmap_and_two_min)); display.addWord(epd_bitmap_and_mins, sizeof(epd_bitmap_and_mins)); }
                else if (mm >= 3 && mm <= 9) { 
                    const unsigned char* bmp = getAndOnesBitmap(mm, tempSize);
                    display.addWord(bmp, tempSize); 
                    display.addWord(epd_bitmap_and_mins, sizeof(epd_bitmap_and_mins)); 
                }
                else if (mm == 12) { display.addWord(epd_bitmap_and_two_teen, sizeof(epd_bitmap_and_two_teen)); display.addWord(epd_bitmap_ten_teen, sizeof(epd_bitmap_ten_teen)); display.addWord(epd_bitmap_and_mins, sizeof(epd_bitmap_and_mins)); }
                else if (mm == 13) { display.addWord(epd_bitmap_and_three_teen, sizeof(epd_bitmap_and_three_teen)); display.addWord(epd_bitmap_ten_teen, sizeof(epd_bitmap_ten_teen)); display.addWord(epd_bitmap_and_mins, sizeof(epd_bitmap_and_mins)); }
                else if (mm == 19) { display.addWord(epd_bitmap_and_nine_teen, sizeof(epd_bitmap_and_nine_teen)); display.addWord(epd_bitmap_ten_teen, sizeof(epd_bitmap_ten_teen)); display.addWord(epd_bitmap_and_mins, sizeof(epd_bitmap_and_mins)); }
                else if (mm >= 11 && mm <= 18) {
                    const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                    display.addWord(bmp, tempSize);
                    display.addWord(epd_bitmap_ten_teen, sizeof(epd_bitmap_ten_teen));
                    display.addWord(epd_bitmap_and_mins, sizeof(epd_bitmap_and_mins));
                }
                else if (mm >= 21 && mm <= 29) {
                    display.addWord(epd_bitmap_twenty, sizeof(epd_bitmap_twenty));
                    const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                    display.addWord(bmp, tempSize);
                }
                else if (mm >= 31 && mm <= 39) {
                    display.addWord(epd_bitmap_thirty, sizeof(epd_bitmap_thirty));
                    const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                    display.addWord(bmp, tempSize);
                }
                else if (mm >= 41 && mm <= 49) {
                    display.addWord(epd_bitmap_forty, sizeof(epd_bitmap_forty));
                    const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                    display.addWord(bmp, tempSize);
                }
                else if (mm >= 51 && mm <= 59) {
                    display.addWord(epd_bitmap_fifty, sizeof(epd_bitmap_fifty));
                    const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                    display.addWord(bmp, tempSize);
                }
            }
        }

        if (hh >= 6 && hh < 12) display.addWord(epd_bitmap_in_the_morning, sizeof(epd_bitmap_in_the_morning));
        else if (hh >= 12 && hh < 15) display.addWord(epd_bitmap_noon, sizeof(epd_bitmap_noon));
        else if (hh >= 15 && hh < 18) display.addWord(epd_bitmap_afternoon, sizeof(epd_bitmap_afternoon));
        else if (hh >= 18 && hh < 22) display.addWord(epd_bitmap_in_the_evening, sizeof(epd_bitmap_in_the_evening));
        else if (hh >= 22 || hh < 6) display.addWord(epd_bitmap_at_night, sizeof(epd_bitmap_at_night));

        
        display.renderAllWords();
        display.epaper.display(!fullRefresh); 
    }

    void updateTimeAndDisplay() {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) return;
        hh = timeinfo.tm_hour; mm = timeinfo.tm_min;
        updateClockDisplay(true);
    }

};

ClockLogicModule timeLogic;
