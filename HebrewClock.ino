#include <WiFi.h>
#include "time.h"
#include <TFT_eSPI.h> 
#include <SPI.h>
#include "driver.h" 
#include "words.h"
#include "arduino_private.h"
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;

#define White 0xFF
#define Black 0x00

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7200;      
const int   daylightOffset_sec = 0; 

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 480;
const int SRC_IMG_H = 141; 
const int Y_SPACING = SRC_IMG_H - 10; 
const int GAP = 20;                   


EPaper epaper = EPaper(); 
uint8_t hh, mm;

const unsigned char* all_words[15];
size_t word_sizes[15]; 
int total_word_count = 0;

#define addWord(bmp) _addWord(bmp, sizeof(bmp))

void _addWord(const unsigned char* bitmap, size_t size) {
    if (bitmap == NULL || total_word_count >= 15) return;
    all_words[total_word_count] = bitmap;
    word_sizes[total_word_count] = size; 
    total_word_count++;
}

int calculateWidth(int index) {
    if (word_sizes[index] == 0) return 0;
    float bytes_per_row = (float)word_sizes[index] / SRC_IMG_H;
    return (int)(bytes_per_row * 8);
}

const unsigned char* getHourBitmap(uint8_t hour, size_t &outSize);
const unsigned char* getToHourBitmap(uint8_t hour, size_t &outSize);
const unsigned char* getAndOnesBitmap(uint8_t units, size_t &outSize);

void updateClockDisplay();
void runDemoMode();
void updateTimeAndDisplay();
void renderAllWords();

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    configTzTime("IST-2IDT,M3.5.5/2,M10.5.0/2", "pool.ntp.org", "time.google.com");

    struct tm timeinfo;
    while (!getLocalTime(&timeinfo) || timeinfo.tm_year < (2026 - 1900)) {
        delay(500);
        Serial.print(".");
    }

    hh = timeinfo.tm_hour;
    mm = timeinfo.tm_min;

    epaper.begin();
    epaper.setRotation(0); 
    updateClockDisplay();
}

void loop() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        static int lastMin = -1;
        
        if (timeinfo.tm_min != lastMin) {
            hh = timeinfo.tm_hour;
            mm = timeinfo.tm_min;
            lastMin = mm;
            
            Serial.printf("Real Time Update: %02d:%02d\n", hh, mm);
            updateClockDisplay();
        }
    }
    delay(1000);
}

void renderAllWords() {
    if (total_word_count == 0) return;

    int num_rows = (total_word_count + 1) / 2; 
    int total_content_height = (num_rows * SRC_IMG_H) + ((num_rows - 1) * (Y_SPACING - SRC_IMG_H));
    int start_y = (SCREEN_HEIGHT - total_content_height) / 2;
    
    int current_y = start_y;
    int i = 0;
    
    while (i < total_word_count) {
        if (i + 1 < total_word_count) {
            int w1 = calculateWidth(i);
            int w2 = calculateWidth(i + 1);
            int totalRowWidth = w1 + w2 + GAP;
            int start_x = (SCREEN_WIDTH - totalRowWidth) / 2;
            
            epaper.drawBitmap(start_x + w2 + GAP, current_y, all_words[i], w1, SRC_IMG_H, Black);
            epaper.drawBitmap(start_x, current_y, all_words[i+1], w2, SRC_IMG_H, Black);
            i += 2;
        } 
        else {
            int w = calculateWidth(i);
            int start_x = (SCREEN_WIDTH - w) / 2;
            epaper.drawBitmap(start_x, current_y, all_words[i], w, SRC_IMG_H, Black);
            i += 1;
        }
        current_y += Y_SPACING;
    }
}

void updateClockDisplay() {
epaper.fillScreen(White);
    total_word_count = 0; 
    size_t tempSize;

    uint8_t current_hh = hh % 12;
    if (current_hh == 0) current_hh = 12;
    uint8_t next_hh = (current_hh % 12) + 1;

    if (mm == 45 || mm == 50 || mm == 55) {
        if (mm == 45) addWord(epd_bitmap_quarter);
        else if (mm == 50) addWord(epd_bitmap_ten_teen);
        else if (mm == 55) addWord(epd_bitmap_five_minutes);
        
        if (next_hh == 11) { addWord(epd_bitmap_to_one); addWord(epd_bitmap_ten_teen); }
        else if (next_hh == 12) { addWord(epd_bitmap_to_two); addWord(epd_bitmap_ten_teen); }
        else { 
            const unsigned char* bmp = getToHourBitmap(next_hh, tempSize);
            _addWord(bmp, tempSize); 
        }
    } 
    else {
        if (current_hh == 11) { addWord(epd_bitmap_one); addWord(epd_bitmap_ten_teen); }
        else if (current_hh == 12) { addWord(epd_bitmap_two); addWord(epd_bitmap_ten_teen); }
        else { 
            const unsigned char* bmp = getHourBitmap(current_hh, tempSize);
            _addWord(bmp, tempSize);
        }

        if (mm != 0) {
            if (mm == 5) addWord(epd_bitmap_and_five_minutes);
            else if (mm == 10) addWord(epd_bitmap_and_ten_teen); 
            else if (mm == 15) addWord(epd_bitmap_and_quarter); 
            else if (mm == 20) addWord(epd_bitmap_and_twenty);
            else if (mm == 30) addWord(epd_bitmap_and_half); 
            else if (mm == 40) addWord(epd_bitmap_and_forty);
            else if (mm == 1) addWord(epd_bitmap_and_min);
            else if (mm == 2) { addWord(epd_bitmap_and_two_min); addWord(epd_bitmap_and_mins); }
            else if (mm >= 3 && mm <= 9) { 
                const unsigned char* bmp = getAndOnesBitmap(mm, tempSize);
                _addWord(bmp, tempSize); 
                addWord(epd_bitmap_and_mins); 
            }
            else if (mm >= 11 && mm <= 19) {
                const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                _addWord(bmp, tempSize);
                addWord(epd_bitmap_ten_teen);
                addWord(epd_bitmap_and_mins);
            }
            else if (mm >= 21 && mm <= 29) {
                addWord(epd_bitmap_twenty);
                const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                _addWord(bmp, tempSize);
            }
            else if (mm >= 31 && mm <= 39) {
                addWord(epd_bitmap_thirty);
                const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                _addWord(bmp, tempSize);
            }
            else if (mm >= 41 && mm <= 49) {
                addWord(epd_bitmap_forty);
                const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                _addWord(bmp, tempSize);
            }
            else if (mm >= 51 && mm <= 59) {
                addWord(epd_bitmap_fifty);
                const unsigned char* bmp = getAndOnesBitmap(mm % 10, tempSize);
                _addWord(bmp, tempSize);
            }
        }
    }

    if (hh >= 6 && hh < 12) addWord(epd_bitmap_in_the_morning);
    else if (hh >= 12 && hh < 15) addWord(epd_bitmap_noon);
    else if (hh >= 15 && hh < 18) addWord(epd_bitmap_afternoon);
    else if (hh >= 18 && hh < 22) addWord(epd_bitmap_in_the_evening);
    else if (hh >= 22 || hh < 6) addWord(epd_bitmap_at_night);

    renderAllWords();
    epaper.update();
}


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

void updateTimeAndDisplay() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return;
    hh = timeinfo.tm_hour; mm = timeinfo.tm_min;
    updateClockDisplay();
}