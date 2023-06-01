#pragma once
#include "skyengine.h"

// NOTE: includes imgui UI
#ifdef INCLUDE_IMGUI
#include "UI/sky_ui_imgui.cpp"
#endif



#include <iostream>

// NOTE: TODO: Change this to sample multiple frames for better precision.
inline double DeltaTimeToFps(double delta_time) {
    return delta_time > 0 ? (1000/delta_time) : 1000;
}

// SECTION: User Defined Functions
static void Init(int* width, int* height, float* width_in_world_space, bool* is_fullscreen, fColor* clear_color);
static void Awake(GameMemory* game_memory);
static void Start(GameState* game_state, KeyboardState* keyboard_state);
static void Update(GameState* game_state, KeyboardState* keyboard_state, double delta_time);
static void UserFree();

static void GameUpdateAndRender(Vector2Int screen_size, GameMemory* memory, KeyboardState* keyboard_state, double delta_time) {

    GameState* game_state = (GameState*)memory->permanent_storage.memory;

    if (!memory->is_initialized) {
        memory->is_initialized = true;
    }
    
    static bool first = true;

    #ifdef INCLUDE_IMGUI
    UI_FrameStart(screen_size);
    #endif

    if (first) {
        Start(game_state, keyboard_state);
        first = false;
    }
    else {
        Update(game_state, keyboard_state, delta_time);
    }
    
    #ifdef INCLUDE_IMGUI
    UI_FrameRender(); 
    #endif
    //double fps = DeltaTimeToFps(delta_time);
    //std::cout << fps << std::endl;
}
