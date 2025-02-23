#define INLUDE_IMGUI
#include "../Engine/SDL_sky.cpp"
#include "../Engine/tilemap.h"
#include "../Engine/skymath.h"
#include "tilemap_manager.cpp"
#include "entity.cpp"

void Init(
    int* w, int* h, 
    float* width_in_world_space, 
    bool* fullscreen,
    fColor* clear_color
) {
    *w = 1280;
    *h = 720;
    *width_in_world_space = 18.0f;
    *fullscreen = false;
    *clear_color = {34.0f/255.0f, 32.0f/255.0f, 52.0f/255.0f, 1.0f};

}

GL_ID* shaders = nullptr;
GPUBufferIDs gpu_buffers = {nullptr};

Tileset tile_atlas = {0};
Tileset overlay_atlas = {0};
Tileset object_atlas = {0};

Tilemap tilemap = {0};

Sprite zombie_sprite = {0};
Sprite player_sprite = {0};

void Awake(GameMemory* gm) {
    shaders = ShaderInit("shader.vs", "shader.fs");
    gpu_buffers = InitGPUBuffers();
    ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
    

    main_camera.position.x = 7.5f;
    main_camera.position.y = 4.0f;

    main_camera.look_target = {main_camera.position.x, main_camera.position.y, 0.0f}; 


    tile_atlas.height_in_tiles = 4;
    tile_atlas.width_in_tiles = 6; 
    tile_atlas.atlas = LoadSprite("main_tiles.png", shaders, gpu_buffers);
    
    overlay_atlas.height_in_tiles = 7;
    overlay_atlas.width_in_tiles = 3; 
    overlay_atlas.atlas = LoadSprite("floor_tile_overlays.png", shaders, gpu_buffers);
    
    object_atlas.height_in_tiles = 5;
    object_atlas.width_in_tiles = 9; 
    object_atlas.atlas = LoadSprite("tile_objects.png", shaders, gpu_buffers);


    zombie_sprite = LoadSprite("zombie.png", shaders, gpu_buffers);
    player_sprite = LoadSprite("player.png", shaders, gpu_buffers);

}


EntityManager entity_manager = {0};
ArenaAllocator scene_arena = {0};

void Start(GameState* gs, KeyboardState* ks) {
    // NOTE: Init arena and entity manager:
    
    InitArena(&scene_arena, Kilobytes(4));
    MemsetArena(&scene_arena, 0);

    entity_manager.InitializePools(&scene_arena); 

    for (int i = 0; i < 10; ++i) {
        entity_manager.monsters.active[i] = true;
    }
    for (int i = 0; i < 10; ++i) {
        entity_manager.monsters.health[i].current_health = 5;
        entity_manager.monsters.health[i].max_health = 5;
    }
    for (int i = 0; i < 10; ++i) {
        entity_manager.monsters.movement[i].position = Vector2Int{i+1, 2};
        entity_manager.monsters.movement[i].tiles_per_turn = 3;
    }
    for (int i = 0; i < 10; ++i) {
        entity_manager.monsters.combat[i].armor_class = 8; 
        entity_manager.monsters.combat[i].damage_per_hit = 1;
        entity_manager.monsters.combat[i].cooldown_per_hit = 1; 
    }

    entity_manager.players.active[0] = true;
    entity_manager.players.health[0].current_health = 10;
    entity_manager.players.health[0].max_health = 10;
    entity_manager.players.movement[0].position = Vector2Int{3, 3};
    entity_manager.players.movement[0].tiles_per_turn = 5;
    entity_manager.players.combat[0].armor_class = 12; 
    entity_manager.players.combat[0].damage_per_hit = 3;
    entity_manager.players.combat[0].cooldown_per_hit = 1; 

    // NOTE: Init tilemap
    tilemap = InitTilemap(Vec2(16, 9));

    for (int y = 0; y < tilemap.height; ++y) {
        for (int x = 0; x < tilemap.width; ++x) {


            if ((x == 0 || x == tilemap.width-1) && y != 0) {
                SetTile(&tilemap, Vector2Int{x, y}, TileType::cave_wall_vert);
                SetOverlay(&tilemap, Vector2Int{x, y}, TileOverlay::void_overlay);
            } else if (y == 0 || y == tilemap.height-1) {
                SetTile(&tilemap, Vector2Int{x, y}, TileType::cave_wall_horiz);
                SetOverlay(&tilemap, Vector2Int{x, y}, TileOverlay::void_overlay);
            } else if (x % 2 == y % 2) {
                SetTile(&tilemap, Vector2Int{x, y}, TileType::ground_dark_cyan);
                SetOverlay(&tilemap, Vector2Int{x, y}, TileOverlay::blue_bricks_1);
            } else {
                SetTile(&tilemap, Vector2Int{x, y}, TileType::ground_dark_blue);
                SetOverlay(&tilemap, Vector2Int{x, y}, TileOverlay::void_overlay);

            }
            SetObject(&tilemap, Vector2Int{x, y}, TileObject::void_object);
        }
    }

}


void Update(GameState* gs, KeyboardState* ks, double dt) {

    RenderTilemap(tilemap, tile_atlas, overlay_atlas, object_atlas, shaders, gpu_buffers); 

    Transform transform = {0};
    transform.scale = {1.0f, 1.0f, 1.0f};
    for (int i = 0; i < 10; ++i) {
        // NOTE: Render all monsters:
        if (!entity_manager.monsters.active[i]) continue;
        Vector2Int pos = entity_manager.monsters.movement[i].position;

        transform.position = Vector3{float(pos.x), float(pos.y), 1.0f};
        DrawSprite(zombie_sprite, transform, main_camera);
        //printf("Rendering Zombie: %f, %f \n", transform.position.x, transform.position.y);
    }
    Vector2Int player_pos = entity_manager.players.movement[0].position;
    transform.position = Vector3{float(player_pos.x), float(player_pos.y), 1.0f};
    DrawSprite(player_sprite, transform, main_camera);
    // SECTION: Unit Selection and movement.
    static int selected_unit = 0;
    // NOTE: Get mouse pos
    Vector2 mouse_world_pos = GetMousePositionInWorldCoords();
    // NOTE: Get inworld tile position from mouse position
    int mouse_tile_x = int(round(mouse_world_pos.x));
    int mouse_tile_y = int(round(mouse_world_pos.y));
    if (ks->state.MBL && !ks->prev_state.MBL) {
        selected_unit=Get
    }
}

void UserFree() {
    FreeTilemap(&tilemap);
    FreeArena(&scene_arena); 
}
        