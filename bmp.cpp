#include <stdint.h>
#include <stdio.h>
#include "skyengine.h"

struct BmpFileHeader {
    uint8_t type1;       // must always be set to 'B' to declare that this is a .bmp-file.
    uint8_t type2;       // must always be set to 'M' to declare that this is a .bmp-file.
    uint32_t size;       // specifies the size of the file in bytes.
    uint16_t reserved1;  // must always be set to zero.
    uint16_t reserved2;  // must always be set to zero.
    uint32_t off_bits;   // specifies the offset from the beginning of the file to the bitmap data.
};

struct BmpInfoHeader {
    uint32_t size;                  // specifies the size of the BITMAPINFOHEADER structure, in bytes.
    uint32_t width;                 // specifies the width of the image, in pixels.
    uint32_t height;                // specifies the height of the image, in pixels.
    uint16_t planes;                // specifies the number of planes of the target device, must be set to zero.
    uint16_t bit_count;             // specifies the number of bits per pixel.
    uint32_t compression;           // specifies the type of compression, usually set to zero (no compression).
    uint32_t image_size;            // specifies the size of the image data, in bytes. If there is no compression, it is valid to set this member to zero.
    uint32_t x_pixels_per_meter;    // specifies the the horizontal pixels per meter on the designated targer device, usually set to zero.
    uint32_t y_pixels_per_meter;    // specifies the the vertical pixels per meter on the designated targer device, usually set to zero.
    uint32_t colors_used;           // specifies the number of colors used in the bitmap, if set to zero the number of colors is calculated using the biBitCount member.
    uint32_t colors_important;      // specifies the number of color that are 'important' for the bitmap, if set to zero, all colors are important.
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
    
    
    uint8_t ih = {
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
    fwrite(fh, 1, sizeof(BmpFileHeader), image_file);
    fwrite(ih, 1, sizeof(BmpInfoHeader), image_file);

    for (int h = 0; h < (height_in_pixels); ++h) {
        for (int w = 0; w < width_in_pixels; ++w) {
            int i = (h * width_in_pixels) + w;
            fwrite(&(image_data[i].red), 1, 8, image_file);
            fwrite(&(image_data[i].green), 1, 8, image_file);
            fwrite(&(image_data[i].blue), 1, 8, image_file);
        }
        fwrite(padding, 1, padding_size, image_file);
    }
    
    fclose(image_file);

}

