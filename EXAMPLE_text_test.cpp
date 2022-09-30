#include "SDL_sky.cpp"
#include "truetype.cpp"
static void Init(int* w, int* h) {
    *w = 1280;
    *h = 720;
}

FontData font_data = {0};

static void Awake(GameMemory* game_memory) {

    LoadFont(game_memory, "pixel.ttf", &font_data);  

}

static void Start(GameState* game_state, KeyboardState* keyboard_state) {
    
    
    return;
}

static void Update(GameState* game_state, KeyboardState* keyboard_state, int delta_time) {
    DebugTextDisplay(&font_data, 1);

    return;
}
