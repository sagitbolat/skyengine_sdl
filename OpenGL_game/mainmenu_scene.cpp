#include "../Engine/SDL_sky.cpp"

void MainMenuAwake() {
    return;
}

void MainMenuStart(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void MainMenuUpdate(GameState* gs, KeyboardState* ks, double dt) {
   
    DrawSprite(bg_sprite, bg_transform, main_camera);

    DrawSimpleText("Asteroids", Vector2{0.5f, 0.2f}, UI_Alignment::CENTER_CENTER, title_font, font_color);

    if (DrawSimpleButton("Start Game", Vector2{0.4f, 0.35f}, Vector2{0.2f, 0.1f}, button_font, &main_button_style)){
        scene_manager.SwitchScene(SCENE_GAME, gs, ks, dt);
    } 
    if (DrawSimpleButton("Quit Game", Vector2{0.4f, 0.55f}, Vector2{0.2f, 0.1f}, button_font, &main_button_style)) {
        QuitApp(); 
    }     
    char score_string[64];
    sprintf(score_string, "Current High Score: %d", high_score);
    DrawSimpleText(score_string, {0.5f, 0.8f}, UI_Alignment::CENTER_CENTER, button_font, font_color);
    return;
}
void MainMenuClose(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void MainMenuFree() {
    return;
}



