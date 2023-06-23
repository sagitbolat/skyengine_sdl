#define INCLUDE_IMGUI
#define FPS_SHOW

#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"





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

struct Tilemap {
    int* map;
    int width;
    int height;
};
Tilemap tilemap = {0};




struct Player {
    Sprite sprite;
    Transform transform;
    Vector2Int prev_position;
    Vector2Int position;
    bool moving;
    float seconds_per_block; // NOTE: measure of how long it takes the player to move 1 block
};
Player player;



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

    tilemap.width = 15;
    tilemap.height = 9;
    int map[tilemap.width * tilemap.height] = {
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
    tilemap.map = (int*)calloc(tilemap.width * tilemap.height, sizeof(int));
    memcpy(tilemap.map, map, sizeof(int) * tilemap.height * tilemap.width);


    transform.position = {0.0f, 0.0f, 0.0f};
    transform.rotation = {0.0f, 0.0f, 0.0f};
    transform.scale = {1.0f, 1.0f, 1.0f};

    player.sprite = LoadSprite("assets/player.png", shaders, gpu_buffers);
    player.transform = transform;
    player.transform.position.z = 1.0f;
    player.prev_position = {7, 4};
    player.position = {7, 4};
    player.seconds_per_block = 0.15f;   

    main_camera.position.x = 7.0f;
    main_camera.position.y = 4.0f;
    main_camera.look_target = {7.0f, 4.0f, 0.0f}; 
}

void Start(GameState *gs, KeyboardState *ks) {

}

void DrawTile(Tileset tileset, Vector2 world_position, int atlas_x, int atlas_y) {
    float uv_width = float(1)/float(tileset.width_in_tiles);
    float uv_height = float(1)/float(tileset.height_in_tiles);
    float uv_x_offset = atlas_x * uv_width;
    float uv_y_offset = atlas_y * uv_height;

    ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
    ShaderSetVector(shaders, "top_right_uv", Vector2{uv_width, uv_height});
    ShaderSetVector(shaders, "uv_offset", Vector2{uv_x_offset, uv_y_offset});
    transform.position = Vector3{world_position.x, world_position.y, transform.position.z};
    DrawSprite(tileset.atlas, transform, main_camera);
    ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
    ShaderSetVector(shaders, "top_right_uv", Vector2{1.0f, 1.0f});
    ShaderSetVector(shaders, "uv_offset", Vector2{0.0f, 0.0f});
}
void DrawTile(Tileset tileset, Vector2 world_position, uint8_t atlas_index) {
    int atlas_x = atlas_index % tileset.width_in_tiles;
    int atlas_y = atlas_index / tileset.width_in_tiles;
    DrawTile(tileset, world_position, atlas_x, atlas_y);
}


// NOTE: Returns true if colliding with wall, false if not colliding with wall.
bool TestTileCollide(Tilemap map, Vector2Int tile_pos) {
    int tile = map.map[tile_pos.y * map.width + tile_pos.x];
    if (tile == 1 || tile == 0) return false;
    else return true;
}



void Update(GameState *gs, KeyboardState *ks, double dt) {


    
    if (ks->state.W && !player.moving && !TestTileCollide(tilemap, {player.position.x, player.position.y+1})) {
        player.moving = true;
        ++player.position.y;
    }
    if (ks->state.S && !player.moving && !TestTileCollide(tilemap, {player.position.x, player.position.y-1})) {
        player.moving = true;
        --player.position.y;
    }
    if (ks->state.A && !player.moving && !TestTileCollide(tilemap, {player.position.x-1, player.position.y})) {
        player.moving = true;
        --player.position.x;
    }
    if (ks->state.D && !player.moving && !TestTileCollide(tilemap, {player.position.x+1, player.position.y})) {
        player.moving = true;
        ++player.position.x;
    }


    static float move_timer = 0.0f;
    if (player.moving) move_timer += dt;
    float move_t = move_timer / (player.seconds_per_block * 1000);

    player.transform.position.y = Lerp((float)player.prev_position.y, (float)player.position.y, move_t);
    player.transform.position.x = Lerp((float)player.prev_position.x, (float)player.position.x, move_t);

    if (move_t >= 1.0f) {
        player.transform.position.y = (float)player.position.y;
        player.transform.position.x = (float)player.position.x;
        move_timer = 0.0f;
        player.moving = false;
        player.prev_position = player.position;
    }
    
    for(int x = 0; x < tilemap.width; ++x) {
        for (int y = 0; y < tilemap.height; ++y) {
            if (x == 0 || y == 0 || x == (tilemap.width-1) || y == (tilemap.height-1))
                ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
            else ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
            DrawTile(tileset, {float(x), float(y)}, tilemap.map[(8-y)*15 + x]);
        }
    }

    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
    DrawSprite(player.sprite, player.transform, main_camera);
}

void UserFree()
{
    FreeSprite(tileset.atlas);
    FreeSprite(player.sprite);

    FreeGPUBuffers(gpu_buffers);
    FreeShaders(shaders);
}
