#define INLUDE_IMGUI
#define NO_SPLASH_SCREEN

#include "../Engine/SDL_sky.cpp"

#include <chrono>




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
    ShaderUse(instance_shaders);
    
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

    auto start_frame = std::chrono::high_resolution_clock::now();
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



    auto start_render = std::chrono::high_resolution_clock::now();

    
    
    DrawSpriteInstanced(sprite, instance_shaders, instance_gpu_buffers, main_camera, NUM_OBJECTS);

    
    auto end = std::chrono::high_resolution_clock::now();


    auto frame_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_frame).count();
    auto render_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_render).count();

    static long long rendering_dur = 0;
    static long long total_dur = 0;
    static long long num_frames = 0;

    // Skip the first 100 frames
    if (num_frames > 100) {
        total_dur += frame_duration;
        rendering_dur += render_duration;
        long long sim_dur = total_dur - rendering_dur;
        // Calculate the average duration in nanoseconds per frame
        long long avg_duration_ns_render = rendering_dur / (num_frames - 100);
        long long avg_duration_ns_sim = sim_dur / (num_frames - 100);
        long long avg_duration_ns_frame = total_dur / (num_frames - 100);

        // Convert the average duration to average FPS
        double avg_fps_render = 1e9 / avg_duration_ns_render;
        double avg_fps_sim = 1e9 / avg_duration_ns_sim;
        double avg_fps_frame = 1e9 / avg_duration_ns_frame;
        if (ks->state.SPACE && !ks->prev_state.SPACE) {
            printf("Frame data:      %.2f FPS | %f ms\n", avg_fps_frame, double(avg_duration_ns_frame)/double(1e6));
            printf("Rendering data:  %.2f FPS | %f ms\n", avg_fps_render, double(avg_duration_ns_render)/double(1e6));
            printf("Simulation data: %.2f FPS | %f ms\n", avg_fps_sim, double(avg_duration_ns_sim)/double(1e6));
        }
    } else {
        if (ks->state.SPACE && !ks->prev_state.SPACE) {
            printf("Calculating FPS \n");
            
        }
    } 

    num_frames++;


}

void UserFree() {

}
        