#include "../Engine/SDL_sky.cpp"


void GameoverAwake() {
    return;
}

bool new_high_score = false;

void GameoverStart(GameState* gs, KeyboardState* ks, double dt) {
    if (last_score > high_score) {
        high_score = last_score;
        new_high_score = true;
    } else {
        new_high_score = false;
    }
    return;
}

void GameoverUpdate(GameState* gs, KeyboardState* ks, double dt) {

    DrawSimpleText("Game Over", Vector2{0.5f, 0.2f}, UI_Alignment::CENTER_CENTER, title_font);
    
    if (DrawSimpleButton("Main Menu", Vector2{0.4f, 0.3f}, Vector2{0.2f, 0.1f}, button_font, &main_button_style)) {
        scene_manager.SwitchScene(SCENE_MAIN_MENU, gs, ks, dt);
    } 
    if (DrawSimpleButton("Restart", Vector2{0.4f, 0.45f}, Vector2{0.2f, 0.1f}, button_font, &main_button_style)) {
        scene_manager.SwitchScene(SCENE_GAME, gs, ks, dt);
    } 
    if (DrawSimpleButton("Quit Game", Vector2{0.4f, 0.6f}, Vector2{0.2f, 0.1f}, button_font, &main_button_style)) {
        QuitApp(); 
    } 
    
    char score_string[64];
    if (new_high_score) {
        sprintf(score_string, "New High Score: %d", last_score);
    } else {
        sprintf(score_string, "Final Score: %d", last_score);
    }
    DrawSimpleText(score_string, {0.5, 0.8f}, UI_Alignment::CENTER_CENTER, button_font);
    
    return;
}
void GameoverClose(GameState* gs, KeyboardState* ks, double dt) {
    
    return;
}

void GameoverFree() {
    return;
}

