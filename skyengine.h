#pragma once
#include <stdint.h>
#include "allocation.cpp"

#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif


// NOTE: Services that the game provides to the platform layer:
    // TODO: Maybe seperate sound on another thread and update async.

// NOTE: We need timing, controller/keyboard input, bitmap buffer for graphics, sound buffer.


// NOTE: Platform independant bitmap buffer.
struct GameBitmapBuffer {
    void*       memory;
    int         width;
    int         height;
    int         bytes_per_pixel;
    int         pitch;
};



// NOTE: KeyboardState data
#define KEY_STATE_Q     0x1
#define KEY_STATE_W     (1 << 1)
#define KEY_STATE_E     (1 << 2)
#define KEY_STATE_R     (1 << 3)
#define KEY_STATE_T     (1 << 4)
#define KEY_STATE_Y     (1 << 5)
#define KEY_STATE_U     (1 << 6)
#define KEY_STATE_I     (1 << 7)
#define KEY_STATE_O     (1 << 8)
#define KEY_STATE_P     (1 << 9)
#define KEY_STATE_A     (1 << 10)
#define KEY_STATE_S     (1 << 11)
#define KEY_STATE_D     (1 << 12)
#define KEY_STATE_F     (1 << 13)
#define KEY_STATE_G     (1 << 14)
#define KEY_STATE_H     (1 << 15)
#define KEY_STATE_J     (1 << 16)
#define KEY_STATE_K     (1 << 17)
#define KEY_STATE_L     (1 << 18)
#define KEY_STATE_Z     (1 << 19)
#define KEY_STATE_X     (1 << 20)
#define KEY_STATE_C     (1 << 21)
#define KEY_STATE_V     (1 << 22)
#define KEY_STATE_B     (1 << 23)
#define KEY_STATE_N     (1 << 24)
#define KEY_STATE_M     (1 << 25)
#define KEY_STATE_BRACE_LEFT    (1 << 26)
#define KEY_STATE_BRACE_RIGHT   (1 << 27)
#define KEY_STATE_BACKSLASH     (1 << 28)
#define KEY_STATE_COLON         (1 << 29)
#define KEY_STATE_QUOTE         (1 << 30)
#define KEY_STATE_COMMA         (1 << 31)
#define KEY_STATE_PERIOD        (1 << 32)
#define KEY_STATE_FORWARDSLASH  (1 << 33)
#define KEY_STATE_NUM1  (1 << 34)
#define KEY_STATE_NUM2  (1 << 35)
#define KEY_STATE_NUM3  (1 << 36)
#define KEY_STATE_NUM4  (1 << 37)
#define KEY_STATE_NUM5  (1 << 38)
#define KEY_STATE_NUM6  (1 << 39)
#define KEY_STATE_NUM7  (1 << 40)
#define KEY_STATE_NUM8  (1 << 41)
#define KEY_STATE_NUM9  (1 << 42)
#define KEY_STATE_NUM0  (1 << 43)
#define KEY_STATE_TILDA (1 << 44)
#define KEY_STATE_MINUS (1 << 45)
#define KEY_STATE_PLUS  (1 << 46)
#define KEY_STATE_BACKSPACE     (1 << 47)
#define KEY_STATE_TAB   (1 << 48)
#define KEY_STATE_CAPS  (1 << 49)
#define KEY_STATE_LEFTSHIFT     (1 << 50)
#define KEY_STATE_LEFTCTRL      (1 << 51)
#define KEY_STATE_MODKEY        (1 << 52)
#define KEY_STATE_LEFTALT       (1 << 53)
#define KEY_STATE_RIGHTALT      (1 << 54)
#define KEY_STATE_FUNCTION      (1 << 55)
#define KEY_STATE_RIGHTCTRL     (1 << 56)
#define KEY_STATE_RIGHTSHIFT    (1 << 57)
#define KEY_STATE_ENTER (1 << 58)
#define KEY_STATE_ESC   (1 << 59)
#define KEY_STATE_SPACE (1 << 60)
#define KEY_STATE_F1    (1 << 61)
#define KEY_STATE_F2    (1 << 62)
#define KEY_STATE_F3    (1 << 63)
#define KEY_STATE_F4    (1 << 64)
#define KEY_STATE_F5    (1 << 65)
#define KEY_STATE_F6    (1 << 66)
#define KEY_STATE_F7    (1 << 67)
#define KEY_STATE_F8    (1 << 68)
#define KEY_STATE_F9    (1 << 69) // NOTE: Nice.
#define KEY_STATE_F10   (1 << 70)
#define KEY_STATE_F11   (1 << 71)
#define KEY_STATE_F12   (1 << 72)
#define KEY_STATE_ARROWUP       (1 << 73)
#define KEY_STATE_ARROWDOWN     (1 << 74)
#define KEY_STATE_ARROWLEFT     (1 << 75)
#define KEY_STATE_ARROWRIGHT    (1 << 76)
#define KEY_STATE_PRINTSCREEN   (1 << 77)
#define KEY_STATE_SCREENLOCK    (1 << 78)
#define KEY_STATE_PAUSE         (1 << 79)
#define KEY_STATE_INSERT        (1 << 80)
#define KEY_STATE_HOME          (1 << 81)
#define KEY_STATE_PAGEUP        (1 << 82)
#define KEY_STATE_DELETE        (1 << 83)
#define KEY_STATE_END           (1 << 84)
#define KEY_STATE_PAGEDOWN      (1 << 85)


struct KeyboardState {
    uint64_t state;
    uint64_t prev_state;
};
// NOTE: input data for 4 controllers


// SECTION: Memory. This is the persistant memory that
// has to be passed from the platform layer to the game
// or vice versa. 
// Using this memory should be done with the Arena Allocator functions.
struct GameMemory {
    bool is_initialized;
    uint64_t permanent_storage_size;
    void* permanent_storage;            // Gamestate stuff


    uint64_t asset_storage_size;    // Asset loading.
    ArenaAllocator asset_storage;
};

// NOTE: The game state representation.
struct GameState {
    void* state;
};


// SECTION: Utils
struct Rect {
    uint16_t x, y, width, height;
};
struct Color {
    uint8_t a, red, green, blue;
};

static void GameUpdateAndRender(GameMemory*, GameBitmapBuffer*, KeyboardState*, int delta_time);
