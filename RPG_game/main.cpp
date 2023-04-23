#include "../Engine/SDL_sky.cpp"
#include "../Engine/bmp.cpp"
#include "../Engine/tilemaps.cpp"
#include "../Engine/collisions.cpp"
#include "../Engine/truetype.cpp"
#include "../Engine/sky_random.cpp"
#include "../Engine/UI/button.cpp"
	
int SCREEN_H = 720;
int SCREEN_W = 1280;


// SECTION: main
void Init(int* w, int* h) {
    *w = SCREEN_W;
    *h = SCREEN_H;
}

Tileset tileset = {0};
Tilemap tilemap_level_1;
uint16_t TILE_SIZE = 16;

int TILEMAP_SCALE = 5;

float player_speed = 0.01;

void Awake(GameMemory* gm) {
    LoadTileset(&gm->asset_storage, "assets/tileset.bmp", TILE_SIZE, TILE_SIZE, &tileset);
    tilemap_level_1 = LoadTilemap("levels/level1.lvl");
}

// SECTION: Gameplay data
int current_level = 0;

void Start(GameState* gs, KeyboardState* ks) {
}

void RenderBackground(Color bg) {
    RectInt rect = {0, 0, SCREEN_W, SCREEN_H};
    DrawRectangle(graphics_buffer, bg, rect); 
}

// TODO: Make this not render things outside the screen.
void RenderLevel(Tilemap level_map, float center_x, float center_y, int pixel_scale) {
    for (int y = 0; y < level_map.height; ++y) {
        for (int x = 0; x < level_map.width; ++x) {
            for (int l = 0; l < level_map.num_layers; ++l) {
                int tile_type = GetTilemapTile(&level_map, x, y, l);
                if (tile_type == 0) continue;
                else --tile_type;
                BlitBitmapScaled(
                    graphics_buffer, 
                    &tileset.tiles[tile_type], 
                    (int)((x + center_x) * (float)(pixel_scale * TILE_SIZE)), 
                    (int)((y + center_y) * (float)(pixel_scale * TILE_SIZE)), 
                    pixel_scale, 1, false
                ); 
            }
        }
    }

}

void Update(GameState* gs, KeyboardState* ks, int dt) {
    static float player_x = 0;
    static float player_y = 0;

    Color bg = {0};
    bg.red = 27;
    bg.green = 38;
    bg.blue = 50;
    bg.alpha = 255;
    RenderBackground(bg);
    RenderLevel(tilemap_level_1, player_x, player_y, TILEMAP_SCALE);
    
    if (ks->state.W) {
        player_y += dt * player_speed;
    }
    if (ks->state.A) {
        player_x += dt * player_speed;
    }
    if (ks->state.S) {
        player_y -= dt * player_speed;
    }
    if (ks->state.D) {
        player_x -= dt * player_speed;
    }
    //printf("(%f, %f)\n", player_x, player_y); 
}


void UserFree() {
  
}

