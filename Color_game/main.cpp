#define INCLUDE_IMGUI
#define FPS_SHOW

#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"



// SECTION: Initialization of stuff...
void Init(int *w, int *h, float *w_in_world_space, bool *fullscreen, fColor *clear_color)
{
    //*w = 640;
    //*h = 480;
    *w_in_world_space = 15.0f;
    //*clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}

GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};
Sprite sprite;
Transform transform;
void Awake(GameMemory *gm)
{
    // SECTION: renderer initializations
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});

    sprite = LoadSprite("assets/tile.png", shaders, gpu_buffers);
    transform.position = {0.0f, 0.0f, 0.0f};
    transform.rotation = {0.0f, 0.0f, 0.0f};
    transform.scale = {1.0f, 1.0f, 1.0f};
}

void Start(GameState *gs, KeyboardState *ks)
{
}

void Update(GameState *gs, KeyboardState *ks, double dt)
{
    ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
    ShaderSetVector(shaders, "top_right_uv", Vector2{0.5f, 0.5f});
    ShaderSetVector(shaders, "uv_offset", Vector2{0.5f, 0.0f});
    DrawSprite(sprite, transform, main_camera);
}

void UserFree()
{
}
