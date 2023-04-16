#include "../Engine/SDL_sky.cpp"
#include "../Engine/bmp.cpp"
#include "../Engine/tilemaps.cpp"
#include "../Engine/collisions.cpp"
#include "stdio.h"


int SCREEN_H = 727;
int SCREEN_W = 1280;

const char* TILEMAP_FILE_NAME = "level.lvl";
int TILEMAP_WIDTH = 16;
int TILEMAP_HEIGHT = 12;
const char* TILESET_NAME = "assets/tileset.bmp";
int TILE_WIDTH = 8;
int TILE_HEIGHT = 8;
int DEFAULT_TILE = 3;    


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

RectInt RenderTextButton(GameBitmapBuffer* gb, const char* text, int x, int y, int scale) {
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
    
    RectInt r = {x, y, (scale * ui_set.tile_width * length) + (2 * scale * 2), (scale * ui_set.tile_width) + (2 * scale * 2)};
    return r;
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
        if (panel_y + tile_scale * (tile_size + 1) >= tilemap_panel.height) break;
        for (int tile_x = 0; tile_x < tilemap_width; ++tile_x) {
            int panel_x = tile_x * tile_scale * (tile_size) + tile_scale; // NOTE: The x coordinate inside the panel
            if (panel_x + tile_scale * (tile_size + 1) >= tilemap_panel.width) break;
            
            // TODO: Draw the actual tiles.
            RectInt r = {panel_x + tilemap_panel.x, panel_y + tilemap_panel.y, tile_scale * tile_size, tile_scale * tile_size};
            int tile_i = tile_y * tilemap_width + tile_x;
            int tile_type = tilemap->tile_data[tile_i];
            if (tile_type == 0) continue;
            else --tile_type;
            BlitBitmapScaled(graphics_buffer, &tileset->tiles[tile_type], r.x, r.y, tile_scale, 1, false); 
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

// Returns the width of the palette window in tiles
int DrawTileset(Tileset* tileset, RectInt tileset_panel, int ui_scale, int tile_scale, Color background_color, int tile_size) {
    // NOTE: Panel Background:
    DrawRectangle(graphics_buffer, background_color, tileset_panel);
    int panel_tile_width = tileset_panel.width / (tile_size * tile_scale);        // NOTE: The width of the panel space in the unit of tiles
    if (panel_tile_width > tileset->width_in_tiles) {
        panel_tile_width = tileset->width_in_tiles;
    }
    int panel_tile_height = tileset->num_tiles / panel_tile_width;
    
    for (int tile_y = 0; tile_y < panel_tile_height; ++tile_y) {
        int panel_y = tile_y * tile_size * tile_scale; // NOTE: The y coordinate inside the panel
        for (int tile_x = 0; tile_x < panel_tile_width; ++tile_x) {
            int panel_x = tile_x * tile_size * tile_scale; // NOTE: The x coordinate inside the panel
            int tile_i = tile_y * panel_tile_width + tile_x;
            Vector2Int v = {panel_x + tileset_panel.x, panel_y + tileset_panel.y};
            BlitBitmapScaled(graphics_buffer, &tileset->tiles[tile_i], v.x, v.y, tile_scale, 1, false); 
        }
    }
    
    // NOTE: Add grid lines
    ///*
    for (int y = 0; y < tileset_panel.height; y += tile_scale * tile_size) {
        int panel_y = y + tileset_panel.y;
        Vector2Int v1 = {0};
        v1.x = tileset_panel.x;
        v1.y = panel_y;
        Vector2Int v2 = {0};
        v2.x = tileset_panel.x + tileset_panel.width;
        v2.y = panel_y;
        DrawLine(graphics_buffer, background_color, v1, v2); 
    }
    for (int x = 0; x < tileset_panel.width; x += tile_scale * tile_size) {
        int panel_x = x + tileset_panel.x;
        Vector2Int v1 = {0};
        v1.x = panel_x;
        v1.y = tileset_panel.y;
        Vector2Int v2 = {0};
        v2.x = panel_x;
        v2.y = tileset_panel.y + tileset_panel.height - tile_scale;
        DrawLine(graphics_buffer, background_color, v1, v2); 
    }
    //*/
    
    return panel_tile_width; 
}


enum Scene {
    IMPORT_MENU = 0,
    EDITOR = 1
};


static Scene curr_scene = IMPORT_MENU;
static bool grid_enable = true;

Tileset tileset = {0};
Tilemap tilemap = {0};


void FillToolAtCoordinate(int tile_x, int tile_y, int target_tile, int fill_tile_type) {
    int tile_i = (tile_y * TILEMAP_WIDTH) + tile_x;
    if (tile_i < 0 || tile_i >= (TILEMAP_WIDTH * TILEMAP_HEIGHT)) return;
    if (tilemap.tile_data[tile_i] != target_tile) return;


    tilemap.tile_data[tile_i] = fill_tile_type;
    FillToolAtCoordinate(tile_x, tile_y+1, target_tile, fill_tile_type);
    FillToolAtCoordinate(tile_x, tile_y-1, target_tile, fill_tile_type);
    FillToolAtCoordinate(tile_x+1, tile_y, target_tile, fill_tile_type);
    FillToolAtCoordinate(tile_x-1, tile_y, target_tile, fill_tile_type);

} 

void Update(GameState* gs, KeyboardState* ks, int dt) {
    
    Vector2Int mouse_pos = GetMousePosition(); 


    if (curr_scene == IMPORT_MENU) {
        tilemap.width = TILEMAP_WIDTH;
        tilemap.height = TILEMAP_HEIGHT;
        tilemap.tile_data = (uint8_t*)malloc(sizeof(uint8_t) * tilemap.width * tilemap.height);
        

        for (int y = 0; y < tilemap.height; ++y) {
            for (int x = 0; x < tilemap.width; ++x) {
                tilemap.tile_data[y * tilemap.width + x] = DEFAULT_TILE;
            }
        }
        
        LoadTileset(&_gm->asset_storage, TILESET_NAME, TILE_WIDTH, TILE_HEIGHT, &tileset);


        curr_scene = EDITOR; 
    } else {
    
        int ui_scale = 5;
        static int tile_scale = 3;
        if (ks->state.Q && !ks->prev_state.Q && tile_scale > 1) {
            --tile_scale;
        } else if (ks->state.E && !ks->prev_state.E && tile_scale < 10) {
            ++tile_scale;
        }

        // SECTION: Display the tilemap Editor Area:
        Color background_color = {255, 255, 255, 255};
        RectInt tilemap_panel = {0};
        tilemap_panel.x = ui_scale;
        tilemap_panel.y = ui_scale * 10;
        tilemap_panel.width = 1024; // NOTE: In pixels
        tilemap_panel.height = SCREEN_H - (ui_scale * 11);
        DrawTilemap(&tilemap, &tileset, tilemap_panel, ui_scale, TILEMAP_WIDTH, TILEMAP_HEIGHT, background_color, tile_scale, TILE_WIDTH, grid_enable);  
        

        // SECTION: Display the tileset palette area:
        int palette_scale = 3;
        RectInt tileset_panel = {0};
        tileset_panel.x = tilemap_panel.x + tilemap_panel.width + ui_scale;
        tileset_panel.y = ui_scale;
        tileset_panel.width = SCREEN_W - ui_scale - tileset_panel.x; // NOTE: In pixels
        tileset_panel.height = SCREEN_H - ui_scale;
        int palette_width = DrawTileset(&tileset, tileset_panel, ui_scale, palette_scale, background_color, TILE_WIDTH);

        
        
        // SECTION: Display Save Button
        RectInt save_button_collider = RenderTextButton(graphics_buffer, "SAVE", ui_scale * 8 * 4 + ui_scale * 5, 0, ui_scale - 1);
        
        
        
        
        // SECTION: Draw the UI
        DrawUI(ks, mouse_pos, ui_scale);

        
        // SECTION: Grid Controls 
        if (ks->state.G && !ks->prev_state.G) {
            grid_enable = !grid_enable;
        }


        // SECTION: Select a tile. 
        static int curr_selected_tile = 3;
        if (ks->state.MBL) {
            int tile_x = (mouse_pos.x - tileset_panel.x - palette_scale)/(palette_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tileset_panel.y - palette_scale)/(palette_scale*TILE_HEIGHT);
            
            int palette_height = tileset.num_tiles / palette_width;

            tile_x = (tile_x >= palette_width)? -1 : tile_x;
            tile_y = (tile_y >= palette_height)? -1 : tile_y;

            int tile_i = (tile_y * palette_width) + tile_x;
            //NOTE: FOR DEBUGGING ONLY
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the current selected tile type:
                curr_selected_tile = tile_i;
            }
        }



        // SECTION: Edit the tilemap:
        // NOTE: Paint Tool
        if (ks->state.MBL && curr_tool == PAINT_TOOL) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT);
            
            tile_x = (tile_x >= TILEMAP_WIDTH || mouse_pos.x - tilemap_panel.x < 0) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT || mouse_pos.y - tilemap_panel.y < 0)  ? -1 : tile_y; 
            int tile_i = (tile_y * TILEMAP_WIDTH) + tile_x;
            //NOTE: FOR DEBUGGING ONLY
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the current selected tile type:
                tilemap.tile_data[tile_i] = curr_selected_tile + 1;
            }
        }
        // NOTE: Erase Tool
        if (ks->state.MBL && curr_tool == ERASE_TOOL) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT);
            
            tile_x = (tile_x >= TILEMAP_WIDTH || mouse_pos.x - tilemap_panel.x < 0) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT || mouse_pos.y - tilemap_panel.y < 0)  ? -1 : tile_y; 
            int tile_i = (tile_y * TILEMAP_WIDTH) + tile_x;
            
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the empty tile (0 value):
                tilemap.tile_data[tile_i] = 0;
            }
        }
        // NOTE: Fill Tool
        if (ks->state.MBL && curr_tool == FILL_TOOL) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT);
            
            tile_x = (tile_x >= TILEMAP_WIDTH || mouse_pos.x - tilemap_panel.x < 0) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT || mouse_pos.y - tilemap_panel.y < 0)  ? -1 : tile_y; 
            int tile_i = (tile_y * TILEMAP_WIDTH) + tile_x;
            
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the empty tile (0 value):
                int target_tile = tilemap.tile_data[tile_i];
                if (target_tile != curr_selected_tile + 1) {
                    FillToolAtCoordinate(tile_x, tile_y, target_tile, curr_selected_tile + 1); 
                }
            }
        }


         
        // SECTION: Do tilemap saving if save button is pressed.
        if (ks->state.MBL && !ks->prev_state.MBL && PointRectCollision(save_button_collider, mouse_pos)) {
            // TODO: Export the current tilemap
            FILE* file = fopen(TILEMAP_FILE_NAME, "w");
            uint32_t width = (uint32_t)TILEMAP_WIDTH;
            uint32_t height = (uint32_t)TILEMAP_HEIGHT;
            fwrite(&width, 4, 1, file);
            fwrite(&height, 4, 1, file);
            
            for (int y = 0; y < tilemap.height; ++y) {
                for (int x = 0; x < tilemap.width; ++x) {
                    int i = (y * tilemap.width) + x;
                    fwrite(&(tilemap.tile_data[i]), 1, 1, file); 
                }
            }
            printf("Saved tilemap");
            fclose(file); 
        }
        
    } 
    

}


void UserFree() {
  
}

