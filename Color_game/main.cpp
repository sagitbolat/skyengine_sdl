#define INCLUDE_IMGUI
#define DEBUG_UI
#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"

#define MAX_ENTITIES 91 
#include "tilemap.h"
#include "entity.h"
#include "level_loader.h"




// SECTION: Initialization of stuff...
void Init(int *w, int *h, float *w_in_world_space, bool *fullscreen, fColor *clear_color)
{
    *w = 1280;
    *h = 720;
    *w_in_world_space = 14.0f;
    *fullscreen = false;
    //*clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}


Tileset tileset = {0};
Tilemap tilemap = {0};


Sprite player_sprite; 
Sprite player_up_sprite; 
Sprite player_down_sprite; 
Sprite player_left_sprite; 
Sprite player_right_sprite; 
Sprite push_block_sprite;        
Sprite static_block_sprite;      
Sprite emitter_sprite;     
Sprite emitter_nozzle_sprite;    
Sprite emitter_indicator_sprite; 
Sprite receiver_sprite;          
Sprite receiver_nozzle_sprite;   
Sprite receiver_indicator_sprite;
Sprite open_door_sprite;
Sprite closed_door_sprite;
Sprite endgoal_sprite;




int player;         // NOTE: entity id of player. Should be 0.
int push_block;     // NOTE: entity id of first push_block.
int static_block;   // NOTE: entity id of first static_block.
int emitter;        // NOTE: entity id of first emitter.
int emission;       // NOTE: entity id of first emission.


EntityMap entity_id_map; // NOTE: acts as a lookup table from tilemap coordinate to an entity. a negative value indicates no entity at coordinate.
Entity* entities_array; // Array of entities in the game. Player is always first element.
EmissionMap emission_map;
Sprite emission_sprite;

GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};
Transform tile_default_transform;


#ifdef DEBUG_UI
#define LEVEL_NAME_MAX 24
char level_name[LEVEL_NAME_MAX] = "";
#endif

void Awake(GameMemory *gm)
{
    // SECTION: renderer initializations
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});



    Sprite tileset_sprite   = LoadSprite("assets/tileset.png", shaders, gpu_buffers);
    tileset.atlas           = tileset_sprite;
    tileset.width_in_tiles  = 5;
    tileset.height_in_tiles = 3;

    tilemap.width   = 15;
    tilemap.height  = 9;
    int map[tilemap.width * tilemap.height] = {
        10,3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,11,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        5,13,13,13,13,13,13,13,13,13,13,13,13,13, 6
    };

    tilemap.map = (int*)calloc(tilemap.width * tilemap.height, sizeof(int));
    memcpy(tilemap.map, map, sizeof(int) * tilemap.height * tilemap.width);



    tile_default_transform.position = {0.0f, 0.0f, -1.0f};
    tile_default_transform.rotation = {0.0f, 0.0f, 0.0f};
    tile_default_transform.scale    = {1.0f, 1.0f, 1.0f};



    player_sprite               = LoadSprite("assets/player.png", shaders, gpu_buffers);
    player_up_sprite            = LoadSprite("assets/player_up.png", shaders, gpu_buffers);
    player_down_sprite          = LoadSprite("assets/player_down.png", shaders, gpu_buffers);
    player_left_sprite          = LoadSprite("assets/player_left.png", shaders, gpu_buffers);
    player_right_sprite         = LoadSprite("assets/player_right.png", shaders, gpu_buffers);
    push_block_sprite           = LoadSprite("assets/push_block.png", shaders, gpu_buffers);
    static_block_sprite         = LoadSprite("assets/static_block.png", shaders, gpu_buffers);
    emitter_sprite              = LoadSprite("assets/emitter.png", shaders, gpu_buffers);
    emitter_nozzle_sprite       = LoadSprite("assets/emitter_nozzle.png", shaders, gpu_buffers);
    emitter_indicator_sprite    = LoadSprite("assets/emitter_indicator.png", shaders, gpu_buffers);
    receiver_sprite             = LoadSprite("assets/receiver.png", shaders, gpu_buffers);
    receiver_nozzle_sprite      = LoadSprite("assets/receiver_nozzle.png", shaders, gpu_buffers);
    receiver_indicator_sprite   = LoadSprite("assets/receiver_indicator.png", shaders, gpu_buffers);
    emission_sprite             = LoadSprite("assets/emission.png", shaders, gpu_buffers);
    open_door_sprite            = LoadSprite("assets/door_open.png", shaders, gpu_buffers);
    closed_door_sprite          = LoadSprite("assets/door_closed.png", shaders, gpu_buffers);
    endgoal_sprite              = LoadSprite("assets/endgoal.png", shaders, gpu_buffers);


    entity_id_map.width     = tilemap.width;
    entity_id_map.height    = tilemap.height;
    entity_id_map.depth     = 2;
    entity_id_map.map       = (int*)malloc(sizeof(int) * tilemap.height * tilemap.width * entity_id_map.depth);
    for (int i = 0; i < tilemap.height * tilemap.width * entity_id_map.depth; ++i) entity_id_map.map[i] = -1;

    entities_array = (Entity*)calloc(sizeof(Entity), MAX_ENTITIES);

    emission_map.width  = tilemap.width;
    emission_map.height = tilemap.height;
    emission_map.map    = (EmissionTile*)calloc(emission_map.width * emission_map.height, sizeof(EmissionTile));
    for (int i = 0; i < emission_map.width * emission_map.height; ++i) emission_map.map[i] = {0};




    PlayerInit (
        &entities_array[0], 
        0,
        player_sprite, 
        player_up_sprite, 
        player_down_sprite, 
        player_left_sprite, 
        player_right_sprite, 
        {7,4}
    );
    player = entities_array[0].id;
    entity_id_map.SetID(entities_array[0].position.x, entities_array[0].position.y, entities_array[0].entity_layer, 0);

    main_camera.position.x  = float(tilemap.width/2);
    main_camera.position.y  = float(tilemap.height/2);
    main_camera.look_target = {7.0f, 4.0f, 0.0f}; 
}

void Start(GameState *gs, KeyboardState *ks) {
    char level_name_default[] = "untitled";
    strncpy(level_name, level_name_default, sizeof(level_name) - 1);
    level_name[sizeof(level_name) - 1] = '\0'; // Null-terminate the string
}

void DrawTile(Tileset tileset, Vector2 world_position, int atlas_x, int atlas_y) {
    float uv_width    = float(1)/float(tileset.width_in_tiles);
    float uv_height   = float(1)/float(tileset.height_in_tiles);
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


#ifdef DEBUG_UI
    UI_WindowStart("Entity Spawn Select", {1280, 108}, {0,0});

    double fps = DeltaTimeToFps(dt);
    char fps_str[64];
    sprintf(fps_str, "FPS: %f", fps);
    DrawSimpleText(fps_str, {0.99f, 0.2f}, UI_Alignment::TOP_RIGHT);

    static int entity_array_offset = 1;
    static int entity_type_to_spawn = 0;


    DrawSimpleTextbox(
        "Level Name", 
        level_name, LEVEL_NAME_MAX, 
        {float(108)/float(1280) * 10.05, 0.4}, 
        {108, 0}
    );

    if (DrawSimpleButton(
            "Save Level",
            {float(108)/float(1280) * 10.05, 0.7},
            {float(108)/float(1280) * 0.8, 0.2},
            nullptr
        )
    ) {
        SaveLevelState(level_name, &tilemap, 2, entities_array, entity_array_offset);
    }
    if (DrawSimpleButton(
            "Load Level",
            {float(108)/float(1280) * 11.0, 0.7},
            {float(108)/float(1280) * 0.8, 0.2},
            nullptr
        )
    ) {
        Sprite sprites[] = {
            player_sprite,                  //0 
            player_up_sprite,               //1
            player_down_sprite,             //2
            player_left_sprite,             //3
            player_right_sprite,            //4
            push_block_sprite,              //5
            static_block_sprite,            //6
            emitter_sprite,                 //7
            emitter_nozzle_sprite,          //8
            emitter_indicator_sprite,       //9
            receiver_sprite,                //10
            receiver_nozzle_sprite,         //11
            receiver_indicator_sprite,      //12
            open_door_sprite,               //13
            closed_door_sprite,             //14
            endgoal_sprite                  //15
        };
        LevelStateInfo level_state_info = ReadLevelState(level_name, &tilemap, &entities_array, &entity_id_map, sprites);
        entity_array_offset = level_state_info.num_entities;
    }




    if ( DrawSimpleImageButton (
            "PushBlockButton", 
            push_block_sprite, 
            {float(108)/float(1280) * 0.2, 0.2}, 
            {float(108)/float(1280) * 0.6, 0.6}
        )
    ) {
        entity_type_to_spawn = 1;
    } 
    if ( DrawSimpleImageButton (
            "StaticBlockButton", 
            static_block_sprite, 
            {float(108)/float(1280) * 1.0, 0.2}, 
            {float(108)/float(1280) * 0.6, 0.6}
        )
    ) {
        entity_type_to_spawn = 2;
    } 
    if ( DrawSimpleImageButton (
            "EmitterBlockButton", 
            emitter_sprite, 
            {float(108)/float(1280) * 1.8, 0.2}, 
            {float(108)/float(1280) * 0.6, 0.6}
        )
    ) {
        entity_type_to_spawn = 3;
    } 
    if ( DrawSimpleImageButton (
            "RecieverButton", 
            receiver_sprite, 
            {float(108)/float(1280) * 2.6, 0.2}, 
            {float(108)/float(1280) * 0.6, 0.6}
        )
    ) {
        entity_type_to_spawn = 4;
    } 
    if ( DrawSimpleImageButton (
            "DoorButton", 
            closed_door_sprite, 
            {float(108)/float(1280) * 3.4, 0.2}, 
            {float(108)/float(1280) * 0.6, 0.6}
        )
    ) {
        entity_type_to_spawn = 5;
    } 
    if ( DrawSimpleImageButton (
            "EndgoalButton", 
            endgoal_sprite, 
            {float(108)/float(1280) * 4.2, 0.2}, 
            {float(108)/float(1280) * 0.6, 0.6}
        )
    ) {
        entity_type_to_spawn = 6;
    } 
     
    
    if (ks->state.MBR && !ks->prev_state.MBR) {

        int tile_mouse_x = int(GetMousePositionInWorldCoords().x + 0.5f);
        int tile_mouse_y = int(GetMousePositionInWorldCoords().y + 0.5f); 

        switch (entity_type_to_spawn)
        {
            case 1: {
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0) >= 0) break;
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1) >= 0) break;
                if (TestTileCollide(tilemap, {tile_mouse_x, tile_mouse_y})) break;

                int e = entity_array_offset;
                entity_array_offset++;
                PushblockInit(&entities_array[e], e, push_block_sprite, {tile_mouse_x, tile_mouse_y});
                int x = entities_array[e].position.x;
                int y = entities_array[e].position.y;
                entity_id_map.SetID(x, y, entities_array[e].entity_layer, entities_array[e].id);
            } break;
            case 2: {
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0) >= 0) break;
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1) >= 0) break;
                if (TestTileCollide(tilemap, {tile_mouse_x, tile_mouse_y})) break;

                int e = entity_array_offset;
                entity_array_offset++;

                StaticBlockInit(&entities_array[e], e, static_block_sprite, {tile_mouse_x, tile_mouse_y});
                int x = entities_array[e].position.x;
                int y = entities_array[e].position.y;
                entity_id_map.SetID(x, y, entities_array[e].entity_layer, entities_array[e].id);
            } break;
            case 3: {
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0) >= 0) break;
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1) >= 0) break;
                if (TestTileCollide(tilemap, {tile_mouse_x, tile_mouse_y})) break;

                int e = entity_array_offset;
                entity_array_offset++;

                EmitterInit(&entities_array[e], e, emitter_sprite, emitter_nozzle_sprite, emitter_indicator_sprite, {tile_mouse_x, tile_mouse_y}, {255, 255, 255, 255}, true, EntityComponentEmitter::DOWN); 
                int x = entities_array[e].position.x;
                int y = entities_array[e].position.y;
                entity_id_map.SetID(x, y, entities_array[e].entity_layer, entities_array[e].id);
            } break;
            case 4: {
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0) >= 0) break;
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1) >= 0) break;
                if (TestTileCollide(tilemap, {tile_mouse_x, tile_mouse_y})) break;

                int e = entity_array_offset;
                entity_array_offset++;

                ReceiverInit(&entities_array[e], e, receiver_sprite, receiver_nozzle_sprite, receiver_indicator_sprite, {tile_mouse_x, tile_mouse_y}, {255, 255, 255, 255}, true);
                int x = entities_array[e].position.x;
                int y = entities_array[e].position.y;
                entity_id_map.SetID(x, y, entities_array[e].entity_layer, entities_array[e].id);
            } break;
            case 5: {
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0) >= 0) break;
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1) >= 0) break;
                if (TestTileCollide(tilemap, {tile_mouse_x, tile_mouse_y})) break;

                int e = entity_array_offset;
                entity_array_offset++;

                DoorInit(&entities_array[e], e, open_door_sprite, open_door_sprite, closed_door_sprite, {tile_mouse_x, tile_mouse_y}); 
                int x = entities_array[e].position.x;
                int y = entities_array[e].position.y;
                entity_id_map.SetID(x, y, entities_array[e].entity_layer, entities_array[e].id);
            } break;
            case 6: {
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0) >= 0) break;
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1) >= 0) break;
                if (TestTileCollide(tilemap, {tile_mouse_x, tile_mouse_y})) break;

                int e = entity_array_offset;
                entity_array_offset++;

                EndgoalInit(&entities_array[e], e, endgoal_sprite, {tile_mouse_x, tile_mouse_y}); 
                int x = entities_array[e].position.x;
                int y = entities_array[e].position.y;
                entity_id_map.SetID(x, y, entities_array[e].entity_layer, entities_array[e].id);
            } break;
            default:
                break;
        }
    }

    // NOTE: Do state changes for entity (entity-type specific)
    if (ks->state.R && !ks->prev_state.R) {
        int tile_mouse_x = int(GetMousePositionInWorldCoords().x + 0.5f);
        int tile_mouse_y = int(GetMousePositionInWorldCoords().y + 0.5f); 

        int entity_id = entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1);
        if (entity_id >= 0) {
            if (entities_array[entity_id].emitter.active) {
                entities_array[entity_id].emitter.direction = (EntityComponentEmitter::DIRECTION_ENUM)((entities_array[entity_id].emitter.direction + 1) % 4);
            }
        } else {
            entity_id = entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0);
            if (entity_id >= 0) {
                if (entities_array[entity_id].door.active) {
                    entities_array[entity_id].door.require_all_to_unlock = !entities_array[entity_id].door.require_all_to_unlock;
                }
            }
        }

    }


    static int selected_receiver_id = -1; 
    // NOTE: When E is first pressed down, select the hovered receiver
    if (ks->state.E && !ks->prev_state.E) {


        int tile_mouse_x = int(GetMousePositionInWorldCoords().x + 0.5f);
        int tile_mouse_y = int(GetMousePositionInWorldCoords().y + 0.5f); 

        int entity_id = entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1);
    
        if (entity_id >= 0) {
            if (entities_array[entity_id].active && entities_array[entity_id].receiver.active) {
                selected_receiver_id = entity_id;
            }
        }
    }
    // NOTE: When E is released, attach selected recevier the hovered door
    if (!ks->state.E && ks->prev_state.E) {

        int tile_mouse_x = int(GetMousePositionInWorldCoords().x + 0.5f);
        int tile_mouse_y = int(GetMousePositionInWorldCoords().y + 0.5f); 

        int entity_id = entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0);
        if (entity_id >= 0) { 
            if (entities_array[entity_id].active && entities_array[entity_id].door.active) {
                if (selected_receiver_id >= 0) {
                    Entity* door = &entities_array[entity_id];
                    if (door->door.num_connected_receivers < MAX_CONNECTED_RECIEVERS) {
                    printf("Released\n");
                        door->door.connected_receiver_ids[door->door.num_connected_receivers] = selected_receiver_id;
                        door->door.num_connected_receivers += 1;
                    }
                }
            }
        }
        selected_receiver_id = -1;
    }

    // NOTE: Print debug info of entity
    if (ks->state.SPACE && !ks->prev_state.SPACE) {
        int tile_mouse_x = int(GetMousePositionInWorldCoords().x + 0.5f);
        int tile_mouse_y = int(GetMousePositionInWorldCoords().y + 0.5f); 

        int entity_id = entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0);
        if (entity_id >=0) {
            Entity entity = entities_array[entity_id];
            PrintEntity(entity);
        } 
        entity_id = entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1);
        if (entity_id >=0) {
            Entity entity = entities_array[entity_id];
            PrintEntity(entity);
        }
        
    }
    // NOTE: Do toggling movability:
    // TODO:

    // NOTE: Do saving:
    // TODO:

    UI_WindowEnd();
#endif

    const int BLOCK_PUSH_LIMIT = MAX_ENTITIES;

    
    if (!entities_array[player].movable.moving) {
        entities_array[player].player.direction = EntityComponentPlayer::DIRECTION_ENUM::NEUTRAL; 
    }
    
    if (ks->state.W && !entities_array[player].movable.moving) { 
        EntityMove(player, {0, 1}, tilemap, entity_id_map, entities_array, BLOCK_PUSH_LIMIT);
        entities_array[player].player.direction = EntityComponentPlayer::DIRECTION_ENUM::UP; 
    }
    if (ks->state.S && !entities_array[player].movable.moving) {
        EntityMove(player, {0,-1}, tilemap, entity_id_map, entities_array, BLOCK_PUSH_LIMIT);
        entities_array[player].player.direction = EntityComponentPlayer::DIRECTION_ENUM::DOWN; 
    }
    if (ks->state.A && !entities_array[player].movable.moving) {
        EntityMove(player, {-1,0}, tilemap, entity_id_map, entities_array, BLOCK_PUSH_LIMIT);
        entities_array[player].player.direction = EntityComponentPlayer::DIRECTION_ENUM::LEFT; 
    }
    if (ks->state.D && !entities_array[player].movable.moving) {
        EntityMove(player, {1, 0}, tilemap, entity_id_map, entities_array, BLOCK_PUSH_LIMIT);
        entities_array[player].player.direction = EntityComponentPlayer::DIRECTION_ENUM::RIGHT; 
    }


    


    for(int x = 0; x < tilemap.width; ++x) {
        for (int y = 0; y < tilemap.height; ++y) {
            DrawTile(tileset, {float(x), float(y)}, tilemap.map[(8-y)*15 + x]);
        }
    }

    for (int z = 0; z < entity_id_map.depth; z++) {
        for (int y = 0; y < entity_id_map.height; y++) {
            for (int x = 0; x < entity_id_map.width; x++) {
                int id = entity_id_map.GetID(x, y, z);
                if (id < 0) continue;
                EntityRender(id, entities_array, shaders);
            }
        }
    }

    EmissionRender(emission_map, emission_sprite, shaders);
    

    // NOTE: This is done after rendering so that rendering that depends on the movable.moving flag doesnt flicker every block 
    //printf("############################\n");
    for (int id = 0; id < entity_array_offset; ++id) {
        EntityUpdateEmit(id, tilemap, entity_id_map, emission_map, entities_array);
    }
    for (int id = 0; id < entity_array_offset; ++id) {
        EntityUpdateMover(id, entities_array, dt);
    }
    for (int id = 0; id < entity_array_offset; ++id) {
        EntityUpdateReceiver(id, entities_array);
    }
    for (int id = 0; id < entity_array_offset; ++id) {
        EntityUpdateDoor(id, entities_array, entity_id_map);
    }
    

}

void UserFree()
{

    free(tilemap.map);
    tilemap.map = nullptr;

    free(entity_id_map.map);
    entity_id_map.map = nullptr;

    free(emission_map.map);
    emission_map.map = nullptr;


    FreeSprite(tileset.atlas);
    entities_array[player].active = false;




    FreeGPUBuffers(gpu_buffers);
    FreeShaders(shaders);
}
