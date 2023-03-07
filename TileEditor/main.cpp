#include "../Engine/SDL_sky.cpp"
#include "../Engine/bmp.cpp"
#include "../Engine/tilemaps.cpp"
#include "stdio.h"


int SCREEN_H = 720;
int SCREEN_W = 1280;


Tileset font = {0};
Tileset ui_set = {0};

// SECTION: main
void Init(int* w, int* h) {
    *w = SCREEN_W;
    *h = SCREEN_H;
}




void Awake(GameMemory* gm) {
    LoadTileset(&gm->asset_storage, "assets/font_light.bmp", 8, 8, &font);
    LoadTileset(&gm->asset_storage, "assets/UI_tileset.bmp", 8, 8, &ui_set);
}


void Start(GameState* gs, KeyboardState* ks) {
    
}

void DisplayString(GameBitmapBuffer* graphics_buffer, const char* text, Tileset* tiles, int x, int y, int scale) {
    int length = strlen(text);
    int x_offset = 0;
    for (int c = 0; c < length; ++c) {
        BlitBitmapScaled(graphics_buffer, &(tiles->tiles[(int)text[c]]), x + x_offset, y, scale, 1, false);
        x_offset += tiles->tile_width * scale;
    }
}

void RenderTextButton(GameBitmapBuffer* gb, const char* text, int x, int y, int scale) {
    const int BORDER_WIDTH = 2;
    
    x += BORDER_WIDTH*scale;
    y += BORDER_WIDTH*scale;
    int length = strlen(text);
    
    // SECTION: left border.
    BlitBitmapScaled(gb, &ui_set.tiles[4], x - (scale * ui_set.tile_width), y - (scale * ui_set.tile_width), scale, 1, false);
    BlitBitmapScaled(gb, &ui_set.tiles[8], x - (scale * ui_set.tile_width), y, scale, 1, false);
    BlitBitmapScaled(gb, &ui_set.tiles[12], x - (scale * ui_set.tile_width), y + (scale * ui_set.tile_width), scale, 1, false);
    for (int c = 0; c < length; ++c) {
        BlitBitmapScaled(gb, &ui_set.tiles[5], x + (scale * c * font.tile_width), y - (scale * ui_set.tile_width), scale, 1, false);
        BlitBitmapScaled(gb, &ui_set.tiles[9], x + (scale * c * font.tile_width), y, scale, 1, false);
        BlitBitmapScaled(gb, &ui_set.tiles[13], x + (scale * c * font.tile_width), y + (scale * ui_set.tile_width), scale, 1, false);
    }
    // SECTION: right border.
    BlitBitmapScaled(gb, &ui_set.tiles[6], x + (scale * ui_set.tile_width * length), y - (scale * ui_set.tile_width), scale, 1, false);
    BlitBitmapScaled(gb, &ui_set.tiles[10], x + (scale * ui_set.tile_width * length), y, scale, 1, false);
    BlitBitmapScaled(gb, &ui_set.tiles[14], x + (scale * ui_set.tile_width * length), y + (scale * ui_set.tile_width), scale, 1, false);
    DisplayString(gb, text, &font, x, y, scale);
}

enum Tool {
    paint = 0,
    erase = 1,
    fill = 2,
    select = 3
};

void Update(GameState* gs, KeyboardState* ks, int dt) {
    Tool curr_tool = paint;
    
    //RenderTextButton(graphics_buffer, "save", 0,0,3);
    int ui_scale = 5;
    
    // SECTION: UI button 
    Rect paint_button = {0};
    paint_button.x = (float)(ui_scale);
    paint_button.y = (float)(ui_scale);
    paint_button.width = (float)(8 * ui_scale);
    paint_button.height = (float)(8 * ui_scale);
    Rect erase_button = {0};
    erase_button.x = (float)(10 * ui_scale);
    erase_button.y = (float)(ui_scale);
    erase_button.width = (float)(8 * ui_scale);
    erase_button.height = (float)(8 * ui_scale);
    Rect fill_button = {0};
    fill_button.x = (float)(19 * ui_scale);
    fill_button.y = (float)(ui_scale);
    fill_button.width = (float)(8 * ui_scale);
    fill_button.height = (float)(8 * ui_scale);
    Rect selection_button = {0};
    selection_button.x = (float)(28 * ui_scale);
    selection_button.y = (float)(ui_scale);
    selection_button.width = (float)(8 * ui_scale);
    selection_button.height = (float)(8 * ui_scale);


    // SECTION: Draw The UI Buttons
    BlitBitmapScaled(graphics_buffer, &ui_set.tiles[3], 1 * ui_scale, 1 * ui_scale, ui_scale, 1, false);     
    BlitBitmapScaled(graphics_buffer, &ui_set.tiles[7], 10 * ui_scale, 1 * ui_scale, ui_scale, 1, false);     
    BlitBitmapScaled(graphics_buffer, &ui_set.tiles[11], 19 * ui_scale, 1 * ui_scale, ui_scale, 1, false);     
    BlitBitmapScaled(graphics_buffer, &ui_set.tiles[15], 28 * ui_scale, 1 * ui_scale, ui_scale, 1, false);     
    
    
     

}


void UserFree() {
  
}

