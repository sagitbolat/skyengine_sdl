#pragma once
#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <GL/gl.h>


#include "skyengine.h"
#include "skyengine.cpp" 

#include "OpenGL_renderer2D.cpp"


// SECTION: Window Size
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;
float SCREEN_WIDTH_IN_WORLD_SPACE = 10.0f;
bool LAUNCH_FULLSCREEN = 0;
fColor CLEAR_COLOR = {0.05, 0.0, 0.15, 1.0};

// NOTE: Returns the x and y of the mouse cursor, in pixels. 0,0 is top left of the window
Vector2Int GetMousePosition() {
    Vector2Int pos = {0};
    SDL_GetMouseState(&pos.x, &pos.y);
    return pos;
}

// NOTE: Pass true to enable the mouse cursor and false to disable the mouse cursor.
void SetCursorVisibility(bool toggle) {
    if (toggle) SDL_ShowCursor(SDL_ENABLE);
    else SDL_ShowCursor(SDL_DISABLE);
}


Camera main_camera = {0};

int main(int argc, char* argv[]) {


    // NOTE: The skyengine function that the user implements
    Init(&SCREEN_WIDTH, &SCREEN_HEIGHT, &SCREEN_WIDTH_IN_WORLD_SPACE, &LAUNCH_FULLSCREEN, &CLEAR_COLOR);
    

    // TODO: pass LAUNCH_FULLSCREEN as an argument to the initializer
    WindowState window = InitWindowContext(SCREEN_WIDTH, SCREEN_HEIGHT, "SkyEngine App", CLEAR_COLOR);

    main_camera.position = {0.0f, 0.0f, 3.0f};
    main_camera.up_direction = {0.0f, 0.1f, 0.0f};
    main_camera.look_target = {0.0f, 0.0f, 0.0f};
    main_camera.near_plane = 0.1f;
    main_camera.far_plane = 1000.0f;
    main_camera.width = SCREEN_WIDTH_IN_WORLD_SPACE;
    main_camera.height = (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH * main_camera.width;
    main_camera.FOV = 90.0f;
    main_camera.projection = ORTHOGRAPHIC_CAMERA;



    // NOTE: Initialize the freelist allocator
    size_t alignment = 64; // TODO: Change to reflect the machine architecture.
#ifdef ALLOCATOR_SIZE
    skymem::init((size_t)ALLOCATOR_SIZE, alignment);
#else
    skymem::init((size_t)Megabytes(128), alignment);
#endif
    
    // NOTE: Initializing game state memory
    GameMemory game_memory = {0};
    game_memory.is_initialized = true;
    game_memory.permanent_storage_size = Megabytes(128);
    game_memory.asset_storage_size = Gigabytes(1);
    game_memory.asset_storage = {0};
    
    InitArena(&game_memory.permanent_storage, game_memory.permanent_storage_size);
    InitArena(&game_memory.asset_storage, game_memory.asset_storage_size);

    // NOTE: Initializing keyboard state.
    KeyboardState keyboard_state = {0};
    keyboard_state.state = {0};

    
    
    SDL_bool running = SDL_TRUE;

    SDL_Event e = {0};
    
    
    //Awake routine.
    Awake(&game_memory);


    // NOTE: Timer variables.
    uint64_t frequency = SDL_GetPerformanceFrequency();
    uint64_t previousCounter = SDL_GetPerformanceCounter();
    uint64_t currentCounter;
    uint64_t deltaCounter;

    while(running) {
        
        // SECTION: Time keeping
        // Calculate delta counter
        currentCounter = SDL_GetPerformanceCounter();
        deltaCounter = currentCounter - previousCounter;
        previousCounter = currentCounter;

        // Calculate delta time in milliseconds
        double delta_time = (double)deltaCounter / (double)frequency * 1000.0;



        // NOTE: Copying current state into previous state.
        // TODO: Should I use memcpy or assignment here?
        keyboard_state.prev_state = keyboard_state.state;
        //memcpy(&keyboard_state.prev_state, &keyboard_state.state, sizeof(KeyState));
        
        
        // NOTE: Use SDL2's keyboard state struct:
        const uint8_t* SDL_keyboard_state = SDL_GetKeyboardState(NULL);

        while (SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    running = SDL_FALSE;
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (e.button.button == SDL_BUTTON_LEFT) keyboard_state.state.MBL = 1;
                    if (e.button.button == SDL_BUTTON_RIGHT) keyboard_state.state.MBR = 1;
                    if (e.button.button == SDL_BUTTON_MIDDLE) keyboard_state.state.MBM = 1;
                } break;
                case SDL_MOUSEBUTTONUP: {
                    if (e.button.button == SDL_BUTTON_LEFT) keyboard_state.state.MBL = 0;
                    if (e.button.button == SDL_BUTTON_RIGHT) keyboard_state.state.MBR = 0;
                    if (e.button.button == SDL_BUTTON_MIDDLE) keyboard_state.state.MBM = 0;
                } break;
                case SDL_KEYDOWN: {
                    if (SDL_keyboard_state[SDL_SCANCODE_Q]) keyboard_state.state.Q = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_W]) keyboard_state.state.W = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_E]) keyboard_state.state.E = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_R]) keyboard_state.state.R = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_I]) keyboard_state.state.I = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_A]) keyboard_state.state.A = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_S]) keyboard_state.state.S = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_D]) keyboard_state.state.D = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_F]) keyboard_state.state.F = 1;  
                    if (SDL_keyboard_state[SDL_SCANCODE_G]) keyboard_state.state.G = 1; 		  
                    if (SDL_keyboard_state[SDL_SCANCODE_J]) keyboard_state.state.J = 1; 		  
                    if (SDL_keyboard_state[SDL_SCANCODE_K]) keyboard_state.state.K = 1; 		  
                    if (SDL_keyboard_state[SDL_SCANCODE_L]) keyboard_state.state.L = 1; 		  
                    if (SDL_keyboard_state[SDL_SCANCODE_Z]) keyboard_state.state.Z = 1;  
                    if (SDL_keyboard_state[SDL_SCANCODE_X]) keyboard_state.state.X = 1;  
                    if (SDL_keyboard_state[SDL_SCANCODE_C]) keyboard_state.state.C = 1;  
                    if (SDL_keyboard_state[SDL_SCANCODE_V]) keyboard_state.state.V = 1;  
                    if (SDL_keyboard_state[SDL_SCANCODE_SPACE]) keyboard_state.state.SPACE = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_RETURN]) keyboard_state.state.ENTER = 1;    
                } break;
                case SDL_KEYUP: {
                    if (!SDL_keyboard_state[SDL_SCANCODE_Q]) keyboard_state.state.Q = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_W]) keyboard_state.state.W = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_E]) keyboard_state.state.E = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_R]) keyboard_state.state.R = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_I]) keyboard_state.state.I = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_A]) keyboard_state.state.A = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_S]) keyboard_state.state.S = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_D]) keyboard_state.state.D = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_F]) keyboard_state.state.F = 0;   
                    if (!SDL_keyboard_state[SDL_SCANCODE_G]) keyboard_state.state.G = 0;  
                    if (!SDL_keyboard_state[SDL_SCANCODE_J]) keyboard_state.state.J = 0;  
                    if (!SDL_keyboard_state[SDL_SCANCODE_K]) keyboard_state.state.K = 0;  
                    if (!SDL_keyboard_state[SDL_SCANCODE_L]) keyboard_state.state.L = 0;  
                    if (!SDL_keyboard_state[SDL_SCANCODE_Z]) keyboard_state.state.Z = 0;   
                    if (!SDL_keyboard_state[SDL_SCANCODE_X]) keyboard_state.state.X = 0;   
                    if (!SDL_keyboard_state[SDL_SCANCODE_C]) keyboard_state.state.C = 0;   
                    if (!SDL_keyboard_state[SDL_SCANCODE_V]) keyboard_state.state.V = 0;   
                    if (!SDL_keyboard_state[SDL_SCANCODE_SPACE]) keyboard_state.state.SPACE = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_RETURN]) keyboard_state.state.ENTER = 0;    
                } break;
                default:
                    break;
            }
        }
        
        

        // Clear the screen
        ClearScreen();
        
        GameUpdateAndRender(&game_memory, &keyboard_state, delta_time); 
        
        // Swap buffers
        SDL_GL_SwapWindow(window.window);

    }
    
    UserFree();
    
#ifdef __linux__ 
    munmap(game_memory.permanent_storage.memory, game_memory.permanent_storage_size);
    munmap(game_memory.asset_storage.memory, game_memory.asset_storage_size);
#endif
#ifdef _WIN32
	free(game_memory.permanent_storage.memory);
    free(game_memory.asset_storage.memory);
#endif
    FreeWindowContext(window);
    SDL_Quit();
    return 0;
}
