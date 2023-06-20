#define INCLUDE_IMGUI
#define FPS_SHOW

#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"



int tilemap[15*9] = {
    10,3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,11,
    9, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 7,
    9, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 7,
    9, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 7,
    9, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 7,
    9, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 7,
    9, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 7,
    9, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 7,
    5,13,13,13,13,13,13,13,13,13,13,13,13,13, 6
};



// SECTION: Initialization of stuff...
void Init(int *w, int *h, float *w_in_world_space, bool *fullscreen, fColor *clear_color)
{
    //*w = 640;
    //*h = 480;
    *w_in_world_space = 16.0f;
    //*clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}

struct Tileset {
    Sprite atlas;
    int width_in_tiles;
    int height_in_tiles;
};
Tileset tileset = {0};


GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};
Transform transform;
void Awake(GameMemory *gm)
{
    // SECTION: renderer initializations
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});

    Sprite sprite = LoadSprite("assets/tileset.png", shaders, gpu_buffers);
    tileset.atlas = sprite;
    tileset.width_in_tiles = 5;
    tileset.height_in_tiles = 4;

    transform.position = {0.0f, 0.0f, 0.0f};
    transform.rotation = {0.0f, 0.0f, 0.0f};
    transform.scale = {1.0f, 1.0f, 1.0f};
}

void Start(GameState *gs, KeyboardState *ks) {

}

void DrawTile(Tileset map, Vector2 world_position, int atlas_x, int atlas_y) {
    float uv_width = float(1)/float(map.width_in_tiles);
    float uv_height = float(1)/float(map.height_in_tiles);
    float uv_x_offset = atlas_x * uv_width;
    float uv_y_offset = atlas_y * uv_height;

    ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
    ShaderSetVector(shaders, "top_right_uv", Vector2{uv_width, uv_height});
    ShaderSetVector(shaders, "uv_offset", Vector2{uv_x_offset, uv_y_offset});
    transform.position = Vector3{world_position.x, world_position.y, transform.position.z};
    DrawSprite(map.atlas, transform, main_camera);
}
void DrawTile(Tileset map, Vector2 world_position, uint8_t atlas_index) {
    int atlas_x = atlas_index % map.width_in_tiles;
    int atlas_y = atlas_index / map.width_in_tiles;
    DrawTile(map, world_position, atlas_x, atlas_y);
}

void Update(GameState *gs, KeyboardState *ks, double dt) {


    
    for(int x = 0; x < 15; ++x) {
        for (int y = 0; y < 9; ++y) {
            int x_ = x - 7;
            int y_ = y - 4;
            DrawTile(tileset, {float(x_), float(y_)}, tilemap[(8-y)*15 + x]);
        }
    }
    
    

}

void UserFree()
{
}
