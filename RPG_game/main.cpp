#include "../Engine/SDL_sky.cpp"
#include "../Engine/bmp.cpp"
#include "../Engine/tilemaps.cpp"
#include "../Engine/collisions.cpp"
#include "../Engine/truetype.cpp"
#include "../Engine/sky_random.cpp"
#include "../Engine/UI/button.cpp"

#include "globals.h"
#include "entity.cpp"
#include "camera.cpp"




// SECTION: main
void Init(int* w, int* h, bool* fullscreen) {
    *w = SCREEN_W;
    *h = SCREEN_H;
    *fullscreen = FULLSCREEN;
}

Tileset tileset = {0};
Tilemap tilemap_level_1;

ImageData player_sprite = {0};


void Awake(GameMemory* gm) {
    LoadTileset(&gm->asset_storage, TILESET_PATH, TILE_SIZE, TILE_SIZE, &tileset);
    tilemap_level_1 = LoadTilemap(LEVEL_1_PATH);
    
    LoadBitmap(&gm->asset_storage, PLAYER_SPRITE_PATH, &player_sprite); 
}

// SECTION: Gameplay data
int current_level = 0;

void RenderBackground(Color bg) {
    RectInt rect = {0, 0, SCREEN_W, SCREEN_H};
    DrawRectangle(graphics_buffer, bg, rect); 
}

// TODO: Make this not render things outside the screen.
void RenderLevel(Tilemap level_map, Camera camera) {
    for (int y = 0; y < level_map.height; ++y) {
        for (int x = 0; x < level_map.width; ++x) {
            for (int l = 0; l < level_map.num_layers; ++l) {
                int tile_type = GetTilemapTile(&level_map, x, y, l);
                if (tile_type == 0) continue;
                else --tile_type;
                Vector2Int coords = {x, y};
                DrawSpriteInWorldCoords(camera, &tileset.tiles[tile_type], coords);
            }
        }
    }
}


Player player = {0};
Camera main_camera = {0};

void Start(GameState* gs, KeyboardState* ks) {
    player.position.x = 3;
    player.position.y = 3;
}

void Update(GameState* gs, KeyboardState* ks, int dt) {
    
    RenderBackground(BACKGROUND_COLOR);
    
    CenterOnCoords(&main_camera, player.position); 
    RenderLevel(tilemap_level_1, main_camera);
    DrawSpriteInWorldCoords(main_camera, &player_sprite, player.position); 
    
    if (ks->state.W && !ks->prev_state.W) {
        --player.position.y;
    }
    if (ks->state.A && !ks->prev_state.A) {
        --player.position.x;
    }
    if (ks->state.S && !ks->prev_state.S) {
        ++player.position.y;
    }
    if (ks->state.D && !ks->prev_state.D) {
        ++player.position.x;
    }
}


void UserFree() {

}

