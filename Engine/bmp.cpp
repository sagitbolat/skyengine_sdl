#pragma once
#include <stdint.h>
#include <stdio.h>
#include <cstring>

#include "skyengine.h"
#include "allocation.cpp"
#include "sky_structs.h"

//NOTE: For cos and sin in RotateBitmap:
#include "skyintrinsics.h"


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
    for (int h = (int)image_height - 1; h >= 0; --h) {
        for (int w = 0; w < (int)image_width; ++w) {
            int i = ((h * (int)image_width) + w) * (int)bytes_per_pixel;
            for (int k = 0; k < (int)bytes_per_pixel; ++k) {
                fread(&(image_data->data[i+k]), 1, 1, image_file);
            }
        }
        if (padding_size > 0) fseek(image_file, padding_size, SEEK_CUR);         
    }
    fclose(image_file);
    
    return image_size;
}



// SECTION: Drawing.
// Returns: 1 if successful, 0 if outside bounds.
// NOTE: For now only support binary alpha values (i.e. 0 or non-zero)
int DrawPixel(GameBitmapBuffer* graphics_buffer, Color color, Vector2Int pos) {
    if (color.alpha == 0) return 1;

    int i = ((pos.y * graphics_buffer->pitch) + (pos.x * graphics_buffer->bytes_per_pixel));
    uint8_t* pixel = (uint8_t*)(graphics_buffer->memory);
    int max_i = ((graphics_buffer->width * graphics_buffer->bytes_per_pixel) + 
                (graphics_buffer->height * graphics_buffer->pitch));
    if (i < 0 || i >= max_i) {
        return 0;
    }
    pixel[i] = color.blue;
    pixel[i + 1] = color.green;
    pixel[i + 2] = color.red;
    return 1;
}


// NOTE: Assumes that pos0 is less than or equal to pos1.
void DrawLine(GameBitmapBuffer* graphics_buffer, Color color, Vector2Int pos0, Vector2Int pos1) {
    int x0 = pos0.x;
    int y0 = pos0.y;
    int x1 = pos1.x;
    int y1 = pos1.y;

    int dx = x1 - x0;
    int dy = y1 - y0;
    int D = 2*dy - dx;
    int y = y0;
    
    if (dx == 0) {
        for (int y = y0; y <= y1; ++y) {
            Vector2Int pos = {0};
            pos.x = x0;
            pos.y = y;
            DrawPixel(graphics_buffer, color, pos);
        }
        return;
    }

    for (int x = x0; x < x1; ++x) {
        //uint8_t* pixel = (uint8_t*)graphics_buffer->memory + 
        //             (x * graphics_buffer->bytes_per_pixel) + 
        //             (y * graphics_buffer->pitch);
        //pixel[0] = red;
        //pixel[1] = green;
        //pixel[2] = blue;
        Vector2Int pos = {0};
        pos.x = x;
        pos.y = y;
        DrawPixel(graphics_buffer, color, pos);
        if (D > 0) {
            ++y;
            D -= 2*dx;
        }
        D += 2*dy;
    }

}

// NOTE: For now only supports binary alpha values (i.e. 0 or non-zero.)
void DrawRectangle(GameBitmapBuffer* graphics_buffer, Color color, RectInt pos_and_size) {
    // NOTE: When refactoring to support continuous alpha values, remove this line
    if (color.alpha == 0) return;
    


    int min_x = pos_and_size.x;
    int min_y = pos_and_size.y;
    int width = pos_and_size.width;
    int height = pos_and_size.height;

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
            *pixel++ = ((color.red << 16) | (color.green << 8) | color.blue);
        }
        row += graphics_buffer->pitch;
    }
}

/*
void BlitBitmapSized(
    GameBitmapBuffer* graphics_buffer,
    ImageData* img_data,
    int x0, int y0,
    int new_height, int new_width,
    int pos_scale
    bool centered    
) {
    int w1 = img_data->width;
    int h1 = img_data->height;
    int w2 = new_width;
    int h2 = new_height;
    uint8_t* data = img_data->data;
    int bytes_per_pixel = img_data->bytes_per_pixel;
    

    if (w2 % w1 == 0 && h2 % h1 == 0 && w2 / w1 == h2 / h1) {
        BlitBitmapScaled(graphics_buffer, img_data, x0, y0, new_width/w, pos_scale, centered); 
    }
    
    float x_ratio = (float)w1 /(float)w2;
    float y_ratio = (float)h1 / (float)h2;
    
    float px;
    float py;

    for (int x = 0; x < h2; x++
}*/

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
    int pixel_scale, int pos_scale, 
    bool centered
) {
    int w = img_data->width;
    int h = img_data->height;
    uint8_t* data = img_data->data;
    int bytes_per_pixel = img_data->bytes_per_pixel;

    if (pos_scale == pixel_scale) { 
        if (pixel_scale  == 1) {
            if (centered) {
                x0 -= w/2;
                y0 -= h/2;
            }
            for (int y = y0; y < y0 + h; ++y) {
                for (int x = x0; x < x0 + w; ++x) {
                    int i = (((y - y0) * w) + (x - x0)) * bytes_per_pixel;
                    Color c = {0};
                    if (bytes_per_pixel > 3) c.alpha = data[i+3];
                    else c.alpha=1;
                    c.red = data[i + 2];
                    c.green = data[i + 1];
                    c.blue = data[i + 0];
                    Vector2Int pos = {0};
                    pos.x = x;
                    pos.y = y;
                    DrawPixel(graphics_buffer, c, pos);
                }
            }
        } else if (pixel_scale > 1) {
            
            if (centered) {
                x0 -= w*pixel_scale/2;
                y0 -= h*pixel_scale/2;
            }
            y0 *= pos_scale;
            x0 *= pos_scale;

            int ymax = y0 + (h * pixel_scale);
            int xmax = x0 + (w * pixel_scale);

            int yi = y0;
            for (int y = y0; y < ymax; y+=pixel_scale) {
                int xi = x0;
                for (int x = x0; x < xmax; x+=pixel_scale) {
                    int i = (((yi - y0) * w) + (xi - x0)) * bytes_per_pixel;
                    Color c = {0};
                    if (bytes_per_pixel > 3) c.alpha = data[i + 3];
                    else c.alpha = 1;
                    c.red = data[i + 2];
                    c.green = data[i + 1];
                    c.blue = data[i + 0];
                    RectInt pos_and_size = {0};
                    pos_and_size.x = x;
                    pos_and_size.y = y;
                    pos_and_size.width = pixel_scale;
                    pos_and_size.height = pixel_scale;

                    DrawRectangle(graphics_buffer, c, pos_and_size);
                    ++xi;
                }
                ++yi;
            }
        }
    } else { // NOTE: pos_scale != scale
        if (pixel_scale == 1) {
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
                    if (bytes_per_pixel > 3) c.alpha = data[i + 3];
                    else c.alpha = 1;
                    c.red = data[i + 2];
                    c.green = data[i + 1];
                    c.blue = data[i + 0];
                    Vector2Int pos = {0};
                    pos.x = x;
                    pos.y = y;
                    DrawPixel(graphics_buffer, c, pos);
                }
            }
        } else if (pixel_scale > 1) {
            if (centered) {
                x0 -= w*pixel_scale/2;
                y0 -= h*pixel_scale/2;
            }
            y0 *= pos_scale;
            x0 *= pos_scale;
            
            int ymax = y0 + (h * pixel_scale);
            int xmax = x0 + (w * pixel_scale);

            int yi = y0;
            for (int y = y0; y < ymax; y+=pixel_scale) {
                int xi = x0;
                for (int x = x0; x < xmax; x+=pixel_scale) {
                    int i = (((yi - y0) * w) + (xi - x0)) * bytes_per_pixel;
                    Color c = {0};
                    if (bytes_per_pixel > 3) c.alpha = data[i + 3];
                    else c.alpha = 1;
                    c.red = data[i + 2];
                    c.green = data[i + 1];
                    c.blue = data[i + 0];
                    RectInt pos_and_size = {0};
                    pos_and_size.x = x;
                    pos_and_size.y = y;
                    pos_and_size.width = pixel_scale;
                    pos_and_size.height = pixel_scale;
                    DrawRectangle(graphics_buffer, c, pos_and_size);
                    ++xi;
                }
                ++yi;
            }
        }
    
    }
}
/*
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
*/

