#include "../Engine/sky_structs.h"


Transform tile_default_transform;
Sprite 

void EditorAwake() {
    tile_default_transform.position = {0.0f, 0.0f, 0.0f};
    tile_default_transform.rotation = {0.0f, 0.0f, 0.0f};
    tile_default_transform.scale    = {1.0f, 1.0f, 1.0f};

    return;
}

void EditorStart(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void EditorUpdate(GameState* gs, KeyboardState* ks, double dt) {
    UI_SubwindowStart("Object Palette", {1280, 108}, {0, 0});
    DrawSimpleImageButton("Tile1") 
    UI_SubwindowEnd(); 
    return;
}
void EditorClose(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void EditorFree() {
    return;
}