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


// SECTION: PRNG
namespace skymath {
    namespace lcg {
        static uint64_t rand_seed = (uint64_t)rand();
        uint64_t a = 1664525;
        uint64_t c = 1013904223;
        uint64_t m = (uint64_t)1 << 32;
        static void UNUSED_FUNCTION(sky_srand)(uint64_t seed) {
            rand_seed = seed;
        }
        static uint64_t UNUSED_FUNCTION(sky_rand)() {
            rand_seed = ((a * rand_seed) + c) % m;
            return rand_seed;
        }
        static float UNUSED_FUNCTION(sky_rand_float)() {
            return (float)rand() / m; 
        
        }
    }
    namespace xorshift128plus {
        static uint64_t s[2] = { (uint64_t)rand(), (uint64_t)rand() };
        
        static void UNUSED_FUNCTION(sky_srand)(uint64_t seed) {
            s[0] = seed;
            s[1] = 0;
        }
            
        static uint64_t sky_rand() {
            uint64_t a = s[0];
            uint64_t b = s[1];

            s[0] = b;
            a ^= a << 23;
            a ^= a >> 18;
            a ^= b;
            a ^= b >> 5;
            s[1] = a;

            return a + b;
        }
    }
}


// SECTION: Generation algorithm

#define TILE_LAND 1
#define TILE_WATER 0

// NOTE: returns the number of land tiles around the tile at [x, y]
static int NumNeighbors(GameState* game_state, int x, int y, int max_x, int max_y) {
    int count = 0;
    
    for (int c = x - 1; c <= x + 1; ++c) {
        for (int r = y - 1; r <= y + 1; ++r) {
            
            if ((c == x && r == y) || 
                (r < 0 || c < 0) ||
                (r >= max_y || c >= max_x)) {
                continue;
            }
            
            int i = (max_x * r) + c;

            if (game_state->tiles[i] == TILE_LAND) {
                ++count;
            }
        }
    }
    return(count);
}

static uint32_t IslandRuleset(uint32_t prev_state, int num_neighbors) {
    if (num_neighbors > 4) {
        return TILE_LAND;
    } else if (num_neighbors < 4) {
        return TILE_WATER;
    } else {
        return prev_state;
    }
}

// NOTE: Generates the intial tilemap using plate tectonic algorithm.
static void GenerateTileMap(GameState* game_state) {
    // STEP 1: populate with random noise
    for (int y = 0; y < 144; ++y) {
        for (int x = 0; x < 256; ++x) {
            
            uint64_t chance_for_water = 45;
            uint32_t tile_id = TILE_LAND;
            if (skymath::xorshift128plus::sky_rand() % 100 < chance_for_water) {
                tile_id = TILE_WATER;            
            } 
            game_state->tiles[y * 256 + x] = tile_id;

        }
    }     


    // STEP 2: collapse into a continental map with cellular automaton
    uint32_t* new_tiles = (uint32_t*)malloc(sizeof(uint32_t) * 144 * 256);
    for (int k = 0; k < 5; ++k) {
        for (int y = 0; y < 144; ++y) {
            for (int x = 0; x < 256; ++x) {
                int i = y * 256 + x;
                uint32_t prev_state = game_state->tiles[i];
                int num_neighbors = NumNeighbors(game_state, x, y, 256, 144); 
                uint32_t new_state = IslandRuleset(prev_state, num_neighbors);
                new_tiles[i] = new_state;        
            }
        } 
        for (int y = 0; y < 144; ++y) {
            for (int x = 0; x < 256; ++x) {
                int i = 256 * y + x;
                game_state->tiles[i] = new_tiles[i];        
            }
        }
    }
    free(new_tiles);
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
    
    if (keyboard_state->state & KEY_STATE_SPACE && 
            !(keyboard_state->prev_state & KEY_STATE_SPACE)) {
        first = true;
    }

    if (first) {
        //skymath::xorshift128plus::sky_srand(223137);
        GenerateTileMap(game_state);
        first = false;
    }

    for (int y = 0; y < 144; ++y) {
        for (int x = 0; x < 256; ++x) {
            uint32_t tile_id = game_state->tiles[y * 256 + x];
            uint8_t red = 0;
            uint8_t green = 0;
            uint8_t blue = 155;
            
            if (tile_id == 1) {
                red = 0;
                green = 155;
                blue = 0;
            }

            int min_x = x * square_size;
            int min_y = y * square_size;
            DrawRectangle(graphics_buffer, red, green, blue, min_x, min_y, min_x + square_size, min_y + square_size);
        }  
    }
    
    //int fps = DeltaTimeToFps(delta_time);
    //std::cout << fps << std::endl;
}
