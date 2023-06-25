#define INCLUDE_IMGUI

#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"

#define NUM_ENTITIES 5

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




struct Entity {
    int id;
    bool active; // NOTE: This is equivalent of a null state if this is false.
    Sprite sprite;
    Transform transform;
    Vector2Int prev_position;
    Vector2Int position;
    bool movable; // NOTE: whether an entity is movable by the player.
    float seconds_per_block; // NOTE: measure of how long it takes the player to move 1 block
    float move_timer;
    bool moving; // NOTE: whether the player is moving
};

void EntityInit (
    Entity* entity, 
    int id, 
    Sprite sprite, 
    Vector2Int position={0, 0}, 
    bool active=true, 
    float seconds_per_block=0.2f,
    bool movable=true 
) {
    entity->id                = id;
    entity->sprite            = sprite;
    entity->active            = active;
    entity->prev_position     = position; // NOTE: Cached for transform updates. Do not modify manually
    entity->position          = position;
    entity->movable           = movable;
    entity->seconds_per_block = seconds_per_block;
    entity->move_timer        = 0.0f;
    entity->moving            = false;

    Transform transform = {0.0f};
    transform.position  = {(float)position.x, (float)position.y, 1.0f};
    transform.rotation  = {0.0f, 0.0f, 0.0f};
    transform.scale     = {1.0f, 1.0f, 1.0f};
    
    entity->transform   = transform;
}

int player; // NOTE: entity id of player. Should be 0.
int* entity_id_map; // NOTE: acts as a lookup table from tilemap coordinate to an entity. a negative value indicates no entity at coordinate.
Entity* entities_array; // Array of entities in the game. Player is always first element.



int GetEntityMapID(int x, int y) {
    return entity_id_map[y * tilemap.width + x];
}
void SetEntityMapID(int x, int y, int id) {
    entity_id_map[y * tilemap.width + x] = id;
}


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


    entity_id_map = (int*)malloc(sizeof(int) * tilemap.height * tilemap.width);
    for (int i = 0; i < tilemap.height * tilemap.width; ++i) entity_id_map[i] = -1;

    entities_array = (Entity*)malloc(sizeof(Entity) * NUM_ENTITIES);

    EntityInit(     // NOTE: initialize player
        &entities_array[0], 0,
        player_sprite,
        {7,4}
    );

    Vector2Int push_block_positions[9] = {{2,2}, {3,3}, {4,4}, {5,5}};
    for (int e = 1; e < NUM_ENTITIES; ++e) {
        EntityInit(
            &entities_array[e], e,
            push_block_sprite,
            push_block_positions[e-1] 
        );
        int x = entities_array[e].position.x;
        int y = entities_array[e].position.y;
        SetEntityMapID(x, y, entities_array[e].id);
    }

    player = entities_array[0].id;


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


// NOTE: Returns true if colliding with wall, false if not colliding with wall.
bool TestTileCollide(Tilemap map, Vector2Int tile_pos) {
    int tile = map.map[tile_pos.y * map.width + tile_pos.x];
    if ((tile == 1 || tile == 0)) return false;
    else return true;
}

// NOTE: Returns true if colliding with an entity, false if not colliding with an entity.
bool TestEntityCollide(Vector2Int tile_pos) {
    int entity_id = GetEntityMapID(tile_pos.x, tile_pos.y);
    if (entity_id < 0) return false;
    else return true;
}

// NOTE: Returns true if moving was successful. false if was unable to move the entity
bool EntityMove(int entity_id, Vector2Int direction, Tilemap map, int* entity_id_map, Entity* entity_array) {
    Entity* entity = &entity_array[entity_id];
    
    if (!entity->movable) return false;

    Vector2Int new_position = entity->position + direction;

    if (TestTileCollide(map, new_position)) return false;


    int new_entity_id = GetEntityMapID(new_position.x, new_position.y);
    Entity* new_entity = &entity_array[new_entity_id]; 

    if (new_entity_id < 0 || !new_entity->active) {
        entity->moving = true;

        entity->position = entity->position + direction;

        SetEntityMapID(entity->position.x, entity->position.y, entity->id);
        SetEntityMapID(entity->prev_position.x, entity->prev_position.y, -1);
        
        return true;
    }
    
    
    if (!new_entity->movable) return false;

    bool could_move_new_entity = EntityMove(new_entity_id, direction, map, entity_id_map, entity_array);

    if (could_move_new_entity) {
        entity->moving = true;

        entity->position = entity->position + direction;

        SetEntityMapID(entity->position.x, entity->position.y, entity->id);
        SetEntityMapID(entity->prev_position.x, entity->prev_position.y, -1);
        
        return true;
    } else return false;


}

void EntityUpdateTransform(int entity_id, Entity* entity_array, float dt) {
    Entity* entity = &entity_array[entity_id];

    if (!entity->active) return;

    if (entity->moving) entity->move_timer += dt;
    float move_t = entity->move_timer / (entity->seconds_per_block * 1000);

    entity->transform.position.y = Lerp((float)entity->prev_position.y, (float)entity->position.y, move_t);
    entity->transform.position.x = Lerp((float)entity->prev_position.x, (float)entity->position.x, move_t);

    if (move_t >= 1.0f) {
        entity->transform.position.y = (float)entity->position.y;
        entity->transform.position.x = (float)entity->position.x;


        entity->move_timer = 0.0f;
        entity->moving = false;
        entity->prev_position = entity->position;
    }
}





void Update(GameState *gs, KeyboardState *ks, double dt) {


    
    if (ks->state.W && !entities_array[player].moving) { 
        EntityMove(player, {0, 1}, tilemap, entity_id_map, entities_array);
    }
    if (ks->state.S && !entities_array[player].moving) {
        EntityMove(player, {0, -1}, tilemap, entity_id_map, entities_array);
    }
    if (ks->state.A && !entities_array[player].moving) {
        EntityMove(player, {-1, 0}, tilemap, entity_id_map, entities_array);
    }
    if (ks->state.D && !entities_array[player].moving) {
        EntityMove(player, {1, 0}, tilemap, entity_id_map, entities_array);
    }


    if (ks->state.SPACE && !ks->prev_state.SPACE) {
        printf("############################\n");
        for (int id = 0; id < NUM_ENTITIES; ++id) {
            Vector2Int position = entities_array[id].position;
            printf("Entity %d position: {%d, %d} %d\n", id, position.x, position.y, entities_array[id].movable);
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
