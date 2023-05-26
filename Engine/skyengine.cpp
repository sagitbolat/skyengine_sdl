#pragma once
#include "skyengine.h"
// TODO: Remove this at some point and implement trig functions myself.
// #include <math.h>
// #define PI32 3.14159265f


#include <iostream>

// NOTE: TODO: Change this to sample multiple frames for better precision.
inline int DeltaTimeToFps(int delta_time) {
    return delta_time > 0 ? (1000/delta_time) : 1000;
}

// SECTION: User Defined Functions
static void Init(int* width, int* height, bool* is_fullscreen, fColor* clear_color);
static void Awake(GameMemory* game_memory);
static void Start(GameState* game_state, KeyboardState* keyboard_state);
static void Update(GameState* game_state, KeyboardState* keyboard_state, double delta_time);
static void UserFree();

static void GameUpdateAndRender(GameMemory* memory, KeyboardState* keyboard_state, double delta_time) {

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
