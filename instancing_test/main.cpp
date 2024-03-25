#define INLUDE_IMGUI
#define NO_SPLASH_SCREEN

#include "../Engine/SDL_sky.cpp"

#include <chrono>



//#define NO_INSTANCING

#define NUM_OBJECTS 1000000
#define NUM_OBJECTS_ROOT 1000


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

Vector3* mapped_buffer;

void Start(GameState* gs, KeyboardState* ks) {
    mapped_buffer = InitInstancePositionsPersistently(instance_gpu_buffers, translations, NUM_OBJECTS);
}

void Update(GameState* gs, KeyboardState* ks, double dt) {

    // Seed the random number generator
    srand(time(NULL));

    // Update the position of each object with random values
    for (int i = 0; i < NUM_OBJECTS; i++) {
        translations[i].x += (rand() % 3) - 1; 
        translations[i].y += (rand() % 3) - 1; 

        // Clamp the positions to the range [0, 100]
        if (translations[i].x > NUM_OBJECTS_ROOT) translations[i].x = 0;
        if (translations[i].x < 0) translations[i].x = NUM_OBJECTS_ROOT;
        if (translations[i].y > NUM_OBJECTS_ROOT) translations[i].y = 0;
        if (translations[i].y < 0) translations[i].y = NUM_OBJECTS_ROOT;

        mapped_buffer[i].x = translations[i].x;
        mapped_buffer[i].y = translations[i].y;
    }




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
    // Update the instance positions using buffer mapping or glBufferSubData
    //instance_gpu_buffers = InitGPUBuffersInstanced(translations, NUM_OBJECTS);
    //UpdateInstancePositions(instance_gpu_buffers, translations, NUM_OBJECTS);
    //UpdateInstancePositionsMapped(instance_gpu_buffers, translations, NUM_OBJECTS);
    DrawSpriteInstanced(sprite, instance_shaders, instance_gpu_buffers, main_camera, NUM_OBJECTS);

    #endif
    
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    static long long total_dur = duration;
    total_dur += duration;
    static long long num_frames = 0;
    num_frames++;

    printf("%lld ns avg\n", total_dur/num_frames);
}

void UserFree() {

}
        