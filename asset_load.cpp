#include "SDL_sky.cpp"
#include "bmp.cpp"

static void Init(int* w, int *h) {
    *w = 1920;
    *h = 1080;
    return;
}

static void Awake(GameMemory* game_memory) {
    LoadBitmap(game_memory, "test_tilemap.bmp");
}


static void Start(GameState* game_state, KeyboardState* keyboard_state) {
    return;
}

static void Update(GameState* game_state, KeyboardState* keyboard_state) {
    
    
    return;
}
