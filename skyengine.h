#if !defined(SKYENGINE_H)

#include <stdint.h>

#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))


// NOTE: Services that the game provides to the platform layer:
    // TODO: Maybe seperate sound on another thread and update async.

// NOTE: We meed timing, controller/keyboard input, bitmap buffer for graphics, sound buffer.


// NOTE: Platform independant bitmap buffer.
struct GameBitmapBuffer {
    void*       memory;
    int         width;
    int         height;
    int         pitch;
};



// NOTE: KeyboardState data
#define KEY_STATE_W     0x1
#define KEY_STATE_A     0x2
#define KEY_STATE_S     0x4
#define KEY_STATE_D     0x8
#define KEY_STATE_I     0x10
#define KEY_STATE_J     0x20
#define KEY_STATE_K     0x40
#define KEY_STATE_L     0x80
#define KEY_STATE_SPACE 0x100
struct KeyboardState {
    union {
        struct {
            uint64_t state;
            uint64_t prev_state;
        }; 
    };
};
// NOTE: input data for 4 controllers


// SECTION: Memory. This is the persistant memory that
// has to be passed from the platform layer to the game
// or vice versa.
struct GameMemory {
    bool is_initialized;
    uint64_t permanent_storage_size;
    void* permanent_storage;

    uint64_t transient_storage_size;
    void* transient_storage;
};

// NOTE: The game state representation.
struct GameState {
    int x_offset;
    int y_offset;
};
struct GameOfLifeState {
    uint32_t cells[256*144];
};
struct Color {
    uint8_t a, red, green, blue;
};
struct HuegeneState {
    Color cells[256*144];    
};


static void GameUpdateAndRender(GameMemory*, GameBitmapBuffer*, KeyboardState*, int delta_time);


#define SKYENGINE_H
#endif
