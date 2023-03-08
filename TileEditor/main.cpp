#include "../Engine/SDL_sky.cpp"
#include "../Engine/bmp.cpp"
#include "../Engine/tilemaps.cpp"
#include "../Engine/collisions.cpp"
#include "stdio.h"


int SCREEN_H = 727;
int SCREEN_W = 1280;


Tileset font = {0};
Tileset ui_set = {0};
Tileset cursor_set = {0};

// SECTION: main
void Init(int* w, int* h) {
    *w = SCREEN_W;
    *h = SCREEN_H;
    SetCursorVisibility(false);
}




void Awake(GameMemory* gm) {
    LoadTileset(&gm->asset_storage, "assets/font_light.bmp", 8, 8, &font);
    LoadTileset(&gm->asset_storage, "assets/UI_tileset.bmp", 8, 8, &ui_set);
    LoadTileset(&gm->asset_storage, "assets/cursor_tileset.bmp", 8, 8, &cursor_set);

    
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
    PAINT_TOOL = 0,
    ERASE_TOOL = 1,
    FILL_TOOL = 2,
    SELECTION_TOOL = 3
};


static Tool curr_tool = PAINT_TOOL;


void DrawUI(KeyboardState* ks, Vector2Int mouse_pos, int ui_scale) {
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
    
    
    // SECTION: Check For Button Collision
    if (ks->state.MBL && PointRectCollision(paint_button, mouse_pos)) {
        curr_tool = PAINT_TOOL;  
    } else if (ks->state.MBL && PointRectCollision(erase_button, mouse_pos)) {
        curr_tool = ERASE_TOOL;  
    } else if (ks->state.MBL && PointRectCollision(fill_button, mouse_pos)) {
        curr_tool = FILL_TOOL;  
    } else if (ks->state.MBL && PointRectCollision(selection_button, mouse_pos)) {
        curr_tool = SELECTION_TOOL;  
    }
    
    // SECTION: Display the correct cursor sprite:
    if (curr_tool == PAINT_TOOL) {
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[0], mouse_pos.x, mouse_pos.y, 2, 1, true);     
    } else if (curr_tool == ERASE_TOOL) {
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[1], mouse_pos.x, mouse_pos.y, 2, 1, true);     
    } else if (curr_tool == FILL_TOOL) {
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[2], mouse_pos.x, mouse_pos.y, 2, 1, true);     
    } else if (curr_tool == SELECTION_TOOL) {
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[3], mouse_pos.x, mouse_pos.y, 2, 1, true);     
    }
}

void DrawTilemap(int ui_scale, int tilemap_width, int tilemap_height, Color background_color, int tile_scale, int tile_size) {
    RectInt tilemap_panel = {0};
    tilemap_panel.x = ui_scale;
    tilemap_panel.y = ui_scale * 10;
    tilemap_panel.width = 1024; // NOTE: In pixels
    tilemap_panel.height = SCREEN_H - (ui_scale * 11);
    
    // NOTE: Panel Background:
    DrawRectangle(graphics_buffer, background_color, tilemap_panel);
    

    for (int tile_y = 0; tile_y < tilemap_height; ++tile_y) {
        int panel_y = tile_y * tile_scale * (tile_size + 1) + tile_scale; // NOTE: The x coordinate inside the panel
        if (panel_y >= tilemap_panel.height) break;
        for (int tile_x = 0; tile_x < tilemap_width; ++tile_x) {
            int panel_x = tile_x * tile_scale * (tile_size + 1) + tile_scale; // NOTE: The x coordinate inside the panel
            if (panel_x + tile_scale * (tile_size + 1) >= tilemap_panel.width) break;
            
            // TODO: Draw the actual tiles.
            RectInt r = {panel_x + tilemap_panel.x, panel_y + tilemap_panel.y, tile_scale * tile_size, tile_scale * tile_size};
            Color c = {255, 140, 140, 140};
            DrawRectangle(graphics_buffer, c, r);
        }
    }


}

void Update(GameState* gs, KeyboardState* ks, int dt) {
    
    Vector2Int mouse_pos = GetMousePosition(); 
     
     
    int ui_scale = 5;
    
    // SECTION: Display the tilemap Editor Area:
    static int tile_scale = 3;
    
    if (ks->state.Q && !ks->prev_state.Q && tile_scale < 5) {
        ++tile_scale;
    } else if (ks->state.E && !ks->prev_state.E && tile_scale > 0) {
        --tile_scale;
    }

    Color background_color = {255, 255, 255, 255};
    DrawTilemap(ui_scale, 32, 24, background_color, tile_scale, 8);  

     
     
    // SECTION: Draw the UI
    DrawUI(ks, mouse_pos, ui_scale); 
    


}


void UserFree() {
  
}

