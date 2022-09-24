#include "SDL_sky.cpp"
#include "sky_random.cpp"
#include "bmp.cpp"
#include <iostream>


bool first_run = true;


int width = 1280;
int height = 720;
int square_size = 1; 

struct HuegeneState {
    Color cells[1280 * 720];
};



static void RenderHuegene(GameBitmapBuffer* graphics_buffer, HuegeneState* hue_state);




static void SkyInit() {
    return;
}

static void Start(GameState* game_state, KeyboardState* keyboard_state) {
    HuegeneState* huegene_state = (HuegeneState*)game_state;
    RenderHuegene(graphics_buffer, huegene_state);
    first_run = false;
    sky_srand(21021039);
    return;
}

static void Update(GameState* game_state, KeyboardState* keyboard_state) {
    HuegeneState* huegene_state = (HuegeneState*)game_state;
    if ((keyboard_state->state & KEY_STATE_SPACE) && !(keyboard_state->prev_state & KEY_STATE_SPACE)) {
        GenerateBitmapImage(huegene_state->cells, 1280, 720, (char*)"Huegene_output.bmp"); 
        printf("attempting bmp generation");
    }
    RenderHuegene(graphics_buffer, huegene_state); 
    return;
}

// SECTION: Huegene
static Color GetColorOfRandNeighbor(Color* cells, int x, int y, int max_x, int max_y) {
    Color color = {0};
    for (int c = x - 1; c <= x + 1; ++c) {
        for (int r = y - 1; r <= y + 1; ++r) {
            
            if ((c == x && r == y) || 
                (r < 0 || c < 0) ||
                (r >= max_y || c >= max_x)) {
                continue;
            }
            int i = (max_x * r) + c;
            if (cells[i].red != 0 || cells[i].green != 0 || cells[i].blue != 0) {
                color = cells[i];
                return color;
            }
    
        }
    }
    return color;
}


// TODO: Swap from using default prng to xorshift or perlin noise.
static void RenderHuegene(GameBitmapBuffer* graphics_buffer, HuegeneState* hue_state) {
    int max_x = width;    
    int max_y = height;    
    int jaggedness = 5;
    int fade_speed = 2;
    int flip_chance = 60; //in percentages
    
    if (jaggedness < 1) {
        jaggedness = 1;
    }
    if (fade_speed < 1) {
        fade_speed = 1;
    }

    if (first_run) {
        //srand(123456789);
    }
     
    Color* new_cells = new Color[max_x * max_y];
    // update the cell_state
    for (int x = 0; x < max_x; ++x) {
        for (int y = 0; y < max_y; ++y) {
            if (first_run) {
                Color color = {0};
                // if (y == 160 && x % 15 == 0) {
                //     color.red = 214;
                //     color.green = 2;
                //     color.blue = 112;
                // }
                // if (y == max_y / 2 && x % 15 == 0) {
                //     color.red = 155;
                //     color.green = 79;
                //     color.blue = 150;
                // }
                // if (y == max_y-161 && x % 15 == 0) {
                //     color.red = 0;
                //     color.green = 106;
                //     color.blue = 220;
                // }

                // if (y == max_y / 2 && x == max_x/2) {
                //     color.red = 155;
                //     color.green = 79;
                //     color.blue = 150;
                // }

                // if (y == 240 && x == 320) {
                //     color.red = 215;
                //     color.green = 169;
                //     color.blue = 227;
                // }
                // if (y == 240 && x == 960) {
                //     color.red = 139;
                //     color.green = 190;
                //     color.blue = 232;
                // }
                // if (y == 480 && x == 640) {
                //     color.red = 168;
                //     color.green = 213;
                //     color.blue = 186;
                // }

                if (y == 0) {
                    color.red = 0;
                    color.green = 170;
                    color.blue = 17;
                }
                int i = (max_x * y) + x;
                new_cells[i] = color;    
            } else {
                int i = (max_x * y) + x;
                Color color = hue_state->cells[i];
                int  to_mutate = sky_rand() % jaggedness;
                if (color.red == 0 && color.green == 0 && color.blue == 0 && to_mutate == 0) {
                    color = GetColorOfRandNeighbor(hue_state->cells, x, y, max_x, max_y);
                    uint8_t red_dec = sky_rand() % fade_speed;
                    uint8_t green_dec = sky_rand() % fade_speed;
                    uint8_t blue_dec = sky_rand() % fade_speed;
                    
                    // SECTION: MORE RANDOMIZATION. CAN DELETE.
                    uint8_t red_flip = sky_rand() % 100;
                    uint8_t green_flip = sky_rand() % 100;
                    uint8_t blue_flip = sky_rand() % 100;
                    if (red_flip < flip_chance) {
                       red_dec *= -1; 
                    }
                    if (green_flip < flip_chance) {
                       green_dec *= -1; 
                    }
                    if (blue_flip < flip_chance) {
                       blue_dec *= -1; 
                    }
                    // END OF SECTION

                    if (color.red > red_dec) {
                        color.red -= red_dec;
                    }
                    if (color.green > green_dec) {
                        color.green -= green_dec;
                    }
                    if (color.blue > blue_dec) {
                        color.blue -= blue_dec;
                    }
                }
                new_cells[i] = color;    
            }
        }
    }

    
    //copy cell state to the game_state.
    for (int x = 0; x < max_x; ++x) {
        for (int y = 0; y < max_y; ++y) {
            
            int i = (max_x * y) + x;
            hue_state->cells[i] = new_cells[i];
        
        }
    } 
    delete new_cells;
    first_run = false;

    // Draw to buffer
    for (int y = 0; y < max_y; ++y) {
        for (int x = 0; x < max_x; ++x) {   
            int i = (y * max_x) + x;  
            uint8_t red = hue_state->cells[i].red;
            uint8_t green = hue_state->cells[i].green;       
            uint8_t blue = hue_state->cells[i].blue;       
            
            
            int min_x = x * square_size;
            int min_y = y * square_size;
            DrawRectangle(red, green, blue, min_x, min_y, min_x + square_size, min_y + square_size);
        }
    }
}
