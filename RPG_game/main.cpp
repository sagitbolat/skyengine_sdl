#include "../Engine/SDL_sky.cpp"
#include "../Engine/bmp.cpp"
#include "../Engine/tilemaps.cpp"
#include "../Engine/collisions.cpp"
#include "../Engine/truetype.cpp"
#include "../Engine/sky_random.cpp"
#include "../Engine/UI/button.cpp"

#include "entity.cpp"


// SECTION: Constants.
const int SCREEN_H = 1080;
const int SCREEN_W = 1920;
const int PIXEL_SCALE = 6;
const uint16_t TILE_SIZE = 16;



// SECTION: main
void Init(int* w, int* h, bool* fullscreen) {
    *w = SCREEN_W;
    *h = SCREEN_H;
    *fullscreen = true;
}

Tileset tileset = {0};
Tilemap tilemap_level_1;

ImageData player_sprite = {0};


void Awake(GameMemory* gm) {
    LoadTileset(&gm->asset_storage, "assets/tileset.bmp", TILE_SIZE, TILE_SIZE, &tileset);
    tilemap_level_1 = LoadTilemap("levels/1.lvl");
    
    LoadBitmap(&gm->asset_storage, "assets/player.bmp", &player_sprite); 
}

// SECTION: Gameplay data
int current_level = 0;

void DrawSpriteInWorldCoords(ImageData* sprite, Vector2Int sprite_world_coordinates, Vector2Int camera_position) {
    BlitBitmapScaled(
        graphics_buffer, 
        sprite, 
        (int)(sprite_world_coordinates.x * TILE_SIZE - camera_position.x), 
        (int)(sprite_world_coordinates.y * TILE_SIZE - camera_position.y), 
        PIXEL_SCALE, PIXEL_SCALE, false
    ); 
}

void CenterCameraOnCoords(Vector2Int* camera_position, Vector2Int center_coordinates) {
    camera_position->x = (center_coordinates.x * TILE_SIZE) - (SCREEN_W)/(2*PIXEL_SCALE) + (TILE_SIZE/2);
    camera_position->y = (center_coordinates.y * TILE_SIZE) - (SCREEN_H)/(2*PIXEL_SCALE) + (TILE_SIZE/2);
}

void RenderBackground(Color bg) {
    RectInt rect = {0, 0, SCREEN_W, SCREEN_H};
    DrawRectangle(graphics_buffer, bg, rect); 
}

// TODO: Make this not render things outside the screen.
void RenderLevel(Tilemap level_map, Vector2Int camera_position) {
    for (int y = 0; y < level_map.height; ++y) {
        for (int x = 0; x < level_map.width; ++x) {
            for (int l = 0; l < level_map.num_layers; ++l) {
                int tile_type = GetTilemapTile(&level_map, x, y, l);
                if (tile_type == 0) continue;
                else --tile_type;
                Vector2Int coords = {x, y};
                DrawSpriteInWorldCoords(&tileset.tiles[tile_type], coords, camera_position);
            }
        }
    }
}


Player player = {0};
Vector2Int camera_position = {0};


void Start(GameState* gs, KeyboardState* ks) {
    player.position.x = 3;
    player.position.y = 3;
}

void Update(GameState* gs, KeyboardState* ks, int dt) {
    Color bg = {0};
    bg.red = 27;
    bg.green = 38;
    bg.blue = 50;
    bg.alpha = 255;
    
    RenderBackground(bg);
    
    CenterCameraOnCoords(&camera_position, player.position); 
    RenderLevel(tilemap_level_1, camera_position);
    DrawSpriteInWorldCoords(&player_sprite, player.position, camera_position); 
    
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
    

    if (ks->state.SPACE && !ks->prev_state.SPACE) {
        printf("Player: (%d, %d)\n", player.position.x, player.position.y);
    }
     
}


void UserFree() {

}

