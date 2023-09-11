#define INLUDE_IMGUI
#define NO_SPLASH_SCREEN

#include "../Engine/SDL_sky.cpp"

#include <chrono>



#define NO_INSTANCING

#define NUM_OBJECTS 225
#define NUM_OBJECTS_ROOT 15


void Init(
    int* w, int* h, 
    float* width_in_world_space, 
    bool* fullscreen,
    fColor* clear_color
) {
    *w = 600;
    *h = 600;
    *width_in_world_space = float(NUM_OBJECTS_ROOT);
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

    translations = (Vector3*)calloc(NUM_OBJECTS, sizeof(Vector3));
    for (int y = 0; y < NUM_OBJECTS_ROOT; ++y) {
        for (int x = 0; x < NUM_OBJECTS_ROOT; ++x) {
            translations[(y*NUM_OBJECTS_ROOT) + x] = {float(x) + 0.5f, float(y) + 0.5f, 0.0};
        }
    }
    shaders = ShaderInit();
    gpu_buffers = InitGPUBuffers();

    instance_shaders = ShaderInit("shader.vs", "shader.fs");
    instance_gpu_buffers = InitGPUBuffersInstanced(translations, NUM_OBJECTS);
    sprite = LoadSprite("test_sprite.png", shaders, gpu_buffers);
    #ifdef NO_INSTANCING
    ShaderUse(shaders);
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
    #endif
    #ifndef NO_INSTANCING
    ShaderUse(instance_shaders);
    #endif
    
    main_camera.width = float(NUM_OBJECTS_ROOT);
    main_camera.position.x =float(NUM_OBJECTS_ROOT)/2.0f;
    main_camera.position.y =float(NUM_OBJECTS_ROOT)/2.0f;
    main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f}; 
}

void Start(GameState* gs, KeyboardState* ks) {
    
}

void Update(GameState* gs, KeyboardState* ks, double dt) {

    auto start = std::chrono::high_resolution_clock::now();
    #ifdef NO_INSTANCING 
    static Transform transform = {0.0f};
    transform.position = {0.5f, 0.5f, 0.0f};
    transform.scale = {1.0f, 1.0f, 1.0f};
    for (int y = 0; y < NUM_OBJECTS_ROOT; ++y) {
        for (int x = 0; x < NUM_OBJECTS_ROOT; ++x) {
            DrawSprite(sprite, transform, main_camera);
            transform.position.x += 1.0f;
        }
        transform.position.y += 1.0f;
        transform.position.x = 0.5f;
    }
    transform.position.x = 0.0f;
    transform.position.y = 0.0f;
    #endif
    #ifndef NO_INSTANCING
    DrawSpriteInstanced(sprite, instance_shaders, instance_gpu_buffers, main_camera, NUM_OBJECTS);
    #endif
    
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    printf("%lld\n", duration);
}

void UserFree() {

}
        