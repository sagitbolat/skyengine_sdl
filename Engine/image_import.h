#pragma once
#include <stdint.h>
#include <stdio.h>

#include "sky_structs.h"

#define INT8ARRAY_TO_INT32(arr, i) (arr[i]) + (arr[i+1] << 8) + (arr[i+2] << 16) + (arr[i+3] << 24)


// Import BMP file
size_t ImportBMP (
    const char* image_file_name, FILE* image_file,
    uint32_t* image_width, uint32_t* image_height, 
    uint32_t* bytes_per_pixel, uint32_t* byte_offset,
    uint32_t* padding_size 
) {
    const int FILE_HEADER_SIZE = 14;
    const int INFO_HEADER_SIZE = 40;
    uint8_t BYTES_PER_PIXEL = 3;
    
    


    // SECTION: file_header
    uint8_t fh[FILE_HEADER_SIZE];
    size_t fh_size = fread(fh, sizeof(uint8_t), FILE_HEADER_SIZE, image_file);
    
    // NOTE: Error checking
    if (fh_size != FILE_HEADER_SIZE) {
        // TODO: Error
        printf("ImportBMP Error. File header sizes do not match. Possible EOF error or wrong format.%c", '\n');
    }
    if (fh[0] != 'B' && fh[1] != 'M') {
        // TODO: Filetype Error
        printf("ImportBMP Error. Incorrect filetype. Filetype must be of type bmp.%c", '\n');
    }
    // TODO: Might be unnecessary to save file size
    //uint32_t file_size = INT8ARRAY_TO_INT32(fh, 2);
    *byte_offset = INT8ARRAY_TO_INT32(fh, 10);

    
    // SECTION: info_header
    uint8_t ih[INFO_HEADER_SIZE];
    size_t ih_size = fread(ih, 1, INFO_HEADER_SIZE, image_file);
    
    *image_width   = INT8ARRAY_TO_INT32(ih, 4);
    *image_height  = INT8ARRAY_TO_INT32(ih, 8);
    uint8_t bits_per_pixel = ih[14]; 
    // NOTE: Error checking
    if (ih_size != INFO_HEADER_SIZE) {
        // TODO: Error
        printf("ImportBMP Error. Info header sizes do not match. Possible EOF error or wrong format.%c", '\n');
    }
    if (BYTES_PER_PIXEL * 8 != bits_per_pixel) {
        // TODO: Pixel Resolution Error
        BYTES_PER_PIXEL = bits_per_pixel / 8;
    }
    *bytes_per_pixel = BYTES_PER_PIXEL;

    // SECTION: image data
    uint32_t width_in_bytes = (*image_width) * BYTES_PER_PIXEL;
    *padding_size = (4 - (width_in_bytes) % 4) % 4;

    size_t image_size = (*image_width) * (*image_height) * BYTES_PER_PIXEL;
    return image_size;
}


// NOTE: Do bitmap loading.
// NOTE: Returns the address of the asset on the GameMemory->transient_storage Arena.
size_t LoadBitmap(const char* image_file_name, ImageData* image_data) {
  
    FILE* image_file = fopen(image_file_name, "rb");
    uint32_t image_width = 0;
    uint32_t image_height = 0;
    uint32_t bytes_per_pixel = 0;
    uint32_t byte_offset = 0; // NOTE: The offset in bytes from the start of the file to the image data.
    uint32_t padding_size = 0; // NOTE: The padding at the end of each line of the bmp data. 
    size_t image_size = ImportBMP(image_file_name, image_file, &image_width, &image_height, &bytes_per_pixel, &byte_offset, &padding_size);
     

    image_data->width = image_width;
    image_data->height = image_height;
    image_data->bytes_per_pixel = bytes_per_pixel;
    image_data->data = (uint8_t*)(malloc(sizeof(uint8_t) * image_size));
     

    //move to start of bitmap data
    fseek(image_file, byte_offset, SEEK_SET);
    //read the data
    for (int h = 0; h < int(image_height); ++h) {
        for (int w = 0; w < (int)image_width; ++w) {
            int i = ((h * (int)image_width) + w) * (int)bytes_per_pixel;
            //for (int k = (int)bytes_per_pixel - 1; k > 0; --k) {
            //    fread(&(image_data->data[i+k]), 1, 1, image_file);
            //}
            fread(&(image_data->data[i+2]), 1, 1, image_file);
            fread(&(image_data->data[i+1]), 1, 1, image_file);
            fread(&(image_data->data[i+0]), 1, 1, image_file);
            fread(&(image_data->data[i+3]), 1, 1, image_file);
        }
        if (padding_size > 0) fseek(image_file, padding_size, SEEK_CUR);         
    }
    fclose(image_file);
    
    return image_size;
}

void FreeBitmap(ImageData* img_data) {
    img_data->width = 0;
    img_data->height = 0;
    img_data->bytes_per_pixel = 0;
    free(img_data->data);
}