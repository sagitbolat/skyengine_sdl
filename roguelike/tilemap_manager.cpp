#pragma once
#include "../Engine/sky_structs.h"
#include <unordered_map>
#include <tuple>

enum TileType {
    void_tile,
    
    ground_dark_red,
    ground_dark_green,
    ground_dark_blue,
    ground_dark_orange,
    ground_dark_purple,
    ground_dark_cyan,
    
    crypt_wall_vert,
    crypt_wall_horiz,
    
    brick_wall_vert,
    brick_wall_horiz,
    
    cave_wall_vert,
    cave_wall_horiz,

    dungeon_wall_vert,
    dungeon_wall_horiz,
    dungeon_wall_tunnel,

    cobble_wall_vert,
    cobble_wall_horiz,

    granite_wall_vert,
    granite_wall_horiz
};

enum TileOverlay {
    void_overlay,

    purple_bricks_1,
    purple_bricks_2,
    purple_bricks_3,

    bones_1,
    bones_2,
    bones_3,

    blue_bricks_1,
    blue_bricks_2,
    blue_bricks_3,

    seeds_1,
    seeds_2,
    seeds_3,

    grass_1,
    grass_2,
    grass_3,

    pebbles_1,
    pebbles_2,
    pebbles_3

};

enum TileObject {
    void_object,

    basic_door_1,
    basic_door_2,

    framed_door_closed_1,
    framed_door_open_1,
    
    framed_door_closed_2,
    framed_door_open_2,

    cell_door,

    stair_down,
    stair_up,

    chest_closed,
    chest_open,

    pot_closed,
    pot_open,

    barrel,

    sack,

    fire_wood_pile,

    boulder_1,
    boulder_2,

    flowers_1,
    flowers_2,
    flowers_3,
    flowers_4,
    flowers_5,
    flowers_6,
    flowers_7,
    flowers_8,
    flowers_9,
    flowers_10,
    flowers_11,
    flowers_12,
    flowers_13,
    flowers_14,
    flowers_15,
    flowers_16,

    mushrooms_1,
    mushrooms_2,

    bone_pile_1, 
    bone_pile_2,

    blood_pool_1,
    blood_pool_2
};

std::unordered_map<TileType, int, std::hash<int>> tile_type_to_atlas_index = {
    {TileType::void_tile, 19},
    
    {TileType::ground_dark_red, 0},
    {TileType::ground_dark_green, 1},
    {TileType::ground_dark_blue, 2},
    {TileType::ground_dark_orange, 3},
    {TileType::ground_dark_purple, 4},
    {TileType::ground_dark_cyan, 5},
 
    {TileType::crypt_wall_vert, 6},
    {TileType::crypt_wall_horiz, 7},

    {TileType::brick_wall_vert, 8},
    {TileType::brick_wall_horiz, 9},

    {TileType::cave_wall_vert, 10},
    {TileType::cave_wall_horiz, 11},
    
    {TileType::dungeon_wall_vert, 12},
    {TileType::dungeon_wall_horiz, 13},
    {TileType::dungeon_wall_tunnel, 14},

    {TileType::cobble_wall_vert, 15},
    {TileType::cobble_wall_horiz, 16},

    {TileType::granite_wall_vert, 17},
    {TileType::granite_wall_horiz, 18}

};

std::unordered_map<TileOverlay, int, std::hash<int>> tile_overlay_to_atlas_index = {
    {TileOverlay::void_overlay, 18},
    
    {TileOverlay::purple_bricks_1, 0},
    {TileOverlay::purple_bricks_2, 1},
    {TileOverlay::purple_bricks_3, 2},
    
    {TileOverlay::bones_1, 3},
    {TileOverlay::bones_2, 4},
    {TileOverlay::bones_3, 5},

    {TileOverlay::blue_bricks_1, 6},
    {TileOverlay::blue_bricks_2, 7},
    {TileOverlay::blue_bricks_3, 8},
    
    {TileOverlay::seeds_1, 9},
    {TileOverlay::seeds_2, 10},
    {TileOverlay::seeds_3, 11},
    
    {TileOverlay::grass_1, 12},
    {TileOverlay::grass_2, 13},
    {TileOverlay::grass_3, 14},

    {TileOverlay::pebbles_1, 15},
    {TileOverlay::pebbles_2, 16},
    {TileOverlay::pebbles_3, 17}

};

std::unordered_map<TileObject, int, std::hash<int>> tile_object_to_atlas_index = {
    {TileObject::void_object, 40},
    
    {TileObject::basic_door_1, 0},
    {TileObject::basic_door_2, 1},

    {TileObject::framed_door_closed_1, 2},
    {TileObject::framed_door_open_1, 3},

    {TileObject::framed_door_closed_2, 4},
    {TileObject::framed_door_open_2, 5},

    {TileObject::cell_door, 6},
    
    {TileObject::stair_down, 7},
    {TileObject::stair_up, 8},

    {TileObject::chest_closed, 9},
    {TileObject::chest_closed, 10},

    {TileObject::pot_closed, 11},
    {TileObject::pot_open, 12},

    {TileObject::barrel, 13},

    {TileObject::sack, 14},

    {TileObject::fire_wood_pile, 15},

    {TileObject::boulder_1, 16},
    {TileObject::boulder_2, 17},

    {TileObject::flowers_1, 18},
    {TileObject::flowers_2, 19},
    {TileObject::flowers_3, 20},
    {TileObject::flowers_4, 21},
    {TileObject::flowers_5, 22},
    {TileObject::flowers_6, 23},
    {TileObject::flowers_7, 24},
    {TileObject::flowers_8, 25},
    {TileObject::flowers_9, 26},
    {TileObject::flowers_10, 27},
    {TileObject::flowers_11, 28},
    {TileObject::flowers_12, 29},
    {TileObject::flowers_13, 30},
    {TileObject::flowers_14, 31},
    {TileObject::flowers_15, 32},
    {TileObject::flowers_16, 33},

    {TileObject::mushrooms_1, 34}, 
    {TileObject::mushrooms_2, 35},

    {TileObject::bone_pile_1, 36},
    {TileObject::bone_pile_2, 37},

    {TileObject::blood_pool_1, 38},
    {TileObject::blood_pool_2, 39}
};

struct Tilemap {
    TileType* state_map; //NOTE: Stores the actual tile types of the map
    TileOverlay* overlay_map; // NOTE: Stores the overlay features of the map
    TileObject* object_map; // NOTE: Stores the object type on this tile.
    int width;
    int height;
};

Tilemap InitTilemap(Vector2Int size) {
    Tilemap tilemap = {0};
    
    tilemap.state_map = (TileType*)malloc(sizeof(int) * size.x * size.y);
    tilemap.overlay_map = (TileOverlay*)malloc(sizeof(int) * size.x * size.y);
    tilemap.object_map = (TileObject*)malloc(sizeof(int) * size.x * size.y);
    
    
    tilemap.width = size.x;
    tilemap.height = size.y;

    return tilemap;
}

void SetTile(Tilemap* tilemap, Vector2Int position, TileType tile_type) { 
    if (position.x >= tilemap->width || position.y >= tilemap->height) return;
    (*tilemap).state_map[position.x + position.y * tilemap->width] = tile_type;
}
void SetOverlay(Tilemap* tilemap, Vector2Int position, TileOverlay overlay_type) { 
    if (position.x >= tilemap->width || position.y >= tilemap->height) return;
    (*tilemap).overlay_map[position.x + position.y * tilemap->width] = overlay_type;
}
void SetObject(Tilemap* tilemap, Vector2Int position, TileObject object_type) { 
    if (position.x >= tilemap->width || position.y >= tilemap->height) return;
    (*tilemap).object_map[position.x + position.y * tilemap->width] = object_type;
}

TileType GetTile(Tilemap tilemap, Vector2Int position) {
    if (position.x >= tilemap.width || position.y >= tilemap.height) return TileType::void_tile;
    return tilemap.state_map[position.x + position.y * tilemap.width];
}
TileOverlay GetOverlay(Tilemap tilemap, Vector2Int position) {
    if (position.x >= tilemap.width || position.y >= tilemap.height) return TileOverlay::purple_bricks_1;
    return tilemap.overlay_map[position.x + position.y * tilemap.width];
} 
TileObject GetObject(Tilemap tilemap, Vector2Int position) {
    if (position.x >= tilemap.width || position.y >= tilemap.height) return TileObject::basic_door_1;
    return tilemap.object_map[position.x + position.y * tilemap.width];
} 


void RenderTilemap(Tilemap tilemap, Tileset tile_atlas, Tileset overlay_atlas, Tileset object_atlas, GL_ID* shaders, GPUBufferIDs gpu_buffers) {
    for (int y = 0; y < tilemap.height; ++y) {
        for (int x = 0; x < tilemap.width; ++x) {
            int tile_index = tile_type_to_atlas_index[GetTile(tilemap, Vector2Int{x, y})];
            int overlay_index = tile_overlay_to_atlas_index[GetOverlay(tilemap, Vector2Int{x, y})];
            int object_index = tile_object_to_atlas_index[GetObject(tilemap, Vector2Int{x, y})];
            DrawTile(tile_atlas, Vector3{float(x), float(y), 0.0f}, tile_index, shaders, gpu_buffers);
            DrawTile(overlay_atlas, Vector3{float(x), float(y), 0.1f}, overlay_index, shaders, gpu_buffers);
            DrawTile(object_atlas, Vector3{float(x), float(y), 0.2f}, object_index, shaders, gpu_buffers);
        }
    }
}

void FreeTilemap(Tilemap* tilemap) {
    free(tilemap->state_map);
    free(tilemap->overlay_map);
    free(tilemap->object_map);
    (*tilemap).height = 0;
    (*tilemap).width = 0;
}

