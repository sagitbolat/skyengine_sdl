#define INCLUDE_IMGUI
#define DEBUG_MODE
#define NO_SPLASH_SCREEN
#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"

#define MAX_ENTITIES 91 
#include "tilemap.h"
#include "entity.h"
#include "level_loader.h"


#ifdef PROFILING
#include <chrono>
#endif

// SECTION: Initialization of stuff...
void Init(int *w, int *h, float *w_in_world_space, bool *fullscreen, fColor *clear_color)
{
    *w = 1280;
    *h = 720;
    *w_in_world_space = 14.0f;
    *fullscreen = false;
    //*clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
    *clear_color = {43.0f/255, 43.0f/255, 39.0f/255, 1.0f};
}


Tileset tileset = {0};
Tilemap tilemap = {0};


const int NUM_LEVELS = 30;
int curr_level_index = 0;
char level_names[][64] = { // NOTE: To calculate array length subtract the line number of the last string from the line number of this line
    "0",
    "1",
    "2",
    "3",   
    "4",      
    "5_w",
    "6_w",         
    "7_w",   
    "29",
    "10_w",         
    "11_w",          
    "12_w", 
    "13_w",
    "14_w",
    "15_w",          
    "16_w",
    "24_w", 
    "25_w", 
    "26_w",
    "17_w",          
    "18_w", 
    "20_w", 
    "27_w",
    "9_w",
    "19_w",
    "23_w", 
    "21_w", 
    "8_w",
    "28_w",
    "22_w2222"
};
float level_zoom[] = {
    14.0f, // 0
    14.0f,
    14.0f,
    14.0f,
    14.0f,
    14.0f, // 5
    14.0f,
    14.0f,
    14.0f,
    14.0f,
    14.0f, //10
    14.0f,
    14.0f,
    14.0f,
    14.0f,
    14.0f, //15
    14.0f,
    14.0f,
    14.0f,
    14.0f,
    14.0f, //20
    14.0f,
    14.0f,
    14.0f,
    14.0f,
    18.0f, //25
    14.0f,
    14.0f,
    14.0f,
    18.0f
};



Sprite WASD_controls_sprite;
Sprite reload_controls_sprite;
Sprite undo_controls_sprite;
Sprite wire_view_controls_sprite;

Sprite player_sprite; 
Sprite player_up_sprite; 
Sprite player_down_sprite; 
Sprite player_left_sprite; 
Sprite player_right_sprite; 
Sprite push_block_sprite;        
Sprite static_block_sprite;  
Sprite static_block_transparent;    
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
Sprite sprites [20];

EntityMap entity_id_map; // NOTE: acts as a lookup table from tilemap coordinate to an entity. a negative value indicates no entity at coordinate.
Entity* entities_array; // Array of entities in the game. Player is always first element.
Entity* prev_entities_array;
EmissionMap emission_map;
Sprite emission_sprite;

GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};
Transform tile_default_transform;

LevelStateInfo level_state_info;

void Awake(GameMemory *gm)
{
    // SECTION: renderer initializations
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});



    Sprite tileset_sprite   = LoadSprite("assets/tileset.png", shaders, gpu_buffers);
    tileset.atlas           = tileset_sprite;
    tileset.width_in_tiles  = 5;
    tileset.height_in_tiles = 8;



    tile_default_transform.position = {0.0f, 0.0f, -1.0f};
    tile_default_transform.rotation = {0.0f, 0.0f, 0.0f};
    tile_default_transform.scale    = {1.0f, 1.0f, 1.0f};


    WASD_controls_sprite        = LoadSprite("assets/WASD.png", shaders, gpu_buffers);
    reload_controls_sprite      = LoadSprite("assets/restart_control.png", shaders, gpu_buffers);
    undo_controls_sprite        = LoadSprite("assets/undo_control.png", shaders, gpu_buffers);
    wire_view_controls_sprite   = LoadSprite("assets/wire_view_control.png", shaders, gpu_buffers);

    player_sprite               = LoadSprite("assets/player.png", shaders, gpu_buffers);
    player_up_sprite            = LoadSprite("assets/player_up.png", shaders, gpu_buffers);
    player_down_sprite          = LoadSprite("assets/player_down.png", shaders, gpu_buffers);
    player_left_sprite          = LoadSprite("assets/player_left.png", shaders, gpu_buffers);
    player_right_sprite         = LoadSprite("assets/player_right.png", shaders, gpu_buffers);
    push_block_sprite           = LoadSprite("assets/push_block.png", shaders, gpu_buffers);
    static_block_sprite         = LoadSprite("assets/static_block.png", shaders, gpu_buffers);
    static_block_transparent    = LoadSprite("assets/static_block_transparent.png", shaders, gpu_buffers);
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


    sprites[0]  = player_sprite;               
    sprites[1]  = player_up_sprite;           
    sprites[2]  = player_down_sprite;         
    sprites[3]  = player_left_sprite;         
    sprites[4]  = player_right_sprite;        
    sprites[5]  = push_block_sprite;          
    sprites[6]  = static_block_sprite;        
    sprites[7]  = emitter_sprite;             
    sprites[8]  = emitter_nozzle_sprite;      
    sprites[9]  = emitter_indicator_sprite;   
    sprites[10] = receiver_sprite;             
    sprites[11] = receiver_nozzle_sprite;      
    sprites[12] = receiver_indicator_sprite;   
    sprites[13] = open_door_vertical_sprite;   
    sprites[14] = closed_door_vertical_sprite; 
    sprites[15] = open_door_horizontal_sprite; 
    sprites[16] = closed_door_horizontal_sprite;
    sprites[17] = endgoal_sprite;              
    sprites[18] = button_up_sprite;            
    sprites[19] = button_down_sprite;          
   

    // TODO: Load the saved data

    level_state_info = ReadLevelState(level_names[curr_level_index], &tilemap, &entities_array, &entity_id_map, sprites);
    prev_entities_array = (Entity*)calloc(level_state_info.num_entities, sizeof(Entity));
    for (int i = 0; i < level_state_info.num_entities; ++i) {
        prev_entities_array[i] = entities_array[i];
    }

    ++curr_level_index;

    emission_map.width  = tilemap.width;
    emission_map.height = tilemap.height;
    emission_map.map    = (EmissionTile*)calloc(emission_map.width * emission_map.height, sizeof(EmissionTile));
    for (int i = 0; i < emission_map.width * emission_map.height; ++i) emission_map.map[i] = {0};



    main_camera.position.x  = float(tilemap.width/2);
    main_camera.position.y  = float(tilemap.height/2);
    main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f}; 
}

void Start(GameState *gs, KeyboardState *ks) {

}




bool level_transitioning = false;
bool fast_reload = false;
bool restarting_level = false; // NOTE: Only used to differentiating between transitions from one level to another and actual restarting.
void Update(GameState *gs, KeyboardState *ks, double dt) {


    if (level_transitioning) {
    
        static const float TRANSITION_DURATION = 0.5f * 1000.0f;
        static const float TRANSITION_ZOOM = 3.0f;
        static bool fading_in = true;
        static float transition_time = 0.0f;
        
        transition_time += dt;

        if (fading_in) {
            if (transition_time < TRANSITION_DURATION) {
                main_camera.width = level_zoom[curr_level_index-1] + (transition_time / (TRANSITION_DURATION)) * TRANSITION_ZOOM;
                if (restarting_level) {
                    main_camera.width = level_zoom[curr_level_index] + (transition_time / (TRANSITION_DURATION)) * TRANSITION_ZOOM;
                }
                main_camera.height = (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH * main_camera.width;
                float complement = 1.0f - (transition_time / (TRANSITION_DURATION/2));
                ShaderSetVector(shaders, "i_color_multiplier", Vec4(fColor{1.0f, 1.0f, 1.0f, complement}));
            }
            else {
                level_state_info = ReadLevelState(level_names[curr_level_index], &tilemap, &entities_array, &entity_id_map, sprites);
                free(prev_entities_array); 
                prev_entities_array = (Entity*)calloc(level_state_info.num_entities, sizeof(Entity));
                for (int i = 0; i < level_state_info.num_entities; ++i) {
                    prev_entities_array[i] = entities_array[i];
                }
                ++curr_level_index;
                restarting_level = false; 
                
                emission_map.width  = tilemap.width;
                emission_map.height = tilemap.height;
                free(emission_map.map);
                emission_map.map = (EmissionTile*)calloc(emission_map.width * emission_map.height, sizeof(EmissionTile));
                
                ShaderSetVector(shaders, "i_color_multiplier", Vec4(fColor{1.0f, 1.0f, 1.0f, 1.0f}));
                main_camera.width = level_zoom[curr_level_index-1];
                main_camera.height = (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH * main_camera.width;
                main_camera.position.x  = float(tilemap.width/2);
                main_camera.position.y  = float(tilemap.height/2);
                main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f}; 
                transition_time = 0.0f;
                fading_in = false;
                return;
            }
        }
        else {
            if (transition_time < TRANSITION_DURATION) {
                float float_val = (transition_time / (TRANSITION_DURATION));
                main_camera.width = (level_zoom[curr_level_index-1] + TRANSITION_ZOOM) - (float_val * TRANSITION_ZOOM);
                main_camera.height = (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH * main_camera.width;
                ShaderSetVector(shaders, "i_color_multiplier", Vec4(fColor{1.0f, 1.0f, 1.0f, float_val}));
            } else {
                fading_in = true;
                level_transitioning = false;
                transition_time = 0.0f;
            }
        }
    }

    const int BLOCK_PUSH_LIMIT = MAX_ENTITIES;

#ifdef PROFILING
    auto start = std::chrono::high_resolution_clock::now();
#endif

    if (!entities_array[0].movable.moving) {
        entities_array[0].player.direction = EntityComponentPlayer::DIRECTION_ENUM::NEUTRAL; 
    }
    
    if ((ks->state.W || ks->state.ARROWUP) && !entities_array[0].movable.moving) {
        memcpy(prev_entities_array, entities_array, level_state_info.num_entities * sizeof(entities_array[0]));
        EntityMove(0, {0, 1}, tilemap, entity_id_map, entities_array, BLOCK_PUSH_LIMIT);
        entities_array[0].player.direction = EntityComponentPlayer::DIRECTION_ENUM::UP; 
    }
    if ((ks->state.S || ks->state.ARROWDOWN) && !entities_array[0].movable.moving) {
        memcpy(prev_entities_array, entities_array, level_state_info.num_entities * sizeof(entities_array[0]));
        EntityMove(0, {0,-1}, tilemap, entity_id_map, entities_array, BLOCK_PUSH_LIMIT);
        entities_array[0].player.direction = EntityComponentPlayer::DIRECTION_ENUM::DOWN; 
    }
    if ((ks->state.A || ks->state.ARROWLEFT) && !entities_array[0].movable.moving) {
        memcpy(prev_entities_array, entities_array, level_state_info.num_entities * sizeof(entities_array[0]));
        EntityMove(0, {-1,0}, tilemap, entity_id_map, entities_array, BLOCK_PUSH_LIMIT);
        entities_array[0].player.direction = EntityComponentPlayer::DIRECTION_ENUM::LEFT; 
    }
    if ((ks->state.D || ks->state.ARROWRIGHT) && !entities_array[0].movable.moving) {
        memcpy(prev_entities_array, entities_array, level_state_info.num_entities * sizeof(entities_array[0]));
        EntityMove(0, {1, 0}, tilemap, entity_id_map, entities_array, BLOCK_PUSH_LIMIT);
        entities_array[0].player.direction = EntityComponentPlayer::DIRECTION_ENUM::RIGHT; 
    }
    
    
    static bool showing_wires = false;
    if (ks->state.F && !ks->prev_state.F) {
        showing_wires = !showing_wires;
    }


    { // NOTE: if player standing on the endgoal, begine transition 
        Vector2Int player_pos = entities_array[0].position;
        int entity_id_at_player_pos = entity_id_map.GetID(player_pos.x, player_pos.y, 0);
        if (entity_id_at_player_pos >= 0) {
            Entity entity = entities_array[entity_id_at_player_pos];
            if (entity.active && entity.endgoal.active && !entities_array[0].movable.moving && curr_level_index < NUM_LEVELS) {
                // NOTE: Load Next Level
                showing_wires = false;
                level_transitioning = true;
                
            } else if (entity.active && entity.endgoal.active && !entities_array[0].movable.moving && curr_level_index >= NUM_LEVELS) {

                // TODO: This is the end of the game. Trigger the ending and roll credits.
                showing_wires = false;
                level_transitioning = true;
                curr_level_index = 0;
            }
        }
    }


    // NOTE: undo last move
    if (ks->state.U && !ks->prev_state.U && !level_transitioning) {
        memcpy(entities_array, prev_entities_array, level_state_info.num_entities * sizeof(entities_array[0]));
        entity_id_map.map       = (int*)realloc(entity_id_map.map, sizeof(int) * entity_id_map.height * entity_id_map.width * entity_id_map.depth);
        for (int i = 0; i < entity_id_map.height * entity_id_map.width * entity_id_map.depth; ++i) entity_id_map.map[i] = -1;
        for (int i = 0; i < level_state_info.num_entities; ++i) {
            Entity entity = entities_array[i];
            entity_id_map.SetID(entity.position.x, entity.position.y, entity.entity_layer, entity.id);
        }
    }

    // NOTE: Restarting level
    if (ks->state.R && !ks->prev_state.R && !level_transitioning) {
        if (fast_reload) {
            printf("Reloading level %s (index %d)\n", level_names[curr_level_index-1], curr_level_index-1);
            level_state_info = ReadLevelState(level_names[curr_level_index-1], &tilemap, &entities_array, &entity_id_map, sprites);
        } else {
            showing_wires = false;
            level_transitioning = true;
            curr_level_index--;
            restarting_level = true;
            printf("Reloading level %s (index %d)\n", level_names[curr_level_index], curr_level_index);
            return;
        }
    }

    // NOTE: Skipping through levels:
#ifdef DEBUG_MODE
    if (ks->state.Q && !ks->prev_state.Q) {
        --curr_level_index;
        if (curr_level_index <= 0) curr_level_index = NUM_LEVELS;
        level_state_info = ReadLevelState(level_names[curr_level_index-1], &tilemap, &entities_array, &entity_id_map, sprites);
        free(prev_entities_array); 
        prev_entities_array = (Entity*)calloc(level_state_info.num_entities, sizeof(Entity));
        memcpy(prev_entities_array, entities_array, level_state_info.num_entities * sizeof(entities_array[0]));
        printf("Restarting level: %d\n", curr_level_index-1);

        emission_map.width  = tilemap.width;
        emission_map.height = tilemap.height;
        free(emission_map.map);
        emission_map.map = (EmissionTile*)calloc(emission_map.width * emission_map.height, sizeof(EmissionTile));
        
        main_camera.position.x  = float(tilemap.width/2);
        main_camera.position.y  = float(tilemap.height/2);
        main_camera.width = level_zoom[curr_level_index-1];
        main_camera.height = (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH * main_camera.width;
        main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f}; 
    }
    if (ks->state.E && !ks->prev_state.E) {
        ++curr_level_index;
        if (curr_level_index > NUM_LEVELS) curr_level_index = 1;
        level_state_info = ReadLevelState(level_names[curr_level_index-1], &tilemap, &entities_array, &entity_id_map, sprites);
        free(prev_entities_array); 
        prev_entities_array = (Entity*)calloc(level_state_info.num_entities, sizeof(Entity));
        memcpy(prev_entities_array, entities_array, level_state_info.num_entities * sizeof(entities_array[0]));
        printf("Restarting level: %d\n", curr_level_index-1);
        
        emission_map.width  = tilemap.width;
        emission_map.height = tilemap.height;
        free(emission_map.map);
        emission_map.map = (EmissionTile*)calloc(emission_map.width * emission_map.height, sizeof(EmissionTile));
        
        main_camera.position.x  = float(tilemap.width/2);
        main_camera.position.y  = float(tilemap.height/2);
        main_camera.width = level_zoom[curr_level_index-1];
        main_camera.height = (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH * main_camera.width;
        main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f}; 
    }
    if (ks->state.SPACE && !ks->prev_state.SPACE) {
        int num_entities = level_state_info.num_entities;
        for (int i = 0; i < num_entities; ++i) {
            printf("%d ", entities_array[i].id); 
        }
        printf("\n");
    }
#endif



#ifdef PROFILING
    auto move_end = std::chrono::high_resolution_clock::now();
#endif

    if (!level_transitioning) {
        if (curr_level_index-1 == 0) {
            Transform t = {0}; 
            t.position.x = float(tilemap.width/2);
            t.position.y = main_camera.position.y + 2.0f;
            t.scale.x = 8.0f;
            t.scale.y = 2.0f;
            t.scale.z = 1.0f;
            DrawSprite(WASD_controls_sprite, t, main_camera);
        } else if (curr_level_index-1 == 1) {
            Transform t = {0}; 
            t.position.x = float(tilemap.width/2);
            t.position.y = main_camera.position.y + 2.5;
            t.scale.x = 4.0f;
            t.scale.y = 1.0f;
            t.scale.z = 1.0f;
            DrawSprite(reload_controls_sprite, t, main_camera);
        } else if (curr_level_index-1 == 2) {
            Transform t = {0}; 
            t.position.x = float(tilemap.width/2);
            t.position.y = main_camera.position.y + 2.5;
            t.scale.x = 5.0f;
            t.scale.y = 1.0f;
            t.scale.z = 1.0f;
            DrawSprite(undo_controls_sprite, t, main_camera);
        } else if (curr_level_index-1 == 3) {
            Transform t = {0}; 
            t.position.x = float(tilemap.width/2);
            t.position.y = main_camera.position.y + 2.5;
            t.scale.x = 7.0f;
            t.scale.y = 1.0f;
            t.scale.z = 1.0f;
            DrawSprite(wire_view_controls_sprite, t, main_camera);
        }
    }   



    { // NOTE: Tilemap Rendering Code
        float uv_width    = float(1)/float(tileset.width_in_tiles);
        float uv_height   = float(1)/float(tileset.height_in_tiles);
        ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
        ShaderSetVector(shaders, "top_right_uv", Vector2{uv_width, uv_height});
        for(int x = 0; x < tilemap.width; ++x) {
            for (int y = 0; y < tilemap.height; ++y) {

                int atlas_index = tilemap.map[y * tilemap.width + x];             
                
                if (!showing_wires && atlas_index > 14) {
                    atlas_index = 1;
                }

                if (atlas_index < 0) continue;

                int atlas_x = atlas_index % tileset.width_in_tiles;
                int atlas_y = atlas_index / tileset.width_in_tiles;

                float uv_x_offset = atlas_x * uv_width;
                float uv_y_offset = atlas_y * uv_height;
                ShaderSetVector(shaders, "uv_offset", Vector2{uv_x_offset, uv_y_offset});
                tile_default_transform.position = Vector3{float(x), float(y), tile_default_transform.position.z};
                DrawSprite(tileset.atlas, tile_default_transform, main_camera);
            }
        }
        
        ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
        ShaderSetVector(shaders, "top_right_uv", Vector2{1.0f, 1.0f});
        ShaderSetVector(shaders, "uv_offset", Vector2{0.0f, 0.0f});
    }

#ifdef PROFILING
    auto tile_render_end = std::chrono::high_resolution_clock::now();
#endif


    for (int z = 0; z < entity_id_map.depth; z++) {
        for (int y = 0; y < entity_id_map.height; y++) {
            for (int x = 0; x < entity_id_map.width; x++) {
                int id = entity_id_map.GetID(x, y, z);
                if (id < 0) continue;
                if (showing_wires) {
                    Entity* entity = &entities_array[id];
                    //if (entity->entity_type == Entity::ENTITY_TYPE_ENUM::STATIC_BLOCK) {
                    //    DrawSprite(static_block_transparent, entity->transform, main_camera);
                    //    continue;
                    //}

                    if (
                        entity->entity_type == Entity::ENTITY_TYPE_ENUM::PUSH_BLOCK ||
                        entity->entity_type == Entity::ENTITY_TYPE_ENUM::STATIC_BLOCK || 
                        entity->entity_type == Entity::ENTITY_TYPE_ENUM::ENDGOAL ||
                        entity->entity_type == Entity::ENTITY_TYPE_ENUM::EMITTER 
                    ) {
                        continue;
                    }
                }
                EntityRender(id, entities_array, shaders, level_transitioning);
            }
        }
    }


#ifdef PROFILING
    auto entity_render_end = std::chrono::high_resolution_clock::now();
#endif

    if (!showing_wires)
        EmissionRender(emission_map, emission_sprite, shaders, level_transitioning);
    

#ifdef PROFILING
    auto emission_render_end = std::chrono::high_resolution_clock::now();
#endif
    
    
    // NOTE: This is done after rendering so that rendering that depends on the movable.moving flag doesnt flicker every block 
    //printf("############################\n");
    for (int id = 0; id < level_state_info.num_entities; ++id) {
        EntityUpdateEmit(id, tilemap, entity_id_map, emission_map, entities_array);
    }
    for (int id = 0; id < level_state_info.num_entities; ++id) {
        EntityUpdateMover(id, entities_array, dt);
    }
    for (int id = 0; id < level_state_info.num_entities; ++id) {
        EntityUpdateReceiver(id, entities_array);
        EntityUpdateButton(id, entities_array, entity_id_map);
    }
    for (int id = 0; id < level_state_info.num_entities; ++id) {
        EntityUpdateDoor(id, entities_array, entity_id_map);
    }
    
#ifdef PROFILING
    auto entity_update_end = std::chrono::high_resolution_clock::now();

    printf("%lld\n", std::chrono::duration_cast<std::chrono::microseconds>(tile_render_end - move_end).count());
    //if(std::chrono::duration_cast<std::chrono::microseconds>(entity_update_end - start).count() > 7000) {
    //printf("Player Move Took:           %lld microseconds\n", std::chrono::duration_cast<std::chrono::microseconds>(move_end - start).count());
    //printf("Tile Rendering Took:        %lld microseconds\n", std::chrono::duration_cast<std::chrono::microseconds>(tile_render_end - move_end).count());
    //printf("Entity Rendering Took:      %lld microseconds\n", std::chrono::duration_cast<std::chrono::microseconds>(entity_render_end - tile_render_end).count());
    //printf("Emission Rendering Took:    %lld microseconds\n", std::chrono::duration_cast<std::chrono::microseconds>(emission_render_end - entity_render_end).count());
    //printf("Enitty Updating Took:       %lld microseconds\n", std::chrono::duration_cast<std::chrono::microseconds>(entity_update_end - emission_render_end).count());
    //}
#endif
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
    entities_array[0].active = false;




    FreeGPUBuffers(gpu_buffers);
    FreeShaders(shaders);
}
