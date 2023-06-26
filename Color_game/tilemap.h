#pragma once
#include "../Engine/sky_structs.h"

struct Tileset {
    Sprite atlas;
    int width_in_tiles;
    int height_in_tiles;
};
struct Tilemap {
    int* map;
    int width;
    int height;
};

// FIXME: Right now, values other than 0 and 1 are hardcoded to be walls. Change this to read collider data.
// NOTE: Returns true if colliding with wall, false if not colliding with wall.
bool TestTileCollide(Tilemap map, Vector2Int tile_pos) {
    int tile = map.map[tile_pos.y * map.width + tile_pos.x];
    if ((tile == 1 || tile == 0)) return false;
    else return true;
}