#include <stdint.h>

struct BmpFileHeader {
    uint16_t type;       // must always be set to 'BM' to declare that this is a .bmp-file.
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

struct RgbQuad {
    uint8_t blue, green, red, reserved;     // reserved must always be set to 0.
};



