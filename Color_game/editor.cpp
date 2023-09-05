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
    #ifdef DEBUG_UI
    *h = 720+108; 
    #endif
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
Sprite open_door_vertical_sprite;
Sprite closed_door_vertical_sprite;
Sprite open_door_horizontal_sprite;
Sprite closed_door_horizontal_sprite;
Sprite endgoal_sprite;
Sprite button_up_sprite;
Sprite button_down_sprite;




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
char* level_name;
#endif

void Awake(GameMemory *gm)
{
    #ifdef DEBUG_UI
    level_name = (char*)malloc(sizeof(char) * LEVEL_NAME_MAX);
    #endif
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
        5,13,13,13,13,13,13,13,13,13,13,13,13,13, 6,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
        10,3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,11
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
    open_door_vertical_sprite   = LoadSprite("assets/door_open.png", shaders, gpu_buffers);
    closed_door_vertical_sprite = LoadSprite("assets/door_closed.png", shaders, gpu_buffers);
    open_door_horizontal_sprite = LoadSprite("assets/door_open_horiz.png", shaders, gpu_buffers);
    closed_door_horizontal_sprite = LoadSprite("assets/door_closed_horiz.png", shaders, gpu_buffers);
    endgoal_sprite              = LoadSprite("assets/endgoal.png", shaders, gpu_buffers);
    button_up_sprite            = LoadSprite("assets/button_up.png", shaders, gpu_buffers);
    button_down_sprite          = LoadSprite("assets/button_down.png", shaders, gpu_buffers);


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
    #ifdef DEBUG_UI
    main_camera.position.y  = float(tilemap.height/2) + 0.5f;
    main_camera.look_target = {7.0f, 4.5f, 0.0f}; 
    #endif
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


// NOTE: For empty level creating
void Update(GameState *gs, KeyboardState *ks, double dt) {


#ifdef DEBUG_UI
    UI_WindowStart("Entity Spawn Select", {1280, 108}, {0,0});

    double fps = DeltaTimeToFps(dt);
    char fps_str[64];
    sprintf(fps_str, "FPS: %d", int(fps));
    DrawSimpleText(fps_str, {0.99f, 0.2f}, UI_Alignment::TOP_RIGHT);

    static int entity_array_offset = 1;
    static int entity_type_to_spawn = 0;

    { // NOTE: Creating empty level.
        static Vector2Int tilemap_size = {15, 9};
        if (ks->state.Z && !ks->prev_state.Z) {
            if (tilemap_size.x > 3) tilemap_size.x--;
        } 
        if (ks->state.X && !ks->prev_state.X) {
            tilemap_size.x++;
        } 
        if (ks->state.C && !ks->prev_state.C) {
            if (tilemap_size.y > 3) tilemap_size.y--;
        } 
        if (ks->state.V && !ks->prev_state.V) {
            tilemap_size.y++;
        }
        char width_str[24];
        sprintf(width_str, "Width: %d", tilemap_size.x);
        char height_str[24];
        sprintf(height_str, "Height: %d", tilemap_size.y);
        DrawSimpleText(height_str, {float(108)/float(1280) * 9.2f, 0.3f}, UI_Alignment::TOP_LEFT);
        DrawSimpleText(width_str, {float(108)/float(1280) * 9.2f, 0.5f}, UI_Alignment::TOP_LEFT);
        if (DrawSimpleButton (
                "Emtpy Level",
                {float(108)/float(1280) * 9.1, 0.7},
                {float(108)/float(1280) * 0.8, 0.2},
                nullptr
            )
        ) {
            entity_array_offset = 1;
            tilemap.width = tilemap_size.x;
            tilemap.height = tilemap_size.y;
            tilemap.map = (int*)realloc(tilemap.map, sizeof(int) * tilemap.height * tilemap.width);

            for (int y = 0; y < tilemap.height; ++y) {
                for (int x = 0; x < tilemap.width; ++x) {
                    if      (y == 0 && x == 0)                              tilemap.map[y * tilemap.width + x] = 5;
                    else if (y == 0 && x == tilemap.width-1)                tilemap.map[y * tilemap.width + x] = 6;
                    else if (x == 0 && y == tilemap.height-1)               tilemap.map[y * tilemap.width + x] = 10;
                    else if (x == tilemap.width-1 && y == tilemap.height-1) tilemap.map[y * tilemap.width + x] = 11;
                    
                    else if (y == 0) tilemap.map[y * tilemap.width + x] = 13;
                    else if (x == 0) tilemap.map[y * tilemap.width + x] = 9;
                    else if (y == tilemap.height-1) tilemap.map[y * tilemap.width + x] = 3;
                    else if (x == tilemap.width-1) tilemap.map[y * tilemap.width + x] = 7;
                    
                    else  tilemap.map[y * tilemap.width + x] = 1;
                }
            }

            entity_id_map.width = tilemap_size.x;
            entity_id_map.height = tilemap_size.y;
            entity_id_map.map = (int*)realloc(entity_id_map.map, sizeof(int) * entity_id_map.width * entity_id_map.height * entity_id_map.depth);

            for (int i = 0; i < entity_id_map.width * entity_id_map.height * entity_id_map.depth; ++i) {
                entity_id_map.map[i] = -1;
            }

            free(entities_array);
            entities_array = (Entity*)calloc(sizeof(Entity), MAX_ENTITIES);


            PlayerInit (
                &entities_array[0], 
                0,
                player_sprite, 
                player_up_sprite, 
                player_down_sprite, 
                player_left_sprite, 
                player_right_sprite, 
                {tilemap.width/2,tilemap.height/2}
            );
            Entity player = entities_array[0];
            entity_id_map.SetID(player.position.x, player.position.y, player.entity_layer, player.id);
             
            emission_map.width  = tilemap.width;
            emission_map.height = tilemap.height;
            free(emission_map.map);
            emission_map.map = (EmissionTile*)calloc(emission_map.width * emission_map.height, sizeof(EmissionTile));

        }
    }

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
            open_door_vertical_sprite,      //13
            closed_door_vertical_sprite,    //14
            open_door_horizontal_sprite,    //15
            closed_door_horizontal_sprite,  //16
            endgoal_sprite,                 //17
            button_up_sprite,               //18
            button_down_sprite,             //19
        };
        LevelStateInfo level_state_info = ReadLevelState(level_name, &tilemap, &entities_array, &entity_id_map, sprites);
        entity_array_offset = level_state_info.num_entities;
        emission_map.width  = tilemap.width;
        emission_map.height = tilemap.height;
        free(emission_map.map);
        emission_map.map = (EmissionTile*)calloc(emission_map.width * emission_map.height, sizeof(EmissionTile));
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
            closed_door_vertical_sprite, 
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
    if ( DrawSimpleImageButton (
            "ButtonButton", 
            button_up_sprite, 
            {float(108)/float(1280) * 5.0, 0.2}, 
            {float(108)/float(1280) * 0.6, 0.6}
        )
    ) {
        entity_type_to_spawn = 7;
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

                DoorInit(&entities_array[e], e, open_door_vertical_sprite, closed_door_vertical_sprite, open_door_horizontal_sprite, closed_door_horizontal_sprite, {tile_mouse_x, tile_mouse_y}); 
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
            case 7: {
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0) >= 0) break;
                if (entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 1) >= 0) break;
                if (TestTileCollide(tilemap, {tile_mouse_x, tile_mouse_y})) break;

                int e = entity_array_offset;
                entity_array_offset++;

                ButtonInit(&entities_array[e], e, button_up_sprite, button_down_sprite, {tile_mouse_x, tile_mouse_y}); 
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
                    entities_array[entity_id].door.open_by_default = !entities_array[entity_id].door.open_by_default;
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
        int bottom_entity_id = entity_id_map.GetID(tile_mouse_x, tile_mouse_y, 0);

        if (entity_id >= 0) {
            if (entities_array[entity_id].active && entities_array[entity_id].receiver.active) {
                selected_receiver_id = entity_id;
                printf("Selected activator id %d\n", entity_id);
            } 
        }
        if (bottom_entity_id >= 0) {
            if (entities_array[bottom_entity_id].active && entities_array[bottom_entity_id].button.active) {
                selected_receiver_id = bottom_entity_id;
                printf("Selected activator id %d\n", bottom_entity_id);
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
                    if (door->door.num_connected_activators < MAX_CONNECTED_ACTIVATORS) {
                    printf("Connected id %d to door %d\n", selected_receiver_id, entity_id);
                        door->door.connected_activators_ids[door->door.num_connected_activators] = selected_receiver_id;
                        door->door.num_connected_activators += 1;
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
            DrawTile(tileset, {float(x), float(y)}, tilemap.map[y*tilemap.width + x]);
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
        EntityUpdateButton(id, entities_array, entity_id_map);
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
