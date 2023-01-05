#pragma once
#include <SDL2/SDL.h>
#include "skyengine.h"
#include "skyengine.cpp" 



// SECTION: Window Size
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

// SECTION: Function declarations
static void BlitToScreen(GameBitmapBuffer*, SDL_Texture*, SDL_Renderer*);
static void ClearBuffer(GameBitmapBuffer*);


// TODO: remove from global space.
SDL_Renderer* renderer;


int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    Init(&SCREEN_WIDTH, &SCREEN_HEIGHT);

    SDL_Window* window = SDL_CreateWindow("Sky Engine",
                        SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED,
                        SCREEN_WIDTH, SCREEN_HEIGHT, 
                        SDL_WINDOW_SHOWN);

    if (window == NULL) {
        //printf("Window failed to create. %c", '\n');
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, 
                        SDL_PIXELFORMAT_ARGB8888, 
                        SDL_TEXTUREACCESS_STREAMING, 
                        SCREEN_WIDTH, SCREEN_HEIGHT); 

    // NOTE: Initializing graphics buffer
    GameBitmapBuffer graphics_buffer = {0};
    graphics_buffer.width = SCREEN_WIDTH;
    graphics_buffer.height = SCREEN_HEIGHT;
    graphics_buffer.pitch = SCREEN_WIDTH * 32;
    graphics_buffer.bytes_per_pixel = 4;
    graphics_buffer.memory = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 32);
    memset(graphics_buffer.memory, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 32);

    
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
    int timer_start = SDL_GetTicks();
    int timer_end = SDL_GetTicks();
    
    while(running) {

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
                case SDL_KEYDOWN: {
                    if (SDL_keyboard_state[SDL_SCANCODE_Q]) keyboard_state.state.Q = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_W]) keyboard_state.state.W = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_E]) keyboard_state.state.E = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_R]) keyboard_state.state.R = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_A]) keyboard_state.state.A = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_S]) keyboard_state.state.S = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_D]) keyboard_state.state.D = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_F]) keyboard_state.state.F = 1;    
                    if (SDL_keyboard_state[SDL_SCANCODE_SPACE]) keyboard_state.state.SPACE = 1;    
                } break;
                case SDL_KEYUP: {
                    if (!SDL_keyboard_state[SDL_SCANCODE_Q]) keyboard_state.state.Q = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_W]) keyboard_state.state.W = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_E]) keyboard_state.state.E = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_R]) keyboard_state.state.R = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_A]) keyboard_state.state.A = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_S]) keyboard_state.state.S = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_D]) keyboard_state.state.D = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_F]) keyboard_state.state.F = 0;    
                    if (!SDL_keyboard_state[SDL_SCANCODE_SPACE]) keyboard_state.state.SPACE = 0;    
                } break;
                default:
                    break;
            }
        }
        timer_end = SDL_GetTicks();
        int delta_time = timer_end - timer_start;
        timer_start = timer_end; 
        
        GameUpdateAndRender(&game_memory, &graphics_buffer, &keyboard_state, delta_time); 
        BlitToScreen(&graphics_buffer, texture, renderer);
        ClearBuffer(&graphics_buffer);    
    }
    
    UserFree();    
    free(graphics_buffer.memory);
    
#ifdef __linux__ 
    munmap(game_memory.permanent_storage.memory, game_memory.permanent_storage_size);
    munmap(game_memory.asset_storage.memory, game_memory.asset_storage_size);
#endif
#ifdef _WIN32
	free(game_memory.permanent_storage.memory);
    free(game_memory.asset_storage.memory);
#endif
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

static void BlitToScreen(GameBitmapBuffer* graphics_buffer, SDL_Texture* texture, SDL_Renderer* renderer) {   
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, NULL, graphics_buffer->memory, graphics_buffer->pitch);
    SDL_RenderCopy(renderer, texture, NULL, NULL); 
    SDL_RenderPresent(renderer);
}

static void ClearBuffer(GameBitmapBuffer* graphics_buffer) {
    uint64_t buffer_size = (graphics_buffer->pitch) * graphics_buffer->height;
    memset(graphics_buffer->memory, 0, buffer_size);
}
