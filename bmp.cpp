#include <stdint.h>
#include <stdio.h>
#include "skyengine.h"

void GenerateBitmapImage(Color* image_data, int width_in_pixels, int height_in_pixels, char* image_file_name) {
    const int FILE_HEADER_SIZE = 14;
    const int INFO_HEADER_SIZE = 40;
    const int BYTES_PER_PIXEL = 3;
    

    int width_in_bytes = width_in_pixels * BYTES_PER_PIXEL;
    uint8_t padding[3] = {0, 0, 0};
    int padding_size = (4 - (width_in_bytes) % 4) % 4;
    int stride = width_in_bytes + padding_size;

    uint16_t off_bits = (FILE_HEADER_SIZE + INFO_HEADER_SIZE);
    uint32_t file_size = off_bits + (stride * height_in_pixels);
    
    uint8_t fh[] = {
        0, 0, 
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
    };
    fh[0] = (uint8_t)'B';
    fh[1] = (uint8_t)'M';
    fh[2] = (uint8_t)(file_size);
    fh[3] = (uint8_t)(file_size >> 8);
    fh[4] = (uint8_t)(file_size >> 16);
    fh[5] = (uint8_t)(file_size >> 24);
    fh[10] = (uint8_t)(off_bits);
    
    
    uint8_t ih[] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0,
        0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
    };
    

    ih[ 0] = (uint8_t)(INFO_HEADER_SIZE); 
    ih[ 4] = (uint8_t)(width_in_pixels); 
    ih[ 5] = (uint8_t)(width_in_pixels >> 8); 
    ih[ 6] = (uint8_t)(width_in_pixels >> 16); 
    ih[ 7] = (uint8_t)(width_in_pixels >> 24); 
    ih[ 8] = (uint8_t)(height_in_pixels); 
    ih[ 9] = (uint8_t)(height_in_pixels >> 8); 
    ih[10] = (uint8_t)(height_in_pixels >> 16); 
    ih[11] = (uint8_t)(height_in_pixels >> 24); 
    ih[12] = (uint8_t)(1); 
    ih[14] = (uint8_t)(BYTES_PER_PIXEL * 8); 

    


    FILE* image_file = fopen(image_file_name, "wb");
    fwrite(fh, 1, FILE_HEADER_SIZE, image_file);
    fwrite(ih, 1, INFO_HEADER_SIZE, image_file);

    for (int h = height_in_pixels - 1; h >= 0; --h) {
        for (int w = 0; w < width_in_pixels; ++w) {
            int i = (h * width_in_pixels) + w;
            fwrite(&(image_data[i].blue), 1, 1, image_file);
            fwrite(&(image_data[i].green), 1, 1, image_file);
            fwrite(&(image_data[i].red), 1, 1, image_file);
        }
        fwrite(padding, 1, padding_size, image_file);

    }
    
    fclose(image_file);

}

