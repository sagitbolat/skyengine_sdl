#include <SDL2/SDL.h>
#include "skyengine.h"
#include "skyengine.cpp" 

// SECTION: Constants
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// SECTION: Function declarations
static void BlitToScreen(GameBitmapBuffer*, SDL_Texture*, SDL_Renderer*);

int main(int argc, char* argv[]) {
    /*int x = 640, y = 480, f = 60;
    for ( int i = 1; i < argc; i++ ) {
        if ( strcmp( argv[ i ], "-x" ) == 0 && argc > i + 1 ) {
            x = (int) strtol( argv[ ++i ], NULL, 10 );
        } else if ( strcmp( argv[ i ], "-y" ) == 0 && argc > i + 1 ) {
            y = (int) strtol( argv[ ++i ], NULL, 10 );
        } else if ( strcmp( argv[ i ], "-f" ) == 0 && argc > i + 1 ) {
            f = (int) strtol( argv[ ++i ], NULL, 10 );
        }
    }*/ 
   

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        
    SDL_Window* window = SDL_CreateWindow("Hello World",
                        SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED,
                        SCREEN_WIDTH, SCREEN_HEIGHT, 
                        SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, 
                        SDL_PIXELFORMAT_ARGB8888, 
                        SDL_TEXTUREACCESS_STREAMING, 
                        SCREEN_WIDTH, SCREEN_HEIGHT); 

    // NOTE: Initializing graphics buffer
    GameBitmapBuffer graphics_buffer = {0};
    graphics_buffer.width = SCREEN_WIDTH;
    graphics_buffer.height = SCREEN_HEIGHT;
    graphics_buffer.pitch = SCREEN_WIDTH * 32;
    graphics_buffer.memory = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 32);
    memset(graphics_buffer.memory, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 32);

    
    // NOTE: Initializing game state memory
    GameMemory game_memory = {0};
    game_memory.is_initialized = true;
    game_memory.permanent_storage_size = Megabytes(4);
    game_memory.permanent_storage = malloc(game_memory.permanent_storage_size);
    game_memory.transient_storage_size = Gigabytes(1);
    game_memory.transient_storage = malloc(game_memory.transient_storage_size);


    // NOTE: Initializing keyboard state.
    KeyboardState keyboard_state = {0};
    keyboard_state.state = 0;

    
    
    SDL_bool running = SDL_TRUE;

    SDL_Event e = {0};
    
    
    // NOTE: Timer variables.
    int timer_start = SDL_GetTicks();
    int timer_end = SDL_GetTicks();


    while(running) {
        keyboard_state.prev_state = keyboard_state.state;
        while (SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    running = SDL_FALSE;
                    break;
                case SDL_KEYDOWN: {
                    SDL_Keycode pressed = e.key.keysym.sym;
                    switch (pressed) {
                        case SDLK_w:
                            keyboard_state.state |= KEY_STATE_W;
                            break;
                        case SDLK_a:
                            keyboard_state.state |= KEY_STATE_A;
                            break;
                        case SDLK_s:
                            keyboard_state.state |= KEY_STATE_S;
                            break;
                        case SDLK_d:
                            keyboard_state.state |= KEY_STATE_D;
                            break;
                        case SDLK_i:
                            keyboard_state.state |= KEY_STATE_I;
                            break;
                        case SDLK_SPACE:
                            keyboard_state.state |= KEY_STATE_SPACE;
                            break;
                        default:
                            break;
                    }
                } break;
                case SDL_KEYUP: {
                    SDL_Keycode pressed = e.key.keysym.sym;
                    switch (pressed) {
                        case SDLK_w:
                            keyboard_state.state ^= KEY_STATE_W;
                            break;
                        case SDLK_a:
                            keyboard_state.state ^= KEY_STATE_A;
                            break;
                        case SDLK_s:
                            keyboard_state.state ^= KEY_STATE_S;
                            break;
                        case SDLK_d:
                            keyboard_state.state ^= KEY_STATE_D;
                            break;
                        case SDLK_i:
                            keyboard_state.state ^= KEY_STATE_I;
                            break;
                        case SDLK_SPACE:
                            keyboard_state.state ^= KEY_STATE_SPACE;
                            break;
                        default:
                            break;
                    }
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
         
    }
    

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

static void BlitToScreen(GameBitmapBuffer* buffer, SDL_Texture* texture, SDL_Renderer* renderer) {

    SDL_UpdateTexture(texture, NULL, buffer->memory, buffer->pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);    
    SDL_RenderPresent(renderer);
}

