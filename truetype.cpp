#pragma once
#include <stdint.h>
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "skyengine.cpp"
#include "arena_allocator.cpp"


struct FontData {
    // TODO: Figure out how to store font asset info and stuff.
    uint16_t width[128], height[128];
    uint8_t bytes_per_pixel;
    uint8_t** data;  // TODO: Is this the right number to fit all ascii chars?
};



// NOTE: font_data.data must be allocated as a 128 long array of pointers.
size_t LoadFont(GameMemory* game_memory_arena, const char* font_file_name, FontData* font_data) {
    
    // SECTION: Read the whole ttf file.
    FILE* font_file = fopen(font_file_name, "rb");
    if (font_file == NULL) {
        //printf("LoadFont Error. Failed to open file %s \n", font_file_name);
    }
    fseek(font_file, 0, SEEK_END);
    size_t file_size= ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    uint8_t* ttf_buffer = (uint8_t*)malloc(file_size + 1);
    fread(ttf_buffer, file_size, 1, font_file);
    fclose(font_file);
    


    // SECTION:  Stb_truetype lib.
    // TODO: Implement the ttf lib calls.
    stbtt_fontinfo font;
    
    stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));

    

    // SECTION: Allocate the font on the arena.
    size_t font_memory_size = 0; 
    


    // SECTION: load every ascii char into memory.
    // NOTE: Starts at 32 because 32 is the first relevant ascii char.
    for (int c = 32; c < 127; ++c){ 
        int width, height, x_offset, y_offset;
        uint8_t* temp_bitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, 64), c, &width, &height, &x_offset, &y_offset);

        
        font_data->width[c] = width;
        font_data->height[c] = height;
        font_data->bytes_per_pixel = 1;
        
        font_data->data[c] = (uint8_t*)(Arena::AllocateAsset(game_memory_arena, width*height));
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                int i = (y * width) + x;
                font_data->data[c][i] = temp_bitmap[i];
            }
        }
        font_memory_size += width*height;
        
        // NOTE: Free the character bitmap.
        stbtt_FreeBitmap(temp_bitmap, 0);
    }


    // SECTION: freeing resources.
    // NOTE: Free the file buffer.
    free(ttf_buffer);



    // SECTION: Return
    return font_memory_size;
}

// TODO: Delete this. this is only for testing.
void DebugTextDisplay(uint8_t c, FontData* font, int scale) {
    int w = font->width[c];
    int h = font->height[c];
    uint8_t** data = font->data;
    
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = (y * w) + x;
            Color color = {0};
            color.red = data[c][i];
            color.green = data[c][i];
            color.blue = data[c][i];
            DrawRectangle(color.red, color.green, color.blue, x, y, scale, scale);
        }
    }
}


// NOTE: Displays text at the x and y location
void DisplayText(char* text, int x, int y, int size) {
    // size = 1 means that a 12px bdf font will be 12px.
    // size = 2 means that a 12px bdf font will be 24px. 
    // etc
    
}
