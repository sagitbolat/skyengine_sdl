#include "SDL_sky.cpp"
#include "bmp.cpp"


void Init(int* w, int* h) {
    *w = 1280;
    *h = 720;

}

ImageData asteroid_sprite = {0};
ImageData ship_sprite = {0};

void Awake(GameMemory* gm) {
    LoadBitmap(&gm->asset_storage, "../asteroids/asteroid.bmp", &asteroid_sprite);
    LoadBitmap(&gm->asset_storage, "../asteroids/ship.bmp", &ship_sprite);
}

void Start(GameState* gs, KeyboardState* ks) {

}

void Update(GameState* gs, KeyboardState* ks, int dt) {

}

void UserFree() {
    return;
}
