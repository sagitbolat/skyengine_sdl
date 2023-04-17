#pragma once
#include <stdint.h>

#include "bmp.cpp"



struct Tileset {
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t num_tiles;
    ImageData* tiles;
    uint16_t width_in_tiles; // NOTE: Represents the width of the full tileset in tiles.
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
    printf("num_tiles: %d\n", num_tiles);
    printf("tile_width: %d\n", tile_width);
    for (int i = 0; i < num_tiles; ++i) {
        printf("i: %d\n", i);
        tileset->tiles[i].width = tile_width;
        tileset->tiles[i].height = tile_height;
        tileset->tiles[i].bytes_per_pixel = (uint8_t)bytes_per_pixel;
        tileset->tiles[i].data = (uint8_t*)(ArenaAllocateAsset(asset_arena, tile_height * tile_width * bytes_per_pixel));
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



struct Tilemap {
    int width;
    int height;
    uint8_t* tile_data;
};

// SECTION: Tilemap Level Loading.
// NOTE: A tilemap level should have the following format:
/* HEADER: 8 bytes to represent the width of the level, 8 bytes to represent the height of the level, newline character
 * n 8-byte characters each representing a tile, where n is the width of the level. This is followed by a newline char
 * The above line repeats m times, where m is the height of the level. 
 * Ex:
 * 44 // THIS IS THE HEADER
 * 1111 
 * 1001
 * 1001
 * 1111
 */
Tilemap LoadTilemap(const char* level_name) {
    FILE *file = fopen(level_name, "r");
    uint32_t width;
    fread(&width, 4, 1, file);
    uint32_t height;
    fread(&height, 4, 1, file);

    Tilemap level_data = {0};
    level_data.tile_data = (uint8_t*)malloc(sizeof(uint8_t) * (int)width * (int)height);
    level_data.width = (int)width;
    level_data.height = (int)height;

    for (int h = 0; h < (int)height; ++h) {
        fread(&(level_data.tile_data[h * (int)width]), 1, (int)width, file);
    }
    fclose(file);
    return level_data;
}
