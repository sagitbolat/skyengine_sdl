#include "SDL_sky.cpp"
#include "bmp.cpp"

void Init(int* w, int* h) {
    *w = 1280;
    *h = 720;
    return;
}

ImageData asteroid_sprite = {0};
ImageData ship_sprite = {0};

void Awake(GameMemory* gm) {
    LoadBitmap(&gm->asset_storage, "asteroid.bmp", &asteroid_sprite);
    LoadBitmap(&gm->asset_storage, "ship.bmp", &ship_sprite);
    return;
}

void Start(GameState* gs, KeyboardState* ks) {
    return;
}

void Update(GameState* gs, KeyboardState* ks, int dt) {
    BlitBitmap(graphics_buffer, &ship_sprite, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 6);
    return;
}

void UserFree() {
    return;
}
