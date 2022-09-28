#include "SDL_sky.cpp"
#include "bmp.cpp"

static void Init(int* w, int *h) {
    *w = 1920;
    *h = 1080;
    return;
}

ImageData image_data = {0};
static void Awake(GameMemory* game_memory) {
    
    size_t image_size = LoadBitmap(game_memory, "test_tilemap.bmp", &image_data);
    
    printf("Image size: %ld%c", image_size, '\0');
    
    return;
}


static void Start(GameState* game_state, KeyboardState* keyboard_state) {
    return;
}

static void Update(GameState* game_state, KeyboardState* keyboard_state) {
    int w = image_data.width;
    int h = image_data.height;
    uint8_t* data = image_data.data;
    
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = (y * w) + x;
            Color c = {0};
            c.blue  = data[i];
            c.green = data[i + 1];
            c.blue  = data[i + 2];
            
            DrawRectangle(c.red, c.green, c.blue, x, y, x+1, y+1);

        }
    }

    
    return;
}
