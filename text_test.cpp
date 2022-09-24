#include "SDL_sky.cpp"
//TODO: Figure out how to get SDL_ttf to work for this example.

static void SkyInit() {
}

static void Start(GameState* game_state, KeyboardState* keyboard_state) {
    return;
}

static void Update(GameState* game_state, KeyboardState* keyboard_state) {
    Rect rect = {};
    rect.height = 100;
    rect.width = 400;
    rect.x = 440;
    rect.y = 300;
    Color color = {};
    color.red = 255;
    color.blue = 255;
    color.green = 255;
    char* message = (char*)"Hello World!";
    if (keyboard_state->state & KEY_STATE_SPACE) {
        message = (char*)"Goodbye World!";
    }
    SkyText(message, rect, color, 1000, "Calibri.ttf");
    return;
}
