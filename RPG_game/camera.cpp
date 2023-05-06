#pragma once
#include "../Engine/bmp.cpp"

#include "globals.h"

struct Camera {
    Vector2Int position;
};
void DrawSpriteInWorldCoords(Camera camera, ImageData* sprite, Vector2Int sprite_world_coordinates) {
    BlitBitmapScaled(
        graphics_buffer, 
        sprite, 
        (int)(sprite_world_coordinates.x * TILE_SIZE - camera.position.x), 
        (int)(sprite_world_coordinates.y * TILE_SIZE - camera.position.y), 
        PIXEL_SCALE, PIXEL_SCALE, false
    ); 
}

void CenterOnCoords(Camera* camera, Vector2Int center_coordinates) {
    camera->position.x = (center_coordinates.x * TILE_SIZE) - (SCREEN_W)/(2*PIXEL_SCALE) + (TILE_SIZE/2);
    camera->position.y = (center_coordinates.y * TILE_SIZE) - (SCREEN_H)/(2*PIXEL_SCALE) + (TILE_SIZE/2);
}
