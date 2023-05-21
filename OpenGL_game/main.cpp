#include "../Engine/SDL_sky.cpp"



void Init(int* w, int* h, bool* fullscreen) {

}

Sprite sprite = {0};
Transform transform = {0.0f};


void Awake(GameMemory* gm) {
    sprite = LoadSprite("ship.bmp");
    
    //printf("Sprite Dimensions: %d x %d\n", sprite.pixel_width, sprite.pixel_height);

    transform.position = {0.0f, 0.0f, 0.0f};
    transform.rotation = {0.0f, 0.0f, 0.0f};
    // NOTE: Scale the sprite based on its pixel size.
    transform.scale = {(float)sprite.pixel_width/32.0f, (float)sprite.pixel_height/32.0f, 1.0f};
}



void Start(GameState* gs, KeyboardState* ks) {

}

void Update(GameState* gs, KeyboardState* ks, int dt) {
    static float player_speed = 5.0f;
    if (ks->state.W) {
        transform.position.y += player_speed * (float)dt / 1000.0f;
    }
    if (ks->state.A) {
        transform.position.x -= player_speed * (float)dt / 1000.0f;
    }
    if (ks->state.S) {
        transform.position.y -= player_speed * (float)dt / 1000.0f;
    }
    if (ks->state.D) {
        transform.position.x += player_speed * (float)dt / 1000.0f;
    }
    DrawSprite(sprite, transform, main_camera);
}


void UserFree() {

}

