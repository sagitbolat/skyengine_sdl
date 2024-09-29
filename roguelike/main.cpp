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
    *w = 1280;
    *h = 720;
    *width_in_world_space = 16.0f;
    *fullscreen = false;
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};

}

GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};

Tileset tile_atlas = {0};
Tileset overlay_atlas = {0};
Tileset object_atlas = {0};

Tilemap tilemap = {0};

void Awake(GameMemory* gm) {
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
    

    main_camera.position.x = 7.5f;
    main_camera.position.y = 4.0f;

    main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f}; 


    tile_atlas.height_in_tiles = 4;
    tile_atlas.width_in_tiles = 6; 
    tile_atlas.atlas = LoadSprite("main_tiles.png", shaders, gpu_buffers);
    
    overlay_atlas.height_in_tiles = 7;
    overlay_atlas.width_in_tiles = 3; 
    overlay_atlas.atlas = LoadSprite("floor_tile_overlays.png", shaders, gpu_buffers);
    
    object_atlas.height_in_tiles = 5;
    object_atlas.width_in_tiles = 9; 
    object_atlas.atlas = LoadSprite("tile_objects.png", shaders, gpu_buffers);
}



void Start(GameState* gs, KeyboardState* ks) {
    tilemap = InitTilemap(Vec2(16, 9));

    for (int y = 0; y < tilemap.height; ++y) {
        for (int x = 0; x < tilemap.width; ++x) {

            if (x % 2 == y % 2) {
                SetTile(&tilemap, Vector2Int{x, y}, TileType::ground_dark_cyan);

                SetOverlay(&tilemap, Vector2Int{x, y}, TileOverlay::blue_bricks_1);
            } else {
                SetTile(&tilemap, Vector2Int{x, y}, TileType::ground_dark_blue);
                SetOverlay(&tilemap, Vector2Int{x, y}, TileOverlay::void_overlay);

            }
            SetObject(&tilemap, Vector2Int{x, y}, TileObject::void_object);
        }
    }
}

void Update(GameState* gs, KeyboardState* ks, double dt) {

    RenderTilemap(tilemap, tile_atlas, overlay_atlas, object_atlas, shaders, gpu_buffers); 


}

void UserFree() {
    FreeTilemap(&tilemap);

}
        