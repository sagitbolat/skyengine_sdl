#pragma once
#include "skyengine.h"
// TODO: Remove this at some point and implement trig functions myself.
// #include <math.h>
// #define PI32 3.14159265f

// TODO: Remove this at some point and implement your own random functions.
#include <stdlib.h>
#include <iostream>


inline int DeltaTimeToFps(int delta_time) {
    return delta_time > 0 ? (1000/delta_time) : 1000;
}


// TODO: Make not global somehow.
static GameBitmapBuffer* graphics_buffer;

void DrawRectangle(uint8_t red, uint8_t green, uint8_t blue, int x, int y, int width, int height) {
    int min_x = x * width;
    int min_y = y * height;
    int max_x = min_x + width;
    int max_y = min_y + height;
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

// SECTION: User Defined Functions
static void Awake(GameMemory* game_memory);
static void Start(GameState* game_state, KeyboardState* keyboard_state);
static void Update(GameState* game_state, KeyboardState* keyboard_state, int delta_time);

static void GameUpdateAndRender(GameMemory* memory, GameBitmapBuffer* _graphics_buffer, KeyboardState* keyboard_state, int delta_time) {

    graphics_buffer = _graphics_buffer;
    GameState* game_state = (GameState*)memory->permanent_storage;

    if (!memory->is_initialized) {
        memory->is_initialized = true;
    }
    
    static bool first = true;

    if (first) {
        Start(game_state, keyboard_state);
        first = false;
    }
    else {
        Update(game_state, keyboard_state, delta_time);
    }
    
    //int fps = DeltaTimeToFps(delta_time);
    //std::cout << fps << std::endl;
}
