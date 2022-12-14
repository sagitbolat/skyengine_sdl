#pragma once
#include "skyengine.h"
// TODO: Remove this at some point and implement trig functions myself.
// #include <math.h>
// #define PI32 3.14159265f


#include <iostream>


inline int DeltaTimeToFps(int delta_time) {
    return delta_time > 0 ? (1000/delta_time) : 1000;
}


// TODO: Make not global by passing as parameter.
static GameBitmapBuffer* graphics_buffer;

// SECTION: User Defined Functions
static void Init(int* width, int* height);
static void Awake(GameMemory* game_memory);
static void Start(GameState* game_state, KeyboardState* keyboard_state);
static void Update(GameState* game_state, KeyboardState* keyboard_state, int delta_time);
static void UserFree();

static void GameUpdateAndRender(GameMemory* memory, GameBitmapBuffer* _graphics_buffer, KeyboardState* keyboard_state, int delta_time) {

    graphics_buffer = _graphics_buffer;
    GameState* game_state = (GameState*)memory->permanent_storage.memory;

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
