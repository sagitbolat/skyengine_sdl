#define INCLUDE_IMGUI
#define FPS_SHOW
#include "../Engine/SDL_sky.cpp"
#include "../Engine/scene.h"

// SECTION: Initialization of stuff...
void Init(int *w, int *h, float *w_in_world_space, bool *fullscreen, fColor *clear_color)
{
    *w = 1280;
    *h = 720;
    *w_in_world_space = 15.0f;
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}

GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};


// SECTION: SCENE
#define NUM_SCENES 2

#define SCENE_IMPORT_MENU 0
#define SCENE_EDITOR 1

SceneManager scene_manager = {0};



// SECTION: UI Fonts

ImFont* font_s = nullptr;
ImFont* font_m = nullptr;
ImFont* font_l = nullptr;


ButtonStyle button_style = {0.0f};

#include "import_scene.cpp"
#include "editor_scene.cpp"

void Awake(GameMemory *gm)
{    
    // SECTION: renderer initializations
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});



    // SECTION: SceneManager init
    scene_manager.InitManager(NUM_SCENES);

    // Init individual scenes
    scene_manager.AwakeScene(SCENE_IMPORT_MENU, ImportAwake, ImportStart, ImportUpdate, ImportClose, ImportFree);
    scene_manager.AwakeScene(SCENE_EDITOR, EditorAwake, EditorStart, EditorUpdate, EditorClose, EditorFree);


    // SECTION: Load Fonts
    font_s = LoadFont("./roboto.ttf", 16.0f);
    font_m = LoadFont("./roboto.ttf", 25.0f);
    font_l = LoadFont("./roboto.ttf", 32.0f);

    // SECTION: Initializing button style
    button_style.button_idle_color = {0.9f, 0.9f, 0.9f, 1.0f};
    button_style.button_hover_color = {0.7f, 0.7f, 0.7f, 1.0f};
    button_style.button_active_color = {1.0f, 1.0f, 1.0f, 1.0f};

    button_style.outline_color = {0.4f, 0.4, 0.4f, 1.0f};
    button_style.text_color = {0.0f, 0.0f, 0.0f, 1.0f};
    
    button_style.button_alpha = 1.0f;
    button_style.text_alignment = {0.5f, 0.5f}; // NOTE: Centered
    button_style.corner_rounding = 0.0f;
    button_style.border_size = 0.1f;
    button_style.frame_padding = {0.0f, 0.0f};

}

void Start(GameState *gs, KeyboardState *ks)
{
    scene_manager.scenes[0].StartScene(gs, ks, 0.0);
}

void Update(GameState *gs, KeyboardState *ks, double dt)
{
    scene_manager.SceneUpdate(gs, ks, dt);
}

void UserFree()
{
    scene_manager.FreeScene(0);
    scene_manager.FreeScene(1);
    scene_manager.FreeManager();
}
