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
struct KeyState {
    unsigned int Q : 1;                   
    unsigned int W : 1;
    unsigned int E : 1; 
    unsigned int R : 1; 
    unsigned int T : 1; 
    unsigned int Y : 1; 
    unsigned int U : 1; 
    unsigned int I : 1; 
    unsigned int O : 1; 
    unsigned int P : 1; 
    unsigned int A : 1; 
    unsigned int S : 1; 
    unsigned int D : 1; 
    unsigned int F : 1; 
    unsigned int G : 1; 
    unsigned int H : 1; 
    unsigned int J : 1; 
    unsigned int K : 1; 
    unsigned int L : 1; 
    unsigned int Z : 1; 
    unsigned int X : 1; 
    unsigned int C : 1; 
    unsigned int V : 1; 
    unsigned int B : 1; 
    unsigned int N : 1; 
    unsigned int M : 1; 
    unsigned int BRACE_LEFT : 1; 
    unsigned int BRACE_RIGHT : 1; 
    unsigned int BACKSLASH : 1; 
    unsigned int COLON : 1; 
    unsigned int QUOTE : 1; 
    unsigned int COMMA : 1; 
    unsigned int PERIOD : 1; 
    unsigned int FORWARDSLASH : 1;
    unsigned int NUM1 : 1; 
    unsigned int NUM2 : 1; 
    unsigned int NUM3 : 1; 
    unsigned int NUM4 : 1; 
    unsigned int NUM5 : 1; 
    unsigned int NUM6 : 1; 
    unsigned int NUM7 : 1; 
    unsigned int NUM8 : 1; 
    unsigned int NUM9 : 1; 
    unsigned int NUM0 : 1; 
    unsigned int TILDA : 1;
    unsigned int MINUS : 1; 
    unsigned int PLUS : 1; 
    unsigned int BACKSPACE : 1;   
    unsigned int TAB : 1; 
    unsigned int CAPS : 1;
    unsigned int LEFTSHIFT : 1; 
    unsigned int LEFTCTRL : 1; 
    unsigned int MODKEY : 1;
    unsigned int LEFTALT : 1;
    unsigned int RIGHTALT : 1;
    unsigned int FUNCTION : 1;
    unsigned int RIGHTCTRL : 1;
    unsigned int RIGHTSHIFT : 1;
    unsigned int ENTER : 1;
    unsigned int ESC : 1;
    unsigned int SPACE : 1;
    unsigned int F1 : 1;
    unsigned int F2 : 1;
    unsigned int F3 : 1;
    unsigned int F4 : 1;
    unsigned int F5 : 1;
    unsigned int F6 : 1;
    unsigned int F7 : 1;
    unsigned int F8 : 1;
    unsigned int F9 : 1;
    unsigned int F10 : 1;
    unsigned int F11 : 1;
    unsigned int F12 : 1;
    unsigned int ARROWUP : 1;
    unsigned int ARROWDOWN : 1;
    unsigned int ARROWLEFT : 1;
    unsigned int ARROWRIGHT : 1;
    unsigned int PRINTSCREEN : 1;
    unsigned int SCREENLOCK : 1;
    unsigned int PAUSE : 1; 
    unsigned int INSERT : 1;
    unsigned int HOME : 1;
    unsigned int PAGEUP : 1;
    unsigned int DELETE : 1;
    unsigned int END : 1;
    unsigned int PAGEDOWN : 1;  
};

struct KeyboardState {
    KeyState state;
    KeyState prev_state;
};
// NOTE: input data for 4 controllers


// SECTION: Memory. This is the persistant memory that
// has to be passed from the platform layer to the game
// or vice versa. 
// Using this memory should be done with the Arena Allocator functions.
struct GameMemory {
    bool is_initialized;
    uint64_t permanent_storage_size;
    ArenaAllocator permanent_storage;            // Gamestate stuff


    uint64_t asset_storage_size;    // Asset loading.
    ArenaAllocator asset_storage;
};

// NOTE: The game state representation.
struct GameState {
    void* state;
};


// SECTION: Utils
struct FRect {
    float x, y, width, height;
};
struct Rect {
    int x, y;
    unsigned int width, height;
};
struct Color {
    uint8_t alpha, red, green, blue;
};

static void GameUpdateAndRender(GameMemory*, GameBitmapBuffer*, KeyboardState*, int delta_time);
