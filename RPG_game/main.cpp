#define NO_SPLASH_SCREEN
#include "../Engine/SDL_sky.cpp"


#include "globals.h"
#include "entity.cpp"
#include <math.h>

GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};

Sprite player_sprite;
Sprite map1_sprite;

Sprite cursor_sprite;

// SECTION: Initialization of stuff...
void Init(int *w, int *h, float *w_in_world_space, bool *fullscreen, fColor *clear_color)
{
    *w = SCREEN_W;
    *h = SCREEN_H;
    *w_in_world_space = 24.0f;
    *fullscreen = FULLSCREEN;
    //*clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
    *clear_color =BACKGROUND_COLOR;
}



void Awake(GameMemory* gm) {
    main_camera.position.x  = 0.0f;
    main_camera.position.y  = 0.0f;
    main_camera.position.z  = 3.0f;
    main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f};
    //main_camera.projection = PERSPECTIVE_CAMERA; 
    
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();

    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});

    player_sprite = LoadSprite("assets/player.png", shaders, gpu_buffers);
    map1_sprite = LoadSprite("assets/map_1.png", shaders, gpu_buffers);

    cursor_sprite = LoadSprite("assets/cursor.png", shaders, gpu_buffers);
}

// SECTION: Gameplay data



Player player = {0};

void Start(GameState* gs, KeyboardState* ks) {
    player.position.x = 0;
    player.position.y = 0;
}

void Update(GameState* gs, KeyboardState* ks, double dt) {

    Transform map_transform = {0.0f};

    map_transform.position.x = 0.0f;
    map_transform.position.y = 0.0f;
    map_transform.position.z = -1.0f;
    map_transform.scale.x = 35.0f;
    map_transform.scale.y = 21.0f;
    map_transform.scale.z = 1.0f;
    

    Transform player_transform = {0.0f};
    DrawSprite(map1_sprite, map_transform, main_camera);

    player_transform.position.x = player.position.x;
    player_transform.position.y = player.position.y;
    player_transform.scale.x = 1.0f;
    player_transform.scale.y = 1.0f;
    player_transform.scale.z = 1.0f;
    DrawSprite(player_sprite, player_transform, main_camera);

    Vector2 mouse_pos = GetMousePositionInWorldCoords();
    

    static bool player_selected = false;

    if (ks->state.MBL && !ks->prev_state.MBL) {
    // NOTE: if mouse collides with player
        if (mouse_pos.x <= player_transform.position.x + player_transform.scale.x * 0.5f
            && mouse_pos.x >= player_transform.position.x - player_transform.scale.x * 0.5f
            && mouse_pos.y <= player_transform.position.y + player_transform.scale.y * 0.5f
            && mouse_pos.y >= player_transform.position.y - player_transform.scale.y * 0.5f
        ) {
            printf("Selected Player\n");
            player_selected = true;
        } else if (player_selected) {
            printf("Moved Player\n");
            player_selected = false;
            mouse_pos.x += copysign(0.5f, mouse_pos.x);
            mouse_pos.y += copysign(0.5f, mouse_pos.y);
            player.position.x = (int)mouse_pos.x;
            player.position.y = (int)mouse_pos.y;
        }
    }

    if (player_selected) {
        Transform cursor_transform = {0.0f};
        cursor_transform.position.x = (int)(mouse_pos.x + copysign(0.5f, mouse_pos.x)); 
        cursor_transform.position.y = (int)(mouse_pos.y + copysign(0.5f, mouse_pos.y)); 
        cursor_transform.position.z = 1.0f;
        cursor_transform.scale.x = 1.0f;
        cursor_transform.scale.y = 1.0f;
        cursor_transform.scale.z = 1.0f;

        DrawSprite(cursor_sprite, cursor_transform, main_camera);
    }


    if (ks->state.W && !ks->prev_state.W) {
        main_camera.position.z++; 
    }
    if (ks->state.A && !ks->prev_state.A) {
        --player.position.x;
    }
    if (ks->state.S && !ks->prev_state.S) {
        main_camera.position.z--; 
    }
    if (ks->state.D && !ks->prev_state.D) {
        ++player.position.x;
    }
}


void UserFree() {

}

