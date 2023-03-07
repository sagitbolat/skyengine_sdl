#pragma once
#include <stdint.h>

#include "bmp.cpp"



struct Tileset {
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t num_tiles;
    ImageData* tiles;
};

size_t LoadTileset (
    ArenaAllocator* asset_arena, 
    const char* image_file_name, 
    int tile_width,
    int tile_height,
    Tileset* tilemap
) {
    
    printf("Working...");

    FILE* image_file = fopen(image_file_name, "rb");
    uint32_t image_width = 0;
    uint32_t image_height = 0;
    uint32_t bytes_per_pixel = 0;
    uint32_t byte_offset = 0; // NOTE: The offset in bytes from the start of the file to the image data.
    uint32_t padding_size = 0; // NOTE: The padding at the end of each line of the bmp data. 
    size_t image_size = ImportBMP(image_file_name, image_file, &image_width, &image_height, &bytes_per_pixel, &byte_offset, &padding_size);
    
    printf("Working...");
    int height_in_tiles = image_height / tile_height;
    int width_in_tiles = image_width / tile_width;
    int num_tiles = width_in_tiles * height_in_tiles;
    
    tilemap->tile_width = tile_width;
    tilemap->tile_height = tile_height;
    tilemap->num_tiles = num_tiles; 
    tilemap->tiles = (ImageData*)(ArenaAllocateAsset(asset_arena, sizeof(ImageData) * num_tiles));
    
    printf("Working...");
    for (int i = 0; i < num_tiles; ++i) {
        tilemap->tiles[i].width = tile_width;
        tilemap->tiles[i].height = tile_height;
        tilemap->tiles[i].bytes_per_pixel = bytes_per_pixel;
        tilemap->tiles[i].data = (uint8_t*)(ArenaAllocateAsset(asset_arena, tile_height * tile_width * bytes_per_pixel));
    }    
    
    printf("Working...");
    //move to start of bitmap data
    fseek(image_file, byte_offset, SEEK_SET);
    //read the data
    printf("Working...");
    for (int h = (int)image_height - 1; h >= 0; --h) {
        for (int w = 0; w < (int)image_width; ++w) {
            int tilemap_h = h / (int)tile_height;
            int tilemap_w = w / (int)tile_width;
            int tilemap_i = (tilemap_h * (int)width_in_tiles) + tilemap_w;
            int tile_h = h % (int)tile_height;
            int tile_w = w % (int)tile_width;
            int tile_i = ((tile_h * (int)tile_width) + tile_w) * (int)bytes_per_pixel;
            for (int k = 0; k < (int)bytes_per_pixel; ++k) {
                fread(&(tilemap->tiles[tilemap_i].data[tile_i+k]), 1, 1, image_file);
            }
        }
        if (padding_size > 0) fseek(image_file, padding_size, SEEK_CUR);         
    }
    fclose(image_file);
    
    printf("Working...");
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
    uint8_t width = fgetc(file);
    uint8_t height = fgetc(file);
    
    fseek(file, 1, SEEK_CUR);

    Tilemap level_data = {0};
    level_data.tile_data = (uint8_t*)malloc(sizeof(uint8_t) * width * height);
    level_data.width = width;
    level_data.height = height;

    for (int h = 0; h < height; ++h) {
        fread(&(level_data.tile_data[h * width]), 1, width, file);
        fseek(file, 1, SEEK_CUR);
    }
    fclose(file);
    return level_data;
}
