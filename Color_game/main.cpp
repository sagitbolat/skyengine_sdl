#define INCLUDE_IMGUI

#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"

#define NUM_PUSH_BLOCKS 4
#define NUM_STATIC_BLOCKS 2
#define NUM_ENTITIES (1 + NUM_PUSH_BLOCKS + NUM_STATIC_BLOCKS)
#include "tilemap.h"
#include "entity.h"



Sprite player_sprite;
Sprite push_block_sprite;

// SECTION: Initialization of stuff...
void Init(int *w, int *h, float *w_in_world_space, bool *fullscreen, fColor *clear_color)
{
    //*w = 640;
    //*h = 480;
    *w_in_world_space = 16.0f;
    *fullscreen = false;
    //*clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}


Tileset tileset = {0};
Tilemap tilemap = {0};


int player; // NOTE: entity id of player. Should be 0.
EntityMap entity_id_map; // NOTE: acts as a lookup table from tilemap coordinate to an entity. a negative value indicates no entity at coordinate.
Entity* entities_array; // Array of entities in the game. Player is always first element.


GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};
Transform tile_default_transform;



void Awake(GameMemory *gm)
{
    // SECTION: renderer initializations
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});

    Sprite tileset_sprite = LoadSprite("assets/tileset.png", shaders, gpu_buffers);
    tileset.atlas = tileset_sprite;
    tileset.width_in_tiles  = 5;
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


    tile_default_transform.position = {0.0f, 0.0f, 0.0f};
    tile_default_transform.rotation = {0.0f, 0.0f, 0.0f};
    tile_default_transform.scale    = {1.0f, 1.0f, 1.0f};


    Sprite player_sprite     = LoadSprite("assets/player.png", shaders, gpu_buffers);
    Sprite push_block_sprite = LoadSprite("assets/push_block.png", shaders, gpu_buffers);
    Sprite static_block_sprite = LoadSprite("assets/static_block.png", shaders, gpu_buffers);

    entity_id_map.width = tilemap.width;
    entity_id_map.height = tilemap.height;
    entity_id_map.map = (int*)malloc(sizeof(int) * tilemap.height * tilemap.width);
    for (int i = 0; i < tilemap.height * tilemap.width; ++i) entity_id_map.map[i] = -1;

    entities_array = (Entity*)malloc(sizeof(Entity) * NUM_ENTITIES);

    
    PlayerInit(&entities_array[0], player_sprite, {7,4}, 0.2f);
    player = entities_array[0].id;

    Vector2Int push_block_positions[NUM_PUSH_BLOCKS] = {{2,2}, {3,3}, {4,4}, {5,5}};
    for (int e = 1; e < 1 + NUM_PUSH_BLOCKS; ++e) {
        PushblockInit(&entities_array[e], e, push_block_sprite, push_block_positions[e-1], entities_array[player]);
        int x = entities_array[e].position.x;
        int y = entities_array[e].position.y;
        entity_id_map.SetID(x, y, entities_array[e].id);
    }
    Vector2Int static_block_positions[NUM_STATIC_BLOCKS] = {{8,5}, {9,3}};
    for (int e = 1 + NUM_PUSH_BLOCKS; e < 1 + NUM_PUSH_BLOCKS + NUM_STATIC_BLOCKS; ++e) {
        StaticBlockInit(&entities_array[e], e, static_block_sprite, static_block_positions[e-NUM_PUSH_BLOCKS-1]);
        int x = entities_array[e].position.x;
        int y = entities_array[e].position.y;
        entity_id_map.SetID(x, y, entities_array[e].id);
    }



    main_camera.position.x = float(tilemap.width/2);
    main_camera.position.y = float(tilemap.height/2);
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
    tile_default_transform.position = Vector3{world_position.x, world_position.y, tile_default_transform.position.z};
    DrawSprite(tileset.atlas, tile_default_transform, main_camera);
    ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
    ShaderSetVector(shaders, "top_right_uv", Vector2{1.0f, 1.0f});
    ShaderSetVector(shaders, "uv_offset", Vector2{0.0f, 0.0f});
}
void DrawTile(Tileset tileset, Vector2 world_position, uint8_t atlas_index) {
    int atlas_x = atlas_index % tileset.width_in_tiles;
    int atlas_y = atlas_index / tileset.width_in_tiles;
    DrawTile(tileset, world_position, atlas_x, atlas_y);
}




void Update(GameState *gs, KeyboardState *ks, double dt) {


    
    if (ks->state.W && !entities_array[player].movable.moving) { 
        EntityMove(player, {0, 1}, tilemap, entity_id_map, entities_array);
    }
    if (ks->state.S && !entities_array[player].movable.moving) {
        EntityMove(player, {0, -1}, tilemap, entity_id_map, entities_array);
    }
    if (ks->state.A && !entities_array[player].movable.moving) {
        EntityMove(player, {-1, 0}, tilemap, entity_id_map, entities_array);
    }
    if (ks->state.D && !entities_array[player].movable.moving) {
        EntityMove(player, {1, 0}, tilemap, entity_id_map, entities_array);
    }


    if (ks->state.SPACE && !ks->prev_state.SPACE) {
        printf("############################\n");
        for (int id = 0; id < NUM_ENTITIES; ++id) {
            Vector2Int position = entities_array[id].position;
            printf("Entity %d position: {%d, %d} %d\n", id, position.x, position.y, entities_array[id].movable.active);
        }
    }


    for (int id = 0; id < NUM_ENTITIES; ++id) {
        EntityUpdateTransform(id, entities_array, dt);
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
    for (int e = 0; e < NUM_ENTITIES; ++e) {
        if (!entities_array[e].active) continue;
        DrawSprite(entities_array[e].sprite, entities_array[e].transform, main_camera);
    }

}

void UserFree()
{

    free(tilemap.map);
    tilemap.map = nullptr;

    FreeSprite(tileset.atlas);
    FreeSprite(entities_array[player].sprite);
    entities_array[player].active = false;

    FreeSprite(entities_array[1].sprite);

    FreeGPUBuffers(gpu_buffers);
    FreeShaders(shaders);
}
