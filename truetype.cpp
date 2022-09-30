#pragma once
#include <stdint.h>
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "skyengine.cpp"
#include "arena_allocator.cpp"


struct FontData {
    // TODO: Figure out how to store font asset info and stuff.
    uint16_t width, height;
    uint8_t bytes_per_pixel;
    uint8_t* data;
};


size_t LoadFont(GameMemory* game_memory_arena, const char* font_file_name, FontData* font_data) {
    
    // SECTION: Read the whole ttf file.
    FILE* font_file = fopen(font_file_name, "rb");
    if (font_file == NULL) {
        printf("LoadFont Error. Failed to open file %s", font_file_name);
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
    int width, height, x_offset, y_offset;
    uint8_t* temp_bitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, 128), 'A', &width, &height, &x_offset, &y_offset);
    
    // NOTE: Free the buffer.
    free(ttf_buffer);
    


    // SECTION: Allocate the font on the arena.
    size_t font_memory_size = 0;
    
    font_data->width = width;
    font_data->height = height;
    font_data->bytes_per_pixel = 1;
    

    font_data->data = (uint8_t*)(Arena::AllocateAsset(game_memory_arena, width*height));
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            int i = (y * width) + x;
            font_data->data[i] = temp_bitmap[i];
        }
    }
    font_memory_size += width*height;
   


    // NOTE: Free the bitmap.
    stbtt_FreeBitmap(temp_bitmap, 0);



    // SECTION: Return
    return font_memory_size;
}

// TODO: Delete this. this is only for testing.
void DebugTextDisplay(FontData* font, int scale) {
    int w = font->width;
    int h = font->height;
    uint8_t* data = font->data;
    
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = (y * w) + x;
            Color c = {0};
            c.red = data[i];
            c.green = data[i];
            c.blue = data[i];
            DrawRectangle(c.red, c.green, c.blue, x, y, scale, scale);
        }
    }
}


// NOTE: Displays text at the x and y location
void DisplayText(char* text, int x, int y, int size) {
    // size = 1 means that a 12px bdf font will be 12px.
    // size = 2 means that a 12px bdf font will be 24px. 
    // etc
    
}
