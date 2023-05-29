#include "../Engine/SDL_sky.cpp"


void MainMenuAwake() {
    return;
}

void MainMenuStart(GameState* gs, KeyboardState* ks, double dt) {
    scene_manager.SwitchScene(1, gs, ks, dt);
    return;
}

void MainMenuUpdate(GameState* gs, KeyboardState* ks, double dt) {
    return;
}
void MainMenuClose(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void MainMenuFree() {
    return;
}



