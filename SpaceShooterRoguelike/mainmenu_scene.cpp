#include "../Engine/SDL_sky.cpp"

void MainMenuAwake() {
    return;
}

void MainMenuStart(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void MainMenuUpdate(GameState* gs, KeyboardState* ks, double dt) {

    DrawSimpleText("Space Survivors", Vector2{0.5f, 0.3f}, UI_Alignment::CENTER_CENTER, title_font, font_color);

    if (DrawSimpleButton("Continue Run", Vector2{0.4f, 0.4f}, Vector2{0.2f, 0.09f}, button_font, &main_button_style)){
        printf("NOT IMPLEMNTED\n");
    } 
    if (DrawSimpleButton("New Run", Vector2{0.4f, 0.5f}, Vector2{0.2f, 0.09f}, button_font, &main_button_style)){
        scene_manager.SwitchScene(SCENE_SELECTION, gs, ks, dt);
    } 
    if (DrawSimpleButton("Quit Game", Vector2{0.4f, 0.6f}, Vector2{0.2f, 0.09f}, button_font, &main_button_style)) {
        QuitApp(); 
    }     
    return;
}
void MainMenuClose(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void MainMenuFree() {
    return;
}



