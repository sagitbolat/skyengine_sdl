#include "../Engine/SDL_sky.cpp"


void MainMenuAwake() {
    return;
}

void MainMenuStart(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void MainMenuUpdate(GameState* gs, KeyboardState* ks, double dt) {

    DrawSimpleText("Main Menu", Vector2{0.5f, 0.2f}, 5.0f, true);

    if (DrawSimpleButton("Start Game", Vector2{0.4f, 0.35f}, Vector2{0.2f, 0.1f}, 5.0f)) {
        scene_manager.SwitchScene(SCENE_GAME, gs, ks, dt);
    } 
    if (DrawSimpleButton("Quit Game", Vector2{0.4f, 0.55f}, Vector2{0.2f, 0.1f}, 5.0f)) {
        QuitApp(); 
    }     
    char score_string[64];
    sprintf(score_string, "Current High Score: %d", high_score);
    DrawSimpleText(score_string, {0.5f, 0.8f}, 5.0f, true);
    return;
}
void MainMenuClose(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void MainMenuFree() {
    return;
}



