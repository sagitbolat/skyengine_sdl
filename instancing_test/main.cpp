#define INLUDE_IMGUI
#define NO_SPLASH_SCREEN
#include "../Engine/SDL_sky.cpp"

void Init(
    int* w, int* h, 
    float* width_in_world_space, 
    bool* fullscreen,
    fColor* clear_color
) {
    *w = 600;
    *h = 600;
    *width_in_world_space = 10.f;
    *fullscreen = false;
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}

GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};
GL_ID* instance_shaders = nullptr;
GPUBufferIDs instance_gpu_buffers = {nullptr};
Sprite sprite;

Vector3* translations = nullptr;

void Awake(GameMemory* gm) {
    shaders = ShaderInit();
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
    sprite = LoadSprite("test_sprite.png", shaders, gpu_buffers);
    
    
    instance_shaders = ShaderInit("shader.vs", "shader.fs");
    instance_gpu_buffers = InitGPUBuffersInstanced(translations, 100);
    
    ShaderUse(shaders);
    
    
    main_camera.width = 10.0f;
    main_camera.position.x = 5.5f;
    main_camera.position.y = 5.5f;
    main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f}; 
}

void Start(GameState* gs, KeyboardState* ks) {
    
}

void Update(GameState* gs, KeyboardState* ks, double dt) {
    
    static Transform transform = {0.0f};
    transform.scale = {1.0f, 1.0f, 1.0f};
    for (int y = 0; y < 10; ++y) {
        transform.position.y += 1.0f;
        for (int x = 0; x < 10; ++x) {
            transform.position.x += 1.0f;
            DrawSprite(sprite, transform, main_camera);
        }
        transform.position.x = 0.0f;
    }
    transform.position.x = 0.0f;
    transform.position.y = 0.0f;
    

    
}

void UserFree() {

}
        