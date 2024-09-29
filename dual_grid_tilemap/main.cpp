#define INLUDE_IMGUI
#include "../Engine/SDL_sky.cpp"
#include "../Engine/tilemap.h"
#include "../Engine/skymath.h"
#include "tilemap_manager.cpp"

void Init(
    int* w, int* h, 
    float* width_in_world_space, 
    bool* fullscreen,
    fColor* clear_color
) {
    *w = 720;
    *h = 720;
    *width_in_world_space = 32.0f;
    *fullscreen = false;
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}

GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};

Tileset tileset = {0};

Tilemap tilemap = {0};

void Awake(GameMemory* gm) {
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
    
    
    tileset.height_in_tiles = 4;
    tileset.width_in_tiles = 4; 

    tileset.atlas = LoadSprite("grass_dirt_tileset.png", shaders, gpu_buffers);
}



void Start(GameState* gs, KeyboardState* ks) {
    tilemap = InitTilemap(Vec2(33, 33));
    for (int x = 0; x < tilemap.width; ++x) {
        for (int y = 0; y < tilemap.height; ++y) {
            int tile_type = 1;
            SetTile(&tilemap, Vector2Int{x, y}, tile_type);
        }
    }
    main_camera.position.x  = float(16);
    main_camera.position.y  = float(16);
    main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f}; 

}

void Update(GameState* gs, KeyboardState* ks, double dt) {

    for (int x = 0; x < tilemap.width-1; ++x) {
        for (int y = 0; y < tilemap.height-1; ++y) {
            UpdateTileSprite(&tilemap, Vector2Int{x, y});
            int sprite_index = GetTileSpriteIndex(&tilemap, Vector2Int{x, y});
            DrawTile(tileset, Vector3{float(x), float(y), 0.0f}, sprite_index, shaders, gpu_buffers);
        }
    }
    Vector2 mouse_pos = GetMousePositionInWorldCoords();

    Vector2Int pos = Vector2Int{int(mouse_pos.x)+1, int(mouse_pos.y)+1};

    if (ks->state.MBL) {
        SetTile(&tilemap, pos, 0);
    }
    else if (ks->state.MBR) {
        SetTile(&tilemap, pos, 1);
    }


}

void UserFree() {
    FreeTilemap(&tilemap);

}
        