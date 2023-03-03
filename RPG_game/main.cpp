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


TilemapData tilemap;
const int TILE_SIZE = 16;

void Awake(GameMemory* gm) {
    LoadTilemap(&gm->asset_storage, "assets/tileset_overworld.bmp", TILE_SIZE, TILE_SIZE, &tilemap);
    
}

void Start(GameState* gs, KeyboardState* ks) {

}


void Update(GameState* gs, KeyboardState* ks, int dt) {
    static int pixel_scale = 5;  
    for (int y = 0; y < SCREEN_H / TILE_SIZE; ++y) {
        for (int x = 0; x < SCREEN_W / TILE_SIZE; ++x) {
            BlitBitmapScaled(graphics_buffer, &tilemap.tiles[9], x, y, pixel_scale, pixel_scale * TILE_SIZE, false); 
        }
    } 
}

void UserFree() {
  
}

