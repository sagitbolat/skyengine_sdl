#pragma once
#include <stdint.h>
#include <stdio.h>
#include <cstring>

#include "skyengine.h"
#include "allocation.cpp"


//NOTE: For cos and sin in RotateBitmap:
#include "skyintrinsics.h"

struct ImageData {
    uint16_t width;
    uint16_t height;
    uint8_t bytes_per_pixel;
    uint8_t* data;
};

struct TilemapData {
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t num_tiles;
    ImageData* tiles;
};

// NOTE: Assumes angle is in Degrees
ImageData RotateBitmap(ArenaAllocator* frame_arena, ImageData image, float angle) {
    constexpr int TRANSPARENT_COLOR = 0x00000000; 
    // Convert the angle from degrees to radians
    float radians = angle * M_PI / 180.0f;

    // Pre-compute the sine and cosine values for the angle
    float sinVal = std::sin(radians);
    float cosVal = std::cos(radians);

    // Calculate the size of the rotated image
    int rotatedWidth = (int) (fabs(cosVal * image.width) + fabs(sinVal * image.height));
    int rotatedHeight = (int) (fabs(sinVal * image.width) + fabs(cosVal * image.height));

    // Create a new array to store the rotated image data
    ImageData rotatedImageData = {0};
    rotatedImageData.height = rotatedHeight;
    rotatedImageData.width = rotatedWidth;
    rotatedImageData.bytes_per_pixel = image.bytes_per_pixel;
    size_t image_size = image.width * image.height * image.bytes_per_pixel;

    rotatedImageData.data = (uint8_t*)(ArenaAllocateAsset(frame_arena, image_size));

    // Initialize the rotated image data to transparent pixels
    for (int i = 0; i < rotatedWidth * rotatedHeight * image.bytes_per_pixel; i += image.bytes_per_pixel) {
        std::memcpy(&rotatedImageData.data[i], &TRANSPARENT_COLOR, image.bytes_per_pixel);
    }


    float x_shear = -sinVal;
    float y_shear = cosVal / sinVal;


    // Iterate over each pixel in the image
    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            int srcX = (int) ((x + y * x_shear) * cosVal + image.width / 2 - rotatedWidth / 2);
            int srcY = (int) ((y + x * y_shear) * sinVal + image.height / 2 - rotatedHeight / 2);
            if (srcX >= 0 && srcX < image.width && srcY >= 0 && srcY < image.height) {
                // Copy the pixel from the original image to the rotated image
                int srcIndex = (srcY * image.width + srcX) * image.bytes_per_pixel;
                int dstIndex = (y * rotatedWidth + x) * rotatedImageData.bytes_per_pixel;
                rotatedImageData.data[dstIndex] = image.data[srcIndex];
                rotatedImageData.data[dstIndex + 1] = image.data[srcIndex + 1];
                rotatedImageData.data[dstIndex + 2] = image.data[srcIndex + 2];
            }
        }
    }

    return rotatedImageData;
    
}


void GenerateBitmapImage(ImageData image_data, const char* image_file_name) {
    const int FILE_HEADER_SIZE = 14;
    const int INFO_HEADER_SIZE = 40;

    int width_in_bytes = image_data.width * image_data.bytes_per_pixel;
    uint8_t padding[3] = {0, 0, 0};
    int padding_size = (4 - (width_in_bytes) % 4) % 4;
    int stride = width_in_bytes + padding_size;

    uint16_t off_bits = (FILE_HEADER_SIZE + INFO_HEADER_SIZE);
    uint32_t file_size = off_bits + (stride * image_data.height);
    
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
    ih[ 4] = (uint8_t)(image_data.width); 
    ih[ 5] = (uint8_t)(image_data.width >> 8); 
    ih[ 6] = (uint8_t)(image_data.width >> 16); 
    ih[ 7] = (uint8_t)(image_data.width >> 24); 
    ih[ 8] = (uint8_t)(image_data.height); 
    ih[ 9] = (uint8_t)(image_data.height >> 8); 
    ih[10] = (uint8_t)(image_data.height >> 16); 
    ih[11] = (uint8_t)(image_data.height >> 24); 
    ih[12] = (uint8_t)(1); 
    ih[14] = (uint8_t)(image_data.bytes_per_pixel * 8); 

    
    printf("Exporting...\n");

    FILE* image_file = fopen(image_file_name, "w");
    fwrite(fh, 1, FILE_HEADER_SIZE, image_file);
    fwrite(ih, 1, INFO_HEADER_SIZE, image_file);

    for (int h = image_data.height - 1; h >= 0; --h) {
        for (int w = 0; w < image_data.width; ++w) {
            int i = (h * image_data.width * image_data.bytes_per_pixel) + (w * image_data.bytes_per_pixel);
	    for (int b = 0; b < image_data.bytes_per_pixel; ++b) {    
		fwrite(&(image_data.data[i+b]), 1, 1, image_file);
	    }
        }
        fwrite(padding, 1, padding_size, image_file);
    }
    
    printf("Exporting...\n");
    fclose(image_file);

}


// arr is the array that contains 4 int8's
// i is the index of the first int8.
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
        printf("ImportBMP Warning. %d bits per pixel. Does not match 24. Will attempt to adjust to %d.%c", bits_per_pixel, BYTES_PER_PIXEL*8, '\n');
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
size_t LoadBitmap(ArenaAllocator* asset_arena, const char* image_file_name, ImageData* image_data) {
  
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
    image_data->data = (uint8_t*)(ArenaAllocateAsset(asset_arena, image_size));
     

    //move to start of bitmap data
    fseek(image_file, byte_offset, SEEK_SET);
    //read the data
    for (uint32_t h = 0; h < image_height; ++h) {
        for (uint32_t w = 0; w < image_width; ++w) {
            uint32_t i = ((h * image_width) + w) * bytes_per_pixel;
            for (uint32_t k = 0; k < bytes_per_pixel; ++k) {
                fread(&(image_data->data[i+k]), 1, 1, image_file);
            }
        }
        if (padding_size > 0) fseek(image_file, padding_size, SEEK_CUR);         
    }
    fclose(image_file);
    
    return image_size;
}

size_t LoadTilemap (
    ArenaAllocator* asset_arena, 
    const char* image_file_name, 
    int tile_width,
    int tile_height,
    TilemapData* tilemap
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
    for (uint32_t h = 0; h < image_height; ++h) {
        for (uint32_t w = 0; w < image_width; ++w) {
            int tilemap_h = h / tile_height;
            int tilemap_w = w / tile_width;
            int tilemap_i = (tilemap_h * width_in_tiles) + tilemap_w;
            int tile_h = h % tile_height;
            int tile_w = w % tile_width;
            int tile_i = ((tile_h * tile_width) + tile_w) * bytes_per_pixel;
            for (uint32_t k = 0; k < bytes_per_pixel; ++k) {
                fread(&(tilemap->tiles[tilemap_i].data[tile_i+k]), 1, 1, image_file);
            }
        }
        if (padding_size > 0) fseek(image_file, padding_size, SEEK_CUR);         
    }
    fclose(image_file);
    
    printf("Working...");
    return image_size;
}

// NOTE: bitmap tilemap loading
// returns size of the entire tilemap allocation.
size_t LoadTilemapOld (
    ArenaAllocator* asset_arena, 
    const char* image_file_name, 
    int tile_width, int tile_height, 
    ImageData tile_array[]
) {
    ImageData tilemap_image = {0};
    size_t tilemap_size = LoadBitmap(asset_arena, image_file_name, &tilemap_image);
     
    int image_width = tilemap_image.width;
    int image_height = tilemap_image.height; 
    int bytes_per_pixel = tilemap_image.bytes_per_pixel;

    int height_in_tiles = image_height / tile_height;
    int width_in_tiles = image_width / tile_width;
    int num_tiles = width_in_tiles * height_in_tiles;
    
    // NOTE: Allocate the ImageDatas array
    tile_array = (ImageData*)(ArenaAllocateAsset(asset_arena, sizeof(ImageData) * num_tiles));
    for (int tile_h = 0; tile_h < height_in_tiles; ++tile_h) {
        for (int tile_w = 0; tile_w < width_in_tiles; ++tile_w) {
            int tile_i = (tile_h * width_in_tiles) + tile_w;

            tile_array[tile_i].width = tile_width;
            tile_array[tile_i].height = tile_height;
            tile_array[tile_i].bytes_per_pixel = bytes_per_pixel;
            tile_array[tile_i].data = (uint8_t*)(ArenaAllocateAsset(asset_arena, tile_width * tile_height * bytes_per_pixel));
            
            int y_offset = tile_h * tile_height;
            int x_offset = tile_w * tile_width;
            for (int h = y_offset; h < tile_height + y_offset; ++h) {
                for (int w = x_offset; w < tile_width + x_offset; ++w) {
                    int image_i = (h * image_width) + w;
                    int i = ((h - y_offset) * tile_width) + (w - x_offset);
                    for (int k = 0; k < bytes_per_pixel; ++k) {
                        tile_array[tile_i].data[i+k] = tilemap_image.data[image_i+k];
                    } 
                }

            }
        
        } 
    }
    
    return tilemap_size; 
}

// SECTION: Drawing.
// Returns: 1 if successful, 0 if outside bounds.
int DrawPixel(GameBitmapBuffer* graphics_buffer, uint8_t red, uint8_t green, uint8_t blue, int x, int y) {
    int i = ((y * graphics_buffer->pitch) + (x * graphics_buffer->bytes_per_pixel));
    uint8_t* pixel = (uint8_t*)(graphics_buffer->memory);
    int max_i = ((graphics_buffer->width * graphics_buffer->bytes_per_pixel) + 
                (graphics_buffer->height * graphics_buffer->pitch));
    if (i < 0 || i >= max_i) {
        return 0;
    }
    pixel[i] = blue;
    pixel[i + 1] = green;
    pixel[i + 2] = red;
    return 1;
}

void DrawLine(GameBitmapBuffer* graphics_buffer, uint8_t red, uint8_t green, uint8_t blue, int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int D = 2*dy - dx;
    int y = y0;

    for (int x = x0; x < x1; ++x) {
        //uint8_t* pixel = (uint8_t*)graphics_buffer->memory + 
        //             (x * graphics_buffer->bytes_per_pixel) + 
        //             (y * graphics_buffer->pitch);
        //pixel[0] = red;
        //pixel[1] = green;
        //pixel[2] = blue;
        DrawPixel(graphics_buffer, red, green, blue, x, y);
        if (D > 0) {
            ++y;
            D -= 2*dx;
        }
        D += 2*dy;
    }

}

void DrawRectangle(GameBitmapBuffer* graphics_buffer, uint8_t red, uint8_t green, uint8_t blue, int x0, int y0, int width, int height) {
    int min_x = x0;
    int min_y = y0;
    int max_x = min_x + width;
    int max_y = min_y + height;
    if (min_x < 0) {
        min_x = 0;
    }
    if (min_y < 0) {
        min_y = 0;
    }
    if (max_x >= graphics_buffer->width) {
        max_x = graphics_buffer->width;
    }
    if (max_y >= graphics_buffer->height) {
        max_y = graphics_buffer->height;
    }

    uint8_t* row = (uint8_t*)graphics_buffer->memory + 
                     (min_x * graphics_buffer->bytes_per_pixel) + 
                     (min_y * graphics_buffer->pitch);

    for (int y = min_y; y < max_y; ++y) {
       
        uint32_t* pixel = (uint32_t*)row;
        
        for (int x = min_x; x < max_x; ++x) {
            *pixel++ = ((red << 16) | (green << 8) | blue);
        }
        row += graphics_buffer->pitch;
    }
}


// NOTE: If centered is false, the x0, y0 coordinate will be in the top left of the bitmap. If it is true, it will be in the center of the bitmap
// NOTE: scale is the size taken up by each pixel in the bmp file on the graphics buffer. Scale = 1 means a 16x16 bitmap will be taking up 16x16 pixel space on screen.
//          Scale = 2 means a 16x16 bitmap will be taking up 32x32 pixels on the screen. Etc.
// NOTE: pos_scale is how the x0 and y0 coordinates of the bitmap will be positioned on the screen. 
//          If x0 = 1, y0 = 1 and pos_scale = 1, then the bitmap will be positioned at (1,1) pixel on screen.
//          If x0 = 1, y0 = 1 and pos_scale = 16, then the bitmap will be positioned at (16, 16) pixel on screen.
// NOTE: centered refers to if the x0, y0 coords represent the center of the bitmap, or the top left corner. 
//          If centered = false, then the x0 y0 coords are the top left of the bitmap.
//          If centered = true, then the bitmap center will be at x0 y0.
void BlitBitmapScaled (
    GameBitmapBuffer* graphics_buffer, 
    ImageData* img_data, 
    int x0, int y0, 
    int scale, int pos_scale, 
    bool centered
) {
    int w = img_data->width;
    int h = img_data->height;
    uint8_t* data = img_data->data;
    int bytes_per_pixel = img_data->bytes_per_pixel;

    if (pos_scale == scale) { 
        if (scale == 1) {
            if (centered) {
                x0 -= w/2;
                y0 -= h/2;
            }
            for (int y = y0; y < y0 + h; ++y) {
                for (int x = x0; x < x0 + w; ++x) {
                    int i = (((y - y0) * w) + (x - x0)) * bytes_per_pixel;
                    Color c = {0};
                    c.red = data[i + 2];
                    c.green = data[i + 1];
                    c.blue = data[i + 0];
                    DrawPixel(graphics_buffer, c.red, c.green, c.blue, x, y);
                }
            }
        } else if (scale > 1) {
            
            if (centered) {
                x0 -= w*scale/2;
                y0 -= h*scale/2;
            }
            y0 *= pos_scale;
            x0 *= pos_scale;

            int ymax = y0 + (h * scale);
            int xmax = x0 + (w * scale);

            int yi = y0;
            for (int y = y0; y < ymax; y+=scale) {
                int xi = x0;
                for (int x = x0; x < xmax; x+=scale) {
                    int i = (((yi - y0) * w) + (xi - x0)) * bytes_per_pixel;
                    Color c = {0};
                    c.red = data[i + 2];
                    c.green = data[i + 1];
                    c.blue = data[i + 0];
                    DrawRectangle(graphics_buffer, c.red, c.green, c.blue, x, y, scale, scale);
                    ++xi;
                }
                ++yi;
            }
        }
    } else { // NOTE: pos_scale != scale
        if (scale == 1) {
            y0 *= pos_scale;
            x0 *= pos_scale;
            if (centered) {
                x0 -= w/2;
                y0 -= h/2;
            }
            for (int y = y0; y < y0 + h; ++y) {
                for (int x = x0; x < x0 + w; ++x) {
                    int i = (((y - y0) * w) + (x - x0)) * bytes_per_pixel;
                    Color c = {0};
                    c.red = data[i + 2];
                    c.green = data[i + 1];
                    c.blue = data[i + 0];
                    DrawPixel(graphics_buffer, c.red, c.green, c.blue, x, y);
                }
            }
        } else if (scale > 1) {
            if (centered) {
                x0 -= w*scale/2;
                y0 -= h*scale/2;
            }
            y0 *= pos_scale;
            x0 *= pos_scale;
            
            int ymax = y0 + (h * scale);
            int xmax = x0 + (w * scale);

            int yi = y0;
            for (int y = y0; y < ymax; y+=scale) {
                int xi = x0;
                for (int x = x0; x < xmax; x+=scale) {
                    int i = (((yi - y0) * w) + (xi - x0)) * bytes_per_pixel;
                    Color c = {0};
                    c.red = data[i + 2];
                    c.green = data[i + 1];
                    c.blue = data[i + 0];
                    DrawRectangle(graphics_buffer, c.red, c.green, c.blue, x, y, scale, scale);
                    ++xi;
                }
                ++yi;
            }
        }
    
    }
}

void BlitBitmap(GameBitmapBuffer* graphics_buffer, ImageData* img_data, int x0, int y0, int scale, bool centered = true, bool scale_position = false) {
    int w = img_data->width;
    int h = img_data->height;
    uint8_t* data = img_data->data;
    int bytes_per_pixel = img_data->bytes_per_pixel;


    if (scale > 1 && scale_position) {
        x0 /= scale;
        y0 /= scale;
    }
    if (centered) {
        x0 -= (w/2);
        y0 -= (h/2);
    }
    for (int y = y0; y < y0+h; ++y) {
        for (int x = x0; x < x0+w; ++x) {
            int i = (((y - y0) * w) + (x - x0)) * bytes_per_pixel;
            Color c = {0};
            c.red = data[i + 2];
            c.green = data[i + 1];
            c.blue = data[i + 0];
            if (scale == 1) {
                DrawPixel(graphics_buffer, c.red, c.green, c.blue, x, y);
            } else if (scale > 1) {
                DrawRectangle(graphics_buffer, c.red, c.green, c.blue, x, y, scale, scale);
            }
        }
    }
}


