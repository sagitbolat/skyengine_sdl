#include "SDL_sky.cpp"
#include "sky_random.cpp"


int square_size = 1;
bool first_run = true;

struct HuegeneState {
    uint32_t tiles[1920 * 1080];
};
struct Color {
    uint8_t a, red, green, blue;
};

static void RenderHuegene(GameBitmapBuffer* graphics_buffer, HuegeneState* hue_state);




static void SkyInit() {
    return;
}

static void Start(GameState* game_state) {
    HuegeneState* huegene_state = (HuegeneState*)game_state;
    RenderHuegene(graphics_buffer, huegene_state);
    first_run = false;
    return;
}

static void Update(GameState* game_state) {
    HuegeneState* huegene_state = (HuegeneState*)game_state;
    RenderHuegene(graphics_buffer, huegene_state);
    return;
}

static void RenderHuegene(GameBitmapBuffer* graphics_buffer, HuegeneState* hue_state) {
    int max_x = 256;    
    int max_y = 144;    
    int jaggedness = 50;
    int fade_speed = 3;
    
    if (first_run) {
        //srand(123456789);
    }
     
    Color* new_cells = new Color[max_x * max_y];
    // update the cell_state
    for (int x = 0; x < max_x; ++x) {
        for (int y = 0; y < max_y; ++y) {
            if (first_run) {
                Color color = {0};
                if (y == 0 && x == max_x/2) {
                    color.red = 214;
                    color.green = 2;
                    color.blue = 112;
                }
                if (y == max_y / 2 && x == max_x/2) {
                    color.red = 155;
                    color.green = 79;
                    color.blue = 150;
                }
                if (y == max_y-1 && x == max_x/2) {
                    color.red = 0;
                    color.green = 56;
                    color.blue = 168;
                }
                int i = (max_x * y) + x;
                new_cells[i] = color;    
            } else {
                int i = (max_x * y) + x;
                Color color = hue_state->cells[i];
                int  to_mutate = rand() % jaggedness;
                if (color.red == 0 && color.green == 0 && color.blue == 0 && to_mutate == 0) {
                    color = GetColorOfRandNeighbor(hue_state->cells, x, y, max_x, max_y);
                    uint8_t red_dec = rand() % fade_speed;
                    uint8_t green_dec = rand() % fade_speed;
                    uint8_t blue_dec = rand() % fade_speed;
                    
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
