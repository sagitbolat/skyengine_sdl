#include <stdint.h>
#include <stdio.h>
#include "skyengine.h"
#include "arena_allocator.cpp"


struct ImageData {
    uint16_t width;
    uint16_t height;
    uint8_t bytes_per_pixel;
    uint8_t* data;
};

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

    


    FILE* image_file = fopen(image_file_name, "rb");
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


// arr is the array that contains 4 int8's
// i is the index of the first int8.
#define INT8ARRAY_TO_INT32(arr, i) (arr[i]) + (arr[i+1] << 8) + (arr[i+2] << 16) + (arr[i+3] << 24)

// NOTE: Do bitmap loading.
// NOTE: Returns the address of the asset on the GameMemory->transient_storage Arena.
size_t LoadBitmap(GameMemory* game_memory_arena, const char* image_file_name, ImageData* image_data) {
    
    const int FILE_HEADER_SIZE = 14;
    const int INFO_HEADER_SIZE = 40;
    int BYTES_PER_PIXEL = 3;
    
    FILE* image_file = fopen(image_file_name, "r");
    


    // SECTION: file_header
    uint8_t fh[FILE_HEADER_SIZE];
    size_t fh_size = fread(fh, sizeof(uint8_t), FILE_HEADER_SIZE, image_file);
    
    // NOTE: Error checking
    if (fh_size != FILE_HEADER_SIZE) {
        // TODO: Error
        printf("LoadBitmap Error. File header sizes do not match. Possible EOF error or wrong format.%c", '\n');
    }
    if (fh[0] != 'B' && fh[1] != 'M') {
        // TODO: Filetype Error
        printf("LoadBitmap Error. Incorrect filetype. Filetype must be of type bmp.%c", '\n');
    }
    // TODO: Might be unnecessary to save file size
    //uint32_t file_size = INT8ARRAY_TO_INT32(fh, 2);
    uint32_t byte_offset = INT8ARRAY_TO_INT32(fh, 10);

    
    // SECTION: info_header
    uint8_t ih[INFO_HEADER_SIZE];
    size_t ih_size = fread(ih, 1, INFO_HEADER_SIZE, image_file);
    
    uint32_t image_width   = INT8ARRAY_TO_INT32(ih, 4);
    uint32_t image_height  = INT8ARRAY_TO_INT32(ih, 8);
    uint8_t bits_per_pixel = ih[14]; 
    // NOTE: Error checking
    if (ih_size != INFO_HEADER_SIZE) {
        // TODO: Error
        printf("LoadBitmap Error. Info header sizes do not match. Possible EOF error or wrong format.%c", '\n');
    }
    if (BYTES_PER_PIXEL * 8 != bits_per_pixel) {
        // TODO: Pixel Resolution Error
        BYTES_PER_PIXEL = bits_per_pixel / 8;
        printf("LoadBitmap Warning. %d bits per pixel. Does not match 24. Will attempt to adjust to %d.%c", bits_per_pixel, BYTES_PER_PIXEL*8, '\n');
    }



    // SECTION: image data
    int width_in_bytes = image_width * BYTES_PER_PIXEL;
    int padding_size = (4 - (width_in_bytes) % 4) % 4;

    size_t image_size = image_width * image_height * BYTES_PER_PIXEL;

    image_data->width = image_width;
    image_data->height = image_height;
    image_data->bytes_per_pixel = BYTES_PER_PIXEL;
    image_data->data = (uint8_t*)(Arena::AllocateAsset(game_memory_arena, image_size));
   

    //move to start of bitmap data
    fseek(image_file, byte_offset, SEEK_SET);
    //read the data
    for (int h = image_height - 1; h >= 0; --h) {
        for (uint32_t w = 0; w < image_width; ++w) {
            uint32_t i = ((h * image_width) + w) * BYTES_PER_PIXEL;
            for (int k = 0; k < BYTES_PER_PIXEL; ++k) {
                fread(&(image_data->data[i+k]), 1, 1, image_file);
            }
        }
        fseek(image_file, padding_size, SEEK_CUR);         
    }
        
    fclose(image_file);
    
    return image_size;
}
