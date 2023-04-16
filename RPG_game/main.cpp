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


Tileset tileset;
Tilemap tilemap;
int TILE_SIZE = 8;


void Awake(GameMemory* gm) {
    LoadTileset(&gm->asset_storage, "assets/tileset.bmp", TILE_SIZE, TILE_SIZE, &tileset);
    tilemap = LoadTilemap("levels/level.lvl");
    TILE_SIZE = tileset.tile_width;
}


void Start(GameState* gs, KeyboardState* ks) {

}


void Update(GameState* gs, KeyboardState* ks, int dt) {
    static int pixel_scale = 5;  
    for (int y = 0; y < tilemap.height; ++y) {
        for (int x = 0; x < tilemap.width; ++x) {
            int i = (y * tilemap.width) + x;   
            int tile_type = tilemap.tile_data[i];
            if (tile_type == 0) continue;
            else --tile_type;
            BlitBitmapScaled(graphics_buffer, &tileset.tiles[tile_type], x, y, pixel_scale, pixel_scale * TILE_SIZE, false); 
        }
    } 
}


void UserFree() {
  
}

