#include "Engine/SDL_sky.cpp"
#include "Engine/bmp.cpp"
#include "array_utils.cpp"



void Init(int* w, int* h) {
    *w = 1280;
    *h = 720;
}

TilemapData grass_tilemap;

void Awake(GameMemory* gm) {
    LoadTilemap(&gm->asset_storage, "tilemap_square.bmp", 16, 16, &grass_tilemap);

}


// SECTION: Tilemap Level Loading.
// NOTE: A tilemap level should have the following format:
/* HEADER: 8 bytes to represent the width of the level, 8 bytes to represent the height of the level, newline character
 * n 8-byte characters each representing a tile, where n is the width of the level. This is followed by a newline char
 * The above line repeats m times, where m is the height of the level. 
 * Ex:
 * 44 // THIS IS THE HEADER
 * 1111 
 * 1001
 * 1001
 * 1111
 */

struct LevelTiledata {
    int width;
    int height;
    uint8_t* tile_data;
};

LevelTiledata LoadLevel(const char* level_name) {
    FILE *file = fopen(level_name, "r");
    uint8_t width = fgetc(file);
    uint8_t height = fgetc(file);
    
    fseek(file, 1, SEEK_CUR);

    LevelTiledata level_data = {0};
    level_data.tile_data = (uint8_t*)malloc(sizeof(uint8_t) * width * height);
    level_data.width = width;
    level_data.height = height;

    for (int h = 0; h < height; ++h) {
        fread(&(level_data.tile_data[h * width]), 1, width, file);
        //for (int w = 0; w < width; ++w) {
        //    level_data.tile_data[h*width+w] -= 0;
        //}
        fseek(file, 1, SEEK_CUR);
    }
    fclose(file);
    return level_data;
}

LevelTiledata level1_data; 

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
    int scale = 2;
    for (int h = 0; h < level1_data.height; ++h) {
        for (int w = 0; w < level1_data.width; ++w) {
            BlitBitmapScaled(graphics_buffer, &grass_tilemap.tiles[level1_data.tile_data[h*level1_data.width+w]], w, h, scale, scale * 16, false); 
        }
    }

}

void UserFree() {

}
