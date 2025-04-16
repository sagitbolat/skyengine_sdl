#define INCLUDE_IMGUI
#define NO_SPLASH_SCREEN


#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"
#include "../Engine/scene.h"


// SECTION: Initialization of stuff...
void Init(int *w, int *h, float *w_in_world_space, bool *fullscreen, fColor *clear_color)
{
    *w = 1920;
    *h = 1080;
    *fullscreen = true;
    *w_in_world_space = 15.0f * 1.5f;
    //*clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}

// SCENE
#define NUM_SCENES 3

#define SCENE_MAIN_MENU 0
#define SCENE_SELECTION 1
#define SCENE_GAME 2

SceneManager scene_manager = {0};

// SECTION: UI Fonts
ImFont* button_font = nullptr;
ImFont* title_font = nullptr;
ImFont* large_font = nullptr;
fColor font_color = {(float)125/255, (float)212/255, (float)221/255, 1.0f};

ButtonStyle main_button_style = {0.0f};


// SECTION: Rendering
GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};


#include "mainmenu_scene.cpp"
#include "selection_scene.cpp"
#include "game_scene.cpp"

void Awake(GameMemory *gm)
{
    // SECTION: renderer initializations
    shaders = ShaderInit();
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});


    // SECTION: SceneManager init
    scene_manager.InitManager(NUM_SCENES);

    // Init individual scenes
    scene_manager.AwakeScene(SCENE_MAIN_MENU, MainMenuAwake, MainMenuStart, MainMenuUpdate, MainMenuClose, MainMenuFree);
    scene_manager.AwakeScene(SCENE_SELECTION, SelectionAwake, SelectionStart, SelectionUpdate, SelectionClose, SelectionFree);
    scene_manager.AwakeScene(SCENE_GAME, GameAwake, GameStart, GameUpdate, GameClose, GameFree);


    // SECTION: Load Fonts
    button_font = LoadFont("pixel.ttf", 48.0f);
    title_font = LoadFont("pixel.ttf", 80.0f);
    large_font = LoadFont("pixel.ttf", 128.0f);

    // SECTION: Initializing button style
    main_button_style.button_idle_color = font_color;
    main_button_style.button_hover_color = font_color * 0.8f;
    main_button_style.button_active_color = font_color * 0.9f;

    main_button_style.outline_color = font_color * 0.5f;
    main_button_style.text_color = {0.0f, 0.0f, 0.0f, 1.0f};
    
    main_button_style. button_alpha = 1.0f;
    main_button_style.text_alignment = {0.5f, 0.5f}; // NOTE: Centered
    main_button_style.corner_rounding = 0.0f;
    main_button_style.border_size = 3.0f;
    main_button_style.frame_padding = {0.0f, 0.0f};

}

void Start(GameState *gs, KeyboardState *ks)
{
    // NOTE: Start background music and then start first scene
    scene_manager.scenes[0].StartScene(gs, ks, 0.0);
}

void Update(GameState *gs, KeyboardState *ks, double dt)
{
    UI_Window_Options ui_wo = {true, true, true, true, true, true};
    UI_WindowStart("Main", {SCREEN_WIDTH, SCREEN_HEIGHT}, {0, 0}, &ui_wo);
    scene_manager.SceneUpdate(gs, ks, dt);
    UI_WindowEnd();
}

void UserFree()
{
    scene_manager.FreeScene(0);
    scene_manager.FreeScene(1);
    scene_manager.FreeScene(2);
    scene_manager.FreeManager();

    // NOTE: Renderer Freeing

    FreeShaders(shaders);
    FreeGPUBuffers(gpu_buffers);
}
