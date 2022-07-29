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



static uint32_t ConwayRuleset(uint32_t was_alive, int num_neighbors) {
    if (was_alive && 
            (num_neighbors == 2 || 
             num_neighbors == 3)) {
        return 1;
    } else if (!was_alive && 
            num_neighbors == 3) {
        return 1;
    } else {
        return 0;
    }
}

static uint32_t IslandRuleset(uint32_t waas_alive, int num_neighbors) {
    if (num_neighbors > 4) {
        return 1;
    } else {
        return 0;
    }
}


static bool first_run = true;

// SECTION: Huegene
static void ApplyColorToRandNeighbor(Color* cells, Color color, int x, int y, int max_x, int max_y) {
    for (int c = x - 1; c <= x + 1; ++c) {
        for (int r = y - 1; r <= y + 1; ++r) {
            
            if ((c == x && r == y) || 
                (r < 0 || c < 0) ||
                (r >= max_y || c >= max_x)) {
                continue;
            }
                    
            int i = (max_x * r) + c;
            int spread_color = rand() % 4;
            if (cells[i].red == 0 && 
                cells[i].green == 0 && 
                cells[i].blue == 0 && 
                spread_color == 0) 
            {
                int red_change_factor = rand() % 10;
                int green_change_factor = rand() % 10;
                int blue_change_factor = rand() % 10;
                                 
                 
                color.red -= red_change_factor; 
                color.green -= green_change_factor; 
                color.blue -= blue_change_factor; 
                 
                if (color.red <= 0) {color.red = 1;}
                if (color.green <= 0) {color.green = 1;}
                if (color.blue <= 0) {color.blue = 1;}

                cells[i] = color; 
                return;
            }

        }
    }

}

static void RenderHuegene(GameBitmapBuffer* graphics_buffer, HuegeneState* life_state) {
    // update the cell_state
    int max_x = 256;    
    int max_y = 144;    
    
    if (first_run) {
        srand(123456789);
    }
     
    Color* new_cells = new Color[max_x * max_y];
    if (first_run) {
        for (int x = 0; x < max_x; ++x) {
            for (int y = 0; y < max_y; ++y) {
                int i = (max_x * y) + x;
                new_cells[i].red = 0;
                new_cells[i].green = 0;
                new_cells[i].blue = 0; 
            }
        } 
        
        
        int center_x = max_x / 2;
        int center_y = max_y / 2;
        
        Color starting_color = {0};
        starting_color.red = 255; 

        new_cells[(max_x * center_y) + center_x] = starting_color;
    }
    else {
        for (int x = 0; x < max_x; ++x) {
            for (int y = 0; y < max_y; ++y) {
                int i = (max_x * y) + x;
                // Apply huegene rules            
                // 1. Find empty squares near it.
                // 2. change the color a little bit.
                
                Color color = life_state->cells[i]; 
                if (color.red == 0 && color.blue == 0 && color.green == 0) { 
                    continue; 
                }
                ApplyColorToRandNeighbor(new_cells, color, x, y, max_x, max_y);             
            }
        }
    }
    
    //copy cell state to the game_state.
    for (int x = 0; x < max_x; ++x) {
        for (int y = 0; y < max_y; ++y) {
            
            int i = (max_x * y) + x;

            life_state->cells[i] = new_cells[i];
        
        }
    } 

    delete new_cells;

    first_run = false;
    
    // Update the buffer

    uint8_t* row = (uint8_t*)graphics_buffer->memory;

    for (int y = 0; y < graphics_buffer->height; ++y) {

        uint32_t* pixel = (uint32_t*)row;

        for (int x = 0; x < graphics_buffer->width; ++x) {
            
            int cell_x = x / 5;
            int cell_y = y / 5;
            
            
            int i = (max_x * cell_y) + cell_x;
            
            uint8_t red = life_state->cells[i].red;
            uint8_t green = life_state->cells[i].green;
            uint8_t blue = life_state->cells[i].blue;

            

            *pixel++ = ((red << 16) | (green << 8) | blue);
        }
        row += graphics_buffer->pitch;
    }
    
}

// SECTION: Game Of Life
static int NumAliveNeighbors(GameOfLifeState* life_state, int x, int y, int max_x, int max_y) {
    int count = 0;
    
    for (int c = x - 1; c <= x + 1; ++c) {
        for (int r = y - 1; r <= y + 1; ++r) {
            
            if ((c == x && r == y) || 
                (r < 0 || c < 0) ||
                (r >= max_y || c >= max_x)) {
                continue;
            }
            
            int i = (max_x * r) + c;

            if (life_state->cells[i] != 0) {
                ++count;
            }
        }
    }
    return(count);
}
static void RenderGameOfLife(GameBitmapBuffer* graphics_buffer, GameOfLifeState* life_state) {
    // update the cell_state
    int max_x = 256;    
    int max_y = 144;    
    
    if (first_run) {
        srand(123456789);
    }
     
    uint32_t* new_cells = new uint32_t[max_x * max_y];

    for (int x = 0; x < max_x; ++x) {
        for (int y = 0; y < max_y; ++y) {
            if (first_run) {
                uint32_t alive = 0;

                if (rand() % 3 != 0) {
                    alive = 1;
                }


                int i = (max_x * y) + x;
                new_cells[i] = (uint32_t)alive;    
            } else {
                int i = (max_x * y) + x;
                uint32_t was_alive = life_state->cells[i];
                uint32_t alive = was_alive;
                int num_neighbors = NumAliveNeighbors(life_state, x, y, max_x, max_y);

                alive = IslandRuleset(was_alive, num_neighbors);


                new_cells[i] = (uint32_t)alive;    
            }
        }
    }

    //copy cell state to the game_state.
    for (int x = 0; x < max_x; ++x) {
        for (int y = 0; y < max_y; ++y) {
            
            int i = (max_x * y) + x;

            life_state->cells[i] = new_cells[i];
        
        }
    } 

    delete new_cells;

    first_run = false;
    
    // Update the buffer

    uint8_t* row = (uint8_t*)graphics_buffer->memory;

    for (int y = 0; y < graphics_buffer->height; ++y) {

        uint32_t* pixel = (uint32_t*)row;

        for (int x = 0; x < graphics_buffer->width; ++x) {

            uint8_t red = 0;
            uint8_t green = 0;
            uint8_t blue = 0;
            
            
            int cell_x = x / 5;
            int cell_y = y / 5;
            
            int i = (max_x * cell_y) + cell_x;

            if (life_state->cells[i] != 0) {
                red = 255;
                green = 255;
                blue = 255;                
            }


            *pixel++ = ((red << 16) | (green << 8) | blue);
        }
        row += graphics_buffer->pitch;
    }
}
static void GameUpdateAndRender(GameMemory* memory, GameBitmapBuffer* graphics_buffer, KeyboardState* keyboard_state, int delta_time) {
    
//    GameState* game_state = (GameState*)memory->permanent_storage;
        
//    GameOfLifeState* cell_state = (GameOfLifeState*)memory->permanent_storage;

    HuegeneState* huegene_state = (HuegeneState*)memory->permanent_storage; 

    if (!memory->is_initialized) {
        memory->is_initialized = true;
    }
    
    /* 
    if (keyboard_state->state & KEY_STATE_W) {
        game_state->y_offset += 1;
    }
    if (keyboard_state->state & KEY_STATE_A) {
        game_state->x_offset += 1;
    }
    if (keyboard_state->state & KEY_STATE_S) {
        game_state->y_offset -= 1;
    }
    if (keyboard_state->state & KEY_STATE_D) {
        game_state->x_offset -= 1;
    }
    */
     
    // TODO: Allow sample offsets here for more robust platform options.
    //RenderWeirdGradient(graphics_buffer, game_state->x_offset, game_state->y_offset);
    
    if ((keyboard_state->state & KEY_STATE_SPACE && 
            !(keyboard_state->prev_state & KEY_STATE_SPACE)) ||
            keyboard_state->state & KEY_STATE_W) {
        RenderHuegene(graphics_buffer, huegene_state);
        //RenderGameOfLife(graphics_buffer, cell_state);
    }
    if ((keyboard_state->state & KEY_STATE_S) &&
            !(keyboard_state->prev_state & KEY_STATE_S)) {
        first_run = true;
        RenderHuegene(graphics_buffer, huegene_state);
        //RenderGameOfLife(graphics_buffer, cell_state);
    }
    //int fps = DeltaTimeToFps(delta_time);
    // std::cout << fps << std::endl;
    
}
