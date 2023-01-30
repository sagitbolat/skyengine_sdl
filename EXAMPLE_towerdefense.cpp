#include "Engine/SDL_sky.cpp"
#include "Engine/bmp.cpp"
#include "Engine/tilemaps.cpp"
#include "array_utils.cpp"


void Init(int* w, int* h) {
    *w = 1280;
    *h = 720;
}

TilemapData grass_tilemap;
LevelTiledata level1_data; 

void Awake(GameMemory* gm) {
    LoadTilemap(&gm->asset_storage, "tilemap_square.bmp", 16, 16, &grass_tilemap);
}


void Start(GameState* gs, KeyboardState* ks) {
    level1_data = LoadLevel("level1.bin");
    /*printf("\n");
    for (int h = 0; h < level1_data.height; ++h) {
        for (int w = 0; w < level1_data.width; ++w) {
            printf("%d", level1_data.tile_data[h*level1_data.width+w]);
        }
        printf("\n");
    }*/
}

int total_time = 0;

void Update(GameState* gs, KeyboardState* ks, int dt) {
    total_time += dt;
    int scale = 3;
    for (int h = 0; h < level1_data.height; ++h) {
        for (int w = 0; w < level1_data.width; ++w) {
            BlitBitmapScaled(graphics_buffer, &grass_tilemap.tiles[level1_data.tile_data[h*level1_data.width+w]], w, h, scale, scale * 16, false); 
        }
    }
}

void UserFree() {
        
}
