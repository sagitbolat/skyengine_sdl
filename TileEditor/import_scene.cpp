#include "../Engine/SDL_sky.cpp"


void ImportAwake() {
    return;
}

void ImportStart(GameState* gs, KeyboardState* ks, double dt) {
    scene_manager.SwitchScene(1, gs, ks, dt);
    return;
}

void ImportUpdate(GameState* gs, KeyboardState* ks, double dt) {
    return;
}
void ImportClose(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void ImportFree() {
    return;
}