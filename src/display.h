#pragma once

#include <GxEPD2_BW.h>

struct DisplayModule {

    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 480;
    static const int SRC_IMG_H = 165; 
    static const int Y_SPACING = SRC_IMG_H - 10; 
    static const int GAP = 20;                   

    // Initialize the 7.5 inch UC8179 E-Ink Display via GxEPD2
    GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> epaper = GxEPD2_750_GDEY075T7(/*CS=*/ D1, /*DC=*/ D3, /*RST=*/ D0, /*BUSY=*/ D2);

    const unsigned char* all_words[15];
    size_t word_sizes[15]; 
    int total_word_count = 0;

    void addWord(const unsigned char* bitmap, size_t size) {
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
                
                epaper.drawBitmap(start_x + w2 + GAP, current_y, all_words[i], w1, SRC_IMG_H, GxEPD_BLACK);
                epaper.drawBitmap(start_x, current_y, all_words[i+1], w2, SRC_IMG_H, GxEPD_BLACK);
                i += 2;
            } 
            else {
                int w = calculateWidth(i);
                int start_x = (SCREEN_WIDTH - w) / 2;
                epaper.drawBitmap(start_x, current_y, all_words[i], w, SRC_IMG_H, GxEPD_BLACK);
                i += 1;
            }
            current_y += Y_SPACING;
        }
    }

};

DisplayModule display;
