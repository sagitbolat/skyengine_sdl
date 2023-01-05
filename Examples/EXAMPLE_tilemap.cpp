#include "SDL_sky.cpp"
#include "bmp.cpp"

void Init(int* w, int* h) {
    *w = 1280;
    *h = 720;
}

TilemapData tilemap;

void Awake(GameMemory* gm) {
    LoadTilemap(&gm->asset_storage, "tilemap.bmp", 16, 16, &tilemap);
}

void Start(GameState* gs, KeyboardState* ks) {
    //printf("width: %d, height: %d \n", tilemap.tiles[0].width, tilemap[0].height);
}

void Update(GameState* gs, KeyboardState* ks, int dt) {
    //BlitBitmapScaled(graphics_buffer, &tilemap[0], 0, 0, 10, 1, false);  
}

void UserFree() {

}
