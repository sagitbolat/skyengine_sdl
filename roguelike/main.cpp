#define INLUDE_IMGUI
#include "../Engine/SDL_sky.cpp"
#include "../Engine/tilemap.h"

void Init(
    int* w, int* h, 
    float* width_in_world_space, 
    bool* fullscreen,
    fColor* clear_color
) {
    *w = 1280;
    *h = 720;
    *width_in_world_space = 10.f;
    *fullscreen = false;
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}

Tileset tileset = {0};

void Awake(GameMemory* gm) {

}

void Start(GameState* gs, KeyboardState* ks) {

}

void Update(GameState* gs, KeyboardState* ks, double dt) {

}

void UserFree() {

}
        