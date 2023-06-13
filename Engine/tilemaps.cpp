#pragma once
#include <stdint.h>

#include "bmp.cpp"

#include "sky_structs.h"

struct Tileset {
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t num_tiles;
    ImageData* tiles;
    uint16_t width_in_tiles; // NOTE: Represents the width of the full tileset in tiles.
};
struct Tilemap {
    int width;
    int height;
    int num_layers;
    uint16_t* tile_data;
    uint8_t* collider_data;
};


size_t LoadTileset (
    ArenaAllocator* asset_arena, 
    const char* image_file_name, 
    uint16_t tile_width,
    uint16_t tile_height,
    Tileset* tileset
) {
    
    printf("LoadTileset Working BP 1\n");

    FILE* image_file = fopen(image_file_name, "rb");
    uint32_t image_width = 0;
    uint32_t image_height = 0;
    uint32_t bytes_per_pixel = 0;
    uint32_t byte_offset = 0; // NOTE: The offset in bytes from the start of the file to the image data.
    uint32_t padding_size = 0; // NOTE: The padding at the end of each line of the bmp data. 
    size_t image_size = ImportBMP(image_file_name, image_file, &image_width, &image_height, &bytes_per_pixel, &byte_offset, &padding_size);
    
    printf("LoadTileset Working BP 2\n");
    int height_in_tiles = image_height / tile_height;
    int width_in_tiles = image_width / tile_width;
    uint16_t num_tiles = width_in_tiles * height_in_tiles;
    
    tileset->tile_width = tile_width;
    tileset->tile_height = tile_height;
    tileset->num_tiles = num_tiles; 
    tileset->tiles = (ImageData*)(ArenaAllocateAsset(asset_arena, sizeof(ImageData) * num_tiles));
    tileset->width_in_tiles = (uint16_t)width_in_tiles;    

    printf("LoadTileset Working BP 3\n");
    for (int i = 0; i < num_tiles; ++i) {
        tileset->tiles[i].width = tile_width;
        tileset->tiles[i].height = tile_height;
        tileset->tiles[i].bytes_per_pixel = (uint8_t)bytes_per_pixel;
        //tileset->tiles[i].data = (uint8_t*)(ArenaAllocateAsset(asset_arena, tile_height * tile_width * bytes_per_pixel));
        tileset->tiles[i].data = (uint8_t*)malloc(tile_height * tile_width * bytes_per_pixel);
    }    
    
    printf("LoadTileset Working BP 4\n");
    //move to start of bitmap data
    fseek(image_file, byte_offset, SEEK_SET);
    //read the data
    printf("LoadTileset Working BP 5\n");
    for (int h = (int)image_height - 1; h >= 0; --h) {
        for (int w = 0; w < (int)image_width; ++w) {
            int tileset_h = h / (int)tile_height;
            int tileset_w = w / (int)tile_width;
            int tileset_i = (tileset_h * (int)width_in_tiles) + tileset_w;
            int tile_h = h % (int)tile_height;
            int tile_w = w % (int)tile_width;
            int tile_i = ((tile_h * (int)tile_width) + tile_w) * (int)bytes_per_pixel;
            for (int k = 0; k < (int)bytes_per_pixel; ++k) {
                fread(&(tileset->tiles[tileset_i].data[tile_i+k]), 1, 1, image_file);
            }
        }
        if (padding_size > 0) fseek(image_file, padding_size, SEEK_CUR);         
    }
    fclose(image_file);
    
    printf("LoadTileset Working BP 6\n");
    return image_size;
}




void SetTilemapCollider(Tilemap* tm, int x, int y, uint8_t collider_type) {
    int w = tm->width;

    tm->collider_data[(y*w) + x] = collider_type;
}

uint8_t GetTilemapCollider(Tilemap* tm, int x, int y) {
    int w = tm->width;

    return tm->collider_data[(y*w) + x];
}

void SetTilemapTile(Tilemap* tm, int x, int y, int layer, uint16_t tile_type) {
    int w = tm->width;
    int l = tm->num_layers;

    tm->tile_data[(((y*w) + x)*l) + layer] = tile_type;
}

uint16_t GetTilemapTile(Tilemap* tm, int x, int y, int layer) {
    int w = tm->width;
    int l = tm->num_layers;

    return tm->tile_data[(((y*w) + x)*l) + layer];
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
    Tilemap level_data = {0};
    level_data.tile_data = (uint16_t*)malloc(sizeof(uint16_t) * (int)width * (int)height * (int)num_layers);
    level_data.collider_data = (uint8_t*)malloc(sizeof(uint8_t) * (int)width * (int)height);
    level_data.width = (int)width;
    level_data.height = (int)height;
    level_data.num_layers = (int)num_layers;
    

    // NOTE: Write tilemap sprite data
    for (int y = 0; y < level_data.height; ++y) {
        for (int x = 0; x < level_data.width; ++x) {
            for (int l = 0; l < level_data.num_layers; ++l) {
                uint16_t tile_type = 0; 
                fread(&(tile_type), 2, 1, file);
                SetTilemapTile(&level_data, x, y, l, tile_type - 27);  
            }
        }
    }
    // NOTE: Write tilemap collider data
    for (int y = 0; y < level_data.height; ++y) {
        for (int x = 0; x < level_data.width; ++x) {
            uint8_t collider_type = 0; 
            fread(&(collider_type), 1, 1, file);
            SetTilemapCollider(&level_data, x, y, collider_type);  
        }
    }
    /*
    // NOTE: Read Tile sprite data
    for (int h = 0; h < (int)height; ++h) {
        fread(&(level_data.tile_data[h * (int)width * (int)num_layers]), 2, (int)width*(int)num_layers, file);
    }
    
    // NOTE: Read Tile collider data
    for (int h = 0; h < (int)height; ++h) {
        fread(&(level_data.collider_data[h * (int)width]), 1, (int)width, file);
    }
    */
    
    fclose(file);
    return level_data;
}
