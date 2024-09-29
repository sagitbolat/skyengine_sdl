#pragma once
#include "../Engine/sky_structs.h"
#include <unordered_map>
#include <tuple>

std::unordered_map<int, int> state_to_sprite_dict = {
    {0, 0},
    {1, 4},
    {2, 1},
    {3, 13},
    {4, 3},
    {5, 5},
    {6, 8},
    {7, 6},
    {8, 12},
    {9, 2},
    {10, 15},
    {11, 9},
    {12, 7},
    {13, 11},
    {14, 14},
    {15, 10}
};

struct Tilemap {
    int* state_map; //NOTE: Stores the actual tile types of the map. 1 is grass, 0 is dirt
    int* sprite_map; //NOTE: Stores the index of the tile on the sprite atlas
    int width;
    int height;
};

Tilemap InitTilemap(Vector2Int size) {
    Tilemap tilemap = {0};
    tilemap.state_map = (int*)malloc(sizeof(int) * size.x * size.y);
    tilemap.sprite_map = (int*)malloc(sizeof(int) * size.x * size.y);
    tilemap.width = size.x;
    tilemap.height = size.y;

    return tilemap;
}

void SetTile(Tilemap* tilemap, Vector2Int position, int tile_type) { 
    if (position.x >= tilemap->width || position.y >= tilemap->height) return;
    if (tile_type != 0 && tile_type != 1) return;
    // changes the state of the tile in position
    (*tilemap).state_map[position.x + position.y * tilemap->width] = tile_type;
}

int GetTile(Tilemap* tilemap, Vector2Int position) {
    if (position.x >= tilemap->width || position.y >= tilemap->height) return 0;
    return tilemap->state_map[position.x + position.y * tilemap->width];
}

int GetTileSpriteIndex(Tilemap* tilemap, Vector2Int position) {
    if (position.x >= tilemap->width - 1 || position.y >= tilemap->height - 1) return 0;
    return tilemap->sprite_map[position.x + position.y * tilemap->width];

} 

// NOTE: Adjusts the atlas index based on the tile state at this position (taking into account neighbors) 
void UpdateTileSprite(Tilemap* tilemap, Vector2Int position) {

    if (position.x >= tilemap->width-1 || position.y >= tilemap->height-1) return;
    int bottom_left = GetTile(tilemap, Vector2Int{position.x, position.y});
    int top_left = GetTile(tilemap, Vector2Int{position.x, position.y+1});
    int bottom_right = GetTile(tilemap, Vector2Int{position.x+1, position.y});
    int top_right = GetTile(tilemap, Vector2Int{position.x+1, position.y+1});

    int state_key = (bottom_left << 3) | (top_left << 2) | (bottom_right << 1) | top_right;

    int sprite_index = state_to_sprite_dict[state_key];
    (*tilemap).sprite_map[position.x + position.y * tilemap->width] = sprite_index;

}

void FreeTilemap(Tilemap* tilemap) {
    free(tilemap->state_map);
    free(tilemap->sprite_map);
    (*tilemap).height = 0;
    (*tilemap).width = 0;
}

