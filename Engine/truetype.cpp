#pragma once
#include <stdint.h>
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "skyengine.cpp"
#include "allocation.cpp"


struct FontData {
    // TODO: Figure out how to store font asset info and stuff.
    uint16_t width[128], height[128];
    uint16_t pixel_height;
    uint8_t bytes_per_pixel;
    uint8_t** data;  // TODO: Is this the right number to fit all ascii chars?
};



// NOTE: font_data.data must be allocated as a 128 long array of pointers.
size_t LoadFont(ArenaAllocator* asset_arena, const char* font_file_name, FontData* font_data, int height_in_pixels) {
    
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
    


    font_data->bytes_per_pixel = 1;
    font_data->pixel_height = height_in_pixels; 
    


    // SECTION: load every ascii char into memory.
    // NOTE: Starts at 32 because 32 is the first relevant ascii char.
    for (int c = 33; c < 127; ++c){ 

        int width, height, x_offset, y_offset;
        uint8_t* temp_bitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, height_in_pixels), c, &width, &height, &x_offset, &y_offset);

        
        // NOTE: Handle special charachters with different alignment. (such as - + ' " )
        

        // NOTE: add vertical padding for propper alignment.
        int y_offset_to_top = height_in_pixels + y_offset;
        font_data->width[c] = width;
        font_data->height[c] = height + y_offset_to_top; 
        font_data->data[c] = (uint8_t*)(ArenaAllocateAsset(asset_arena, font_data->width[c] * font_data->height[c]));
        for (int y = 0; y < y_offset_to_top; ++y) {
            for (int x = 0; x < width; ++x){
                int i = (y * width) + x;
                font_data->data[c][i] = (uint8_t)0;
            }
        }
        


        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int i = ((y + y_offset_to_top) * width) + x;
                int j = (y * width) + x;
                font_data->data[c][i] = temp_bitmap[j];
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



// NOTE: Displays text at the x and y location
void DisplayText(const char* text, FontData* font, int x_start, int y_start, int scale, int word_spacing) {
    // scale = 1 means that a 12px loaded font will be 12px.
    // scale = 2 means that a 12px loaded font will be 24px. 
    // etc
    int length = strlen(text);
    int x_offset = 0;
    for (int k = 0; k < length; ++k) {
        uint8_t c = text[k];
        int w = font->width[c];
        int h = font->height[c];
        uint8_t** data = font->data;
        

        if (c == 32) {
            if (word_spacing == 0) word_spacing = (font->pixel_height * scale / 4); 
            w = word_spacing;
        }


        for (int y = y_start; y < (y_start + h); ++y) {
            for (int x = x_start; x < (x_start + w); ++x) {
                int i = ((y - y_start) * w) + (x - x_start);
                Color color = {0};
                color.red = data[c][i];
                color.green = data[c][i];
                color.blue = data[c][i];
                DrawRectangle(color.red, color.green, color.blue, x + x_offset, y, scale, scale);
            }
        }
        x_offset += (w + (font->pixel_height * scale / 10));
    }
}
