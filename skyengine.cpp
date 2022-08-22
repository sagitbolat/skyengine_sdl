#include "skyengine.h"
// TODO: Remove this at some point and implement trig functions myself.
#include <math.h>
#define PI32 3.14159265f

// TODO: Remove this at some point and implement your own random functions.
#include <stdlib.h>
#include <iostream>


inline int DeltaTimeToFps(int delta_time) {
    return delta_time > 0 ? (1000/delta_time) : 1000;
}

static uint64_t sky_rand_seed = 0;
uint64_t a = 1664525;
uint64_t c = 1013904223;
uint64_t m = 4294967296;
static void sky_srand(uint64_t seed) {
    sky_rand_seed = seed;
}

static uint64_t sky_rand() {
    
    
    sky_rand_seed = ((a * sky_rand_seed) + c) % m;
    return sky_rand_seed;
}

static float sky_rand_float() {
    return (float)sky_rand() / m; 
}


static void GenerateTileMap(GameState* game_state) {
    // NOTE: populate with random noise
    for (int y = 0; y < 144; ++y) {
        for (int x = 0; x < 256; ++x) {
            
            uint32_t tile_id = 0;
            if (sky_rand_float() < 0.5f) {
                tile_id = 1;            
            } 
            game_state->tiles[y * 256 + x] = tile_id;

        }
    }     
}

static void DrawRectangle(GameBitmapBuffer* graphics_buffer, uint8_t red, uint8_t green, uint8_t blue, int min_x, int min_y, int max_x, int max_y) {
    
    if (min_x < 0) {
        min_x = 0;
    }
    if (min_y < 0) {
        min_y = 0;
    }
    if (max_x > graphics_buffer->width) {
        max_x = graphics_buffer->width;
    }
    if (max_y > graphics_buffer->height) {
        max_y = graphics_buffer->height;
    }

    uint8_t* row = (uint8_t*)graphics_buffer->memory + 
                     (min_x * graphics_buffer->bytes_per_pixel) + 
                     (min_y * graphics_buffer->pitch);

    for (int y = min_y; y < max_y; ++y) {
       
        uint32_t* pixel = (uint32_t*)row;
        
        for (int x = min_x; x < max_x; ++x) {
            *pixel++ = ((red << 16) | (green << 8) | blue);
        }
        row += graphics_buffer->pitch;
    }
}

static void GameUpdateAndRender(GameMemory* memory, GameBitmapBuffer* graphics_buffer, KeyboardState* keyboard_state, int delta_time) {
    
    GameState* game_state = (GameState*)memory->permanent_storage;

    if (!memory->is_initialized) {
        memory->is_initialized = true;
    }
    int square_size = 5; // NOTE: Size of square tile in pixels 
    
    static bool first = true;
    if (first) {
        sky_srand(223137);
        GenerateTileMap(game_state);
        first = false;
    }

    for (int y = 0; y < 144; ++y) {
        for (int x = 0; x < 256; ++x) {
            uint32_t tile_id = game_state->tiles[y * 256 + x];
            uint8_t red = 155;
            uint8_t green = 155;
            uint8_t blue = 155;
            
            if (tile_id == 1) {
                red = 255;
                green = 255;
                blue = 255;
            }

            int min_x = x * square_size;
            int min_y = y * square_size;
            DrawRectangle(graphics_buffer, red, green, blue, min_x, min_y, min_x + square_size, min_y + square_size);
        }  
    }


    //int fps = DeltaTimeToFps(delta_time);
    // std::cout << fps << std::endl;
    
}
