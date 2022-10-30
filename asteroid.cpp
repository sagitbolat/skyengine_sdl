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

int asteroid_x = 0;
int asteroid_y = 0;

void Update(GameState* gs, KeyboardState* ks, int dt) {
    
    //printf("%d \n", dt);
    //float delta_time = (float)dt / 1000;
    //asteroid_x += (velocity * delta_time);
    //asteroid_y += (velocity * delta_time);
    if (
        ks->state.SPACE == 1 
        && 
        ks->prev_state.SPACE == 0
    ) { 
        asteroid_x += 1;
        asteroid_y += 1;
    }
    //printf("%f \n", asteroid_x);
    
    // NOTE: What I want out of blitBitmap functionality:
    // Blit centered, default resolution
    // Blit centered, resolution is recalculated based on scale.
    // Blit centered, at a scale, default resolution
    // Blit centered, at a scale, resolution is recalculated based on scale.

    //BlitBitmap(graphics_buffer, &ship_sprite, 300, 300, 3, true, true);
    //BlitBitmap(graphics_buffer, &asteroid_sprite, (int)(asteroid_x), (int)(asteroid_y), 16, false, false);
    //BlitBitmapScaled(graphics_buffer, &ship_sprite, 300, 300, 3, 1, true);
    int scale = 16;
    int pos_scale = 1;
    bool centered = true;
    BlitBitmapScaled(graphics_buffer, &asteroid_sprite, (int)(asteroid_x), (int)(asteroid_y), scale, pos_scale, centered);
    //DrawRectangle(graphics_buffer, 255, 255, 255, 0, 0, 10, 10);
    //DrawRectangle(graphics_buffer, 255, 255, 255, 10, 10, 10, 10);
    return;
}

void UserFree() {
} 
