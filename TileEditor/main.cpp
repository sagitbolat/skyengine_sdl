#include "../Engine/SDL_sky.cpp"
#include "../Engine/bmp.cpp"
#include "../Engine/tilemaps.cpp"
#include "../Engine/collisions.cpp"
#include "stdio.h"


int SCREEN_H = 727;
int SCREEN_W = 1280;

int TILEMAP_WIDTH = 32;
int TILEMAP_HEIGHT = 24;
const char* TILESET_NAME = "assets/tileset.bmp";
int TILE_WIDTH = 8;
int TILE_HEIGHT = 8;


Tileset font = {0};
Tileset ui_set = {0};
Tileset cursor_set = {0};


// SECTION: main
void Init(int* w, int* h) {
    *w = SCREEN_W;
    *h = SCREEN_H;
    SetCursorVisibility(false);
}


GameMemory* _gm;

void Awake(GameMemory* gm) {
    LoadTileset(&gm->asset_storage, "assets/font_light.bmp", 8, 8, &font);
    LoadTileset(&gm->asset_storage, "assets/UI_tileset.bmp", 8, 8, &ui_set);
    LoadTileset(&gm->asset_storage, "assets/cursor_tileset.bmp", 8, 8, &cursor_set);
    _gm = gm;
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

void DrawTilemap(Tilemap* tilemap, Tileset* tileset, RectInt tilemap_panel, int ui_scale, int tilemap_width, int tilemap_height, Color background_color, int tile_scale, int tile_size, bool grid_enabled) {
    
    // NOTE: Panel Background:
    DrawRectangle(graphics_buffer, background_color, tilemap_panel);
    

    for (int tile_y = 0; tile_y < tilemap_height; ++tile_y) {
        int panel_y = tile_y * tile_scale * (tile_size) + tile_scale; // NOTE: The x coordinate inside the panel
        if (panel_y >= tilemap_panel.height) break;
        for (int tile_x = 0; tile_x < tilemap_width; ++tile_x) {
            int panel_x = tile_x * tile_scale * (tile_size) + tile_scale; // NOTE: The x coordinate inside the panel
            if (panel_x + tile_scale * (tile_size + 1) >= tilemap_panel.width) break;
            
            // TODO: Draw the actual tiles.
            RectInt r = {panel_x + tilemap_panel.x, panel_y + tilemap_panel.y, tile_scale * tile_size, tile_scale * tile_size};
            int tile_i = tile_y * tilemap_width + tile_x;
            BlitBitmapScaled(graphics_buffer, &tileset->tiles[tilemap->tile_data[tile_i]], r.x, r.y, tile_scale, 1, false); 
        }
    }
    
    // NOTE: Add grid lines
    if (!grid_enabled) return;
    for (int y = 0; y < tilemap_panel.height; y += tile_scale * tile_size) {
        int panel_y = y + tilemap_panel.y + tile_scale;
        Vector2Int v1 = {0};
        v1.x = tilemap_panel.x;
        v1.y = panel_y;
        Vector2Int v2 = {0};
        v2.x = tilemap_panel.x + tilemap_panel.width;
        v2.y = panel_y;
        DrawLine(graphics_buffer, background_color, v1, v2); 
    }
    for (int x = 0; x < tilemap_panel.width; x += tile_scale * tile_size) {
        int panel_x = x + tilemap_panel.x + tile_scale;
        Vector2Int v1 = {0};
        v1.x = panel_x;
        v1.y = tilemap_panel.y;
        Vector2Int v2 = {0};
        v2.x = panel_x;
        v2.y = tilemap_panel.y + tilemap_panel.height;
        DrawLine(graphics_buffer, background_color, v1, v2); 
    }

}


enum Scene {
    IMPORT_MENU = 0,
    EDITOR = 1
};


static Scene curr_scene = IMPORT_MENU;


Tileset tileset = {0};
Tilemap tilemap = {0};



void Update(GameState* gs, KeyboardState* ks, int dt) {
    
    Vector2Int mouse_pos = GetMousePosition(); 
     
    if (curr_scene == IMPORT_MENU) {
        tilemap.width = TILEMAP_WIDTH;
        tilemap.height = TILEMAP_HEIGHT;
        tilemap.tile_data = (uint8_t*)malloc(sizeof(uint8_t) * tilemap.width * tilemap.height);
        

        for (int y = 0; y < tilemap.height; ++y) {
            for (int x = 0; x < tilemap.width; ++x) {
                tilemap.tile_data[y * tilemap.width + x] = 0;
            }
        }
        
        LoadTileset(&_gm->asset_storage, TILESET_NAME, TILE_WIDTH, TILE_HEIGHT, &tileset);


        curr_scene = EDITOR; 
    } else {
    
        int ui_scale = 5;
        static int tile_scale = 3;
        static bool grid_enabled = true; 
        // SECTION: Display the tilemap Editor Area:
            
        if (ks->state.Q && !ks->prev_state.Q && tile_scale > 1) {
            --tile_scale;
        } else if (ks->state.E && !ks->prev_state.E && tile_scale < 10) {
            ++tile_scale;
        }

        Color background_color = {255, 255, 255, 255};
        RectInt tilemap_panel = {0};
        tilemap_panel.x = ui_scale;
        tilemap_panel.y = ui_scale * 10;
        tilemap_panel.width = 1024; // NOTE: In pixels
        tilemap_panel.height = SCREEN_H - (ui_scale * 11);
        DrawTilemap(&tilemap, &tileset, tilemap_panel, ui_scale, TILEMAP_WIDTH, TILEMAP_HEIGHT, background_color, tile_scale, TILE_WIDTH, grid_enabled);  

         

        // SECTION: Draw the UI
        DrawUI(ks, mouse_pos, ui_scale);


        
        // SECTION: Edit the tilemap:
        static int curr_selected_tile = 1;
        if (ks->state.MBL && curr_tool == PAINT_TOOL) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT);
            
            tile_x = (tile_x >= TILEMAP_WIDTH) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT) ? -1 : tile_y; 
            int tile_i = (tile_y * TILEMAP_WIDTH) + tile_x;
            //NOTE: FOR DEBUGGING ONLY
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the current selected tile type:
                tilemap.tile_data[tile_i] = curr_selected_tile;
            }
        }
        
    } 


}


void UserFree() {
  
}

