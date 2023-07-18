#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "sky_renderer2D.h"
#include "sky_structs.h"

struct Tileset {
    int width_in_tiles;
    int height_in_tiles;
    Sprite atlas;
};
struct Tilemap {
    int width;
    int height;
    int num_layers;
    uint16_t* tile_data;
    uint8_t* collider_data;

    void SetCollider(int x, int y, uint8_t collider_type);
    uint8_t GetCollider(int x, int y);
    void SetTile(int x, int y, int layer, uint16_t tile_type);
    uint16_t GetTile(int x, int y, int layer);
};

void Tilemap::SetCollider(int x, int y, uint8_t collider_type) {
    int w = Tilemap::width;

    Tilemap::collider_data[(y*w) + x] = collider_type;
}

uint8_t Tilemap::GetCollider(int x, int y) {
    int w = Tilemap::width;

    return Tilemap::collider_data[(y*w) + x];
}

void Tilemap::SetTile(int x, int y, int layer, uint16_t tile_type) {
    int w = Tilemap::width;
    int l = Tilemap::num_layers;

    Tilemap::tile_data[(((y*w) + x)*l) + layer] = tile_type;
}

uint16_t Tilemap::GetTile(int x, int y, int layer) {
    int w = Tilemap::width;
    int l = Tilemap::num_layers;

    return Tilemap::tile_data[(((y*w) + x)*l) + layer];
}


// SECTION: Tilemap Level Loading.
// NOTE: A tilemap level should have the following format:
/* HEADER: 32 bytes to represent the width of the level, 32 bytes to represent the height of the level, 
 * 32 bits to represent the number of layers, and finally 32 bits of padding.
 * 
 * Each tile is represented with a string of 16-bit words. 
 * The first word represents the collision information of the time  
 * The rest of the 16-bit words represent the tile data. Each word is part of a layer. If the tilemap has 3 layers,
 * the bottom layer is the first of the non-collider words. If a tilemap has 3 total layers, each tile will be a 64-bit string 
 * with the first 16-bits being the colliders, and the rest of the 16-bit words being the bottom-top layers.
 * NOTE: if num_layers is only 1, then there are no collider layer.
 */
Tilemap LoadTilemap(const char* level_name) {
    printf("LoadTilemap 1...\n");
    FILE *file = fopen(level_name, "r");
    
    printf("LoadTilemap 2...\n");
    uint32_t width;
    fread(&width, 4, 1, file);
    uint32_t height;
    fread(&height, 4, 1, file);
    uint32_t num_layers;
    fread(&num_layers, 4, 1, file);
    uint32_t padding;
    fread(&padding, 4, 1, file);
    

    printf("LoadTilemap 3...\n");
    Tilemap tilemap = {0};
    tilemap.tile_data = (uint16_t*)malloc(sizeof(uint16_t) * (int)width * (int)height * (int)num_layers);
    tilemap.collider_data = (uint8_t*)malloc(sizeof(uint8_t) * (int)width * (int)height);
    tilemap.width = (int)width;
    tilemap.height = (int)height;
    tilemap.num_layers = (int)num_layers;
    

    // NOTE: Write tilemap sprite data
    for (int y = 0; y < tilemap.height; ++y) {
        for (int x = 0; x < tilemap.width; ++x) {
            for (int l = 0; l < tilemap.num_layers; ++l) {
                uint16_t tile_type = 0; 
                fread(&(tile_type), 2, 1, file);
                tilemap.SetTile(x, y, l, tile_type - 27);  
            }
        }
    }
    // NOTE: Write tilemap collider data
    for (int y = 0; y < tilemap.height; ++y) {
        for (int x = 0; x < tilemap.width; ++x) {
            uint8_t collider_type = 0; 
            fread(&(collider_type), 1, 1, file);
            tilemap.SetCollider(x, y, collider_type);  
        }
    }
    
    
    fclose(file);
    return tilemap;
}


void ExportTilemap(const char* level_name, Tilemap* tilemap) {
    FILE* file = fopen(level_name, "w");
    int width = tilemap->width;
    int height = tilemap->height;
    int num_layers = tilemap->num_layers;
    fwrite(&width, 4, 1, file);
    fwrite(&height, 4, 1, file);
    fwrite(&num_layers, 4, 1, file); 
    
    int last_x = 0;
    int last_y = 0;
    // NOTE: Write tilemap sprite data
    for (int y = 0; y < tilemap->height; ++y) {
        last_y = y;
        for (int x = 0; x < tilemap->width; ++x) {
            last_x = x;
            for (int l = 0; l < tilemap->num_layers; ++l) {
                uint16_t tile_type = tilemap->GetTile(x, y, l) + 27; 
                fwrite(&(tile_type), 2, 1, file); 
            }
        }
    }

    printf("%d, %d", last_x, last_y);

    // NOTE: Write tilemap collider data
    for (int y = 0; y < tilemap->height; ++y) {
        for (int x = 0; x < tilemap->width; ++x) {
            uint8_t collider_type = tilemap->GetCollider(x, y);
            fwrite(&(collider_type), 1, 1, file); 
        }
    }

    
    printf("Saved tilemap \n");
    fclose(file); 
}
