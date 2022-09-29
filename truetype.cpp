#include <stdint.h>
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "skyengine.h"
#include "arena_allocator.cpp"


struct FontData {
    // TODO: Figure out how to store font asset info and stuff.
};


size_t LoadFont(GameMemory* game_memory_arena, const char* font_file_name, FontData* font_data) {
    
    // SECTION: Read the whole ttf file.
    FILE* font_file = fopen(font_file_name, "r");
    fseek(font_file, 0, SEEK_END);
    size_t file_size= ftell(font_file);
    fseek(f, 0, SEEK_SET);

    uint8_t* ttf_buffer = malloc(file_size + 1);
    fread(ttf_buffer, file_size, 1, font_file);
    fclose(font_file);
    
    // SECTION:  Stb_truetype lib.
    // TODO: Implement the ttf lib calls.


    // NOTE: Free the buffer.
    free(ttf_buffer);
    return file_size;
}


// NOTE: Displays text at the x and y location
void DisplayText(char* text, int x, int y, int size) {
    // size = 1 means that a 12px bdf font will be 12px.
    // size = 2 means that a 12px bdf font will be 24px. 
    // etc
    
}
