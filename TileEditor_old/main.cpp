#include "../Engine/SDL_sky.cpp"
#include "../Engine/bmp.cpp"
#include "../Engine/tilemaps.cpp"
#include "../Engine/collisions.cpp"
#include "stdio.h"

#include "globals.h"


Tileset font = {0};
Tileset ui_set = {0};
Tileset cursor_set = {0};


// SECTION: main
void Init(int* w, int* h, bool* fullscreen) {
    *w = SCREEN_W;
    *h = SCREEN_H;
	*fullscreen = 0;
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
    COLLIDER_TOOL = 3,
    NULL_TOOL = 4
};


static Tool curr_tool = NULL_TOOL;


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
    Rect collider_button = {0};
    collider_button.x = (float)(28 * ui_scale);
    collider_button.y = (float)(ui_scale);
    collider_button.width = (float)(8 * ui_scale);
    collider_button.height = (float)(8 * ui_scale);


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
    } else if (ks->state.MBL && PointRectCollision(collider_button, mouse_pos)) {
        curr_tool = COLLIDER_TOOL;  
    }
    
    // SECTION: Display the correct cursor sprite:
    if (curr_tool == PAINT_TOOL) {
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[0], mouse_pos.x, mouse_pos.y, 2, 1, true);     
    } else if (curr_tool == ERASE_TOOL) {
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[1], mouse_pos.x, mouse_pos.y, 2, 1, true);     
    } else if (curr_tool == FILL_TOOL) {
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[2], mouse_pos.x, mouse_pos.y, 2, 1, true);     
    } else if (curr_tool == COLLIDER_TOOL) {
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[3], mouse_pos.x, mouse_pos.y, 2, 1, true);     
    } else if (curr_tool == NULL_TOOL) {
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[0], mouse_pos.x, mouse_pos.y, 2, 1, true);     
    }
}


void DrawColliders(Tilemap* tilemap, RectInt tilemap_panel, int x_offset, int y_offset, int tile_scale, int tile_size) {
    int tilemap_height = tilemap->height;
    int tilemap_width = tilemap->width;
    
    for (int tile_y = y_offset; tile_y < tilemap_height; ++tile_y) {
        int panel_y = (tile_y - y_offset) * tile_scale * (tile_size) + tile_scale; // NOTE: The x coordinate inside the panel
        if (panel_y + tile_scale * (tile_size + 1) >= tilemap_panel.height) break;
        for (int tile_x = x_offset; tile_x < tilemap_width; ++tile_x) {
            int panel_x = (tile_x - x_offset) * tile_scale * (tile_size) + tile_scale; // NOTE: The x coordinate inside the panel
            if (panel_x + tile_scale * (tile_size + 1) >= tilemap_panel.width) break;
            
            // TODO: Draw the actual tiles.
            RectInt r = {panel_x + tilemap_panel.x, panel_y + tilemap_panel.y, tile_scale * tile_size, tile_scale * tile_size};
            int collider_type = GetTilemapCollider(tilemap, tile_x, tile_y);
            if (collider_type == 0) continue;
            Color c = {0};
            c.red = 0;
            c.green = 255;
            c.blue = 0;
            c.alpha = 127;
            DrawRectangle(graphics_buffer, c, r); 
        }
    }

}



void DrawTilemap(Tilemap* tilemap, Tileset* tileset, RectInt tilemap_panel, int x_offset, int y_offset, int ui_scale, Color background_color, Color gridline_color, int tile_scale, int tile_size, bool grid_enabled, int current_layer, bool show_all_layers) {
     
    // NOTE: Panel Background:
    DrawRectangle(graphics_buffer, background_color, tilemap_panel);
    int tilemap_width = tilemap->width;
    int tilemap_height = tilemap->height;

    for (int tile_y = y_offset; tile_y < tilemap_height; ++tile_y) {
        int panel_y = (tile_y - y_offset) * tile_scale * (tile_size) + tile_scale; // NOTE: The x coordinate inside the panel
        if (panel_y + tile_scale * (tile_size + 1) >= tilemap_panel.height) break;
        for (int tile_x = x_offset; tile_x < tilemap_width; ++tile_x) {
            int panel_x = (tile_x-x_offset) * tile_scale * (tile_size) + tile_scale; // NOTE: The x coordinate inside the panel
            if (panel_x + tile_scale * (tile_size + 1) >= tilemap_panel.width) break;
            
            // TODO: Draw the actual tiles.
            RectInt r = {panel_x + tilemap_panel.x, panel_y + tilemap_panel.y, tile_scale * tile_size, tile_scale * tile_size};
            for (int tile_l = 0; tile_l < tilemap->num_layers; ++tile_l) {
                if (!show_all_layers && tile_l != current_layer) continue;
                int tile_type = GetTilemapTile(tilemap, tile_x, tile_y, tile_l);
                if (tile_type == 0) continue;
                else --tile_type;
                BlitBitmapScaled(graphics_buffer, &tileset->tiles[tile_type], r.x, r.y, tile_scale, 1, false); 
            }
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
        DrawLine(graphics_buffer, gridline_color, v1, v2); 
    }
    for (int x = 0; x < tilemap_panel.width; x += tile_scale * tile_size) {
        int panel_x = x + tilemap_panel.x + tile_scale;
        Vector2Int v1 = {0};
        v1.x = panel_x;
        v1.y = tilemap_panel.y;
        Vector2Int v2 = {0};
        v2.x = panel_x;
        v2.y = tilemap_panel.y + tilemap_panel.height;
        DrawLine(graphics_buffer, gridline_color, v1, v2); 
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

    
    return panel_tile_width; 
}


enum Scene {
    IMPORT_MENU = 0,
    EDITOR = 1
};


static Scene curr_scene = IMPORT_MENU;
static bool grid_enable = true;
static bool colliders_enable = true;


static int current_layer = 0;
static bool show_all_layers = true;


Tileset tileset = {0};
Tilemap tilemap;

int TILEMAP_PANEL_WIDTH = 1024;

void FillToolAtCoordinate(int tile_x, int tile_y, int tile_l, int target_tile, int fill_tile_type) {
    int tile_i = ((tile_y * TILEMAP_WIDTH) + tile_x)*NUMBER_OF_LAYERS + tile_l;
    if (tile_i < 0 || tile_i >= (TILEMAP_WIDTH * TILEMAP_HEIGHT * NUMBER_OF_LAYERS)) return;
    if (tilemap.tile_data[tile_i] != target_tile) return;


    tilemap.tile_data[tile_i] = fill_tile_type;
    FillToolAtCoordinate(tile_x, tile_y+1, tile_l, target_tile, fill_tile_type);
    FillToolAtCoordinate(tile_x, tile_y-1, tile_l, target_tile, fill_tile_type);
    FillToolAtCoordinate(tile_x+1, tile_y, tile_l, target_tile, fill_tile_type);
    FillToolAtCoordinate(tile_x-1, tile_y, tile_l, target_tile, fill_tile_type);

} 

void Update(GameState* gs, KeyboardState* ks, int dt) {
    
    Vector2Int mouse_pos = GetMousePosition(); 

    if (curr_scene == IMPORT_MENU) {
        static bool new_tilemap = false;
        static bool load_tilemap = false;
        // SECTION: set up and render import menu buttons 
        RectInt new_button_collider = RenderTextButton(graphics_buffer, "New Tilemap", SCREEN_W / 2 - 200, SCREEN_H/2 - 70, 5);
        RectInt load_button_collider = RenderTextButton(graphics_buffer, "Load Tilemap", SCREEN_W / 2 - 220, SCREEN_H/2 + 40, 5);

        
        // SECTION: Render a cursor:
        BlitBitmapScaled(graphics_buffer, &cursor_set.tiles[0], mouse_pos.x, mouse_pos.y, 2, 1, true);     
        
        // SECTION: Handle button clicks:
        if (ks->state.MBL && PointRectCollision(new_button_collider, mouse_pos)) {
            new_tilemap=true;
        } 
        if (ks->state.MBL && PointRectCollision(load_button_collider, mouse_pos)) {
            load_tilemap=true;
        } 
        

        // SECTION: Load tilemap
        if (load_tilemap) {
            tilemap = LoadTilemap(LEVEL_NAME); 
            TILEMAP_WIDTH = tilemap.width;
            TILEMAP_HEIGHT = tilemap.height; 
            NUMBER_OF_LAYERS = tilemap.num_layers;
            printf("Loading tileset...\n");
            LoadTileset(&_gm->asset_storage, TILESET_NAME, TILE_WIDTH, TILE_HEIGHT, &tileset);
            curr_scene = EDITOR; 
        }

        // SECTION: New tilemap: 
        if (new_tilemap) {
            tilemap.width = TILEMAP_WIDTH;
            tilemap.height = TILEMAP_HEIGHT;
            tilemap.num_layers = NUMBER_OF_LAYERS;
            tilemap.tile_data = (uint16_t*)malloc(sizeof(uint16_t) * tilemap.width * tilemap.height * tilemap.num_layers);
            tilemap.collider_data = (uint8_t*)malloc(sizeof(uint8_t) * tilemap.width * tilemap.height); 

            for (int y = 0; y < tilemap.height; ++y) {
                for (int x = 0; x < tilemap.width; ++x) {
                    for (int l = 0; l < tilemap.num_layers; ++l) {
                        SetTilemapTile(&tilemap, x, y, l, DEFAULT_TILE); 
                    }
                }
            }
            
            for (int y = 0; y < tilemap.height; ++y) {
                for (int x = 0; x < tilemap.width; ++x) {
                    SetTilemapCollider(&tilemap, x, y, 0); 
                }
            }
            
            printf("Loading tileset...\n");
            LoadTileset(&_gm->asset_storage, TILESET_NAME, TILE_WIDTH, TILE_HEIGHT, &tileset);


            curr_scene = EDITOR; 
        }
    } else {
        static int tilemap_x_offset = 0;
        static int tilemap_y_offset = 0; 
        int ui_scale = 5;
        static int tile_scale = 3;
        
        // SECTION: Display the tilemap Editor Area:
        Color background_color = {255, 255, 255, 255};
        Color gridline_color = {155, 155, 155, 255};
        RectInt tilemap_panel = {0};
        tilemap_panel.x = ui_scale;
        tilemap_panel.y = ui_scale * 10;
        tilemap_panel.width = TILEMAP_PANEL_WIDTH; // NOTE: In pixels
        tilemap_panel.height = SCREEN_H - (ui_scale * 11);
        DrawTilemap(&tilemap, &tileset, tilemap_panel, tilemap_x_offset, tilemap_y_offset, ui_scale, background_color, gridline_color, tile_scale, TILE_WIDTH, grid_enable, current_layer, show_all_layers);  
        

        // SECTION: Render the colliders on top of the tilemap. 
        if (colliders_enable) { 
            DrawColliders(&tilemap, tilemap_panel, tilemap_x_offset, tilemap_y_offset, tile_scale, TILE_WIDTH); 
        }
            
        // SECTION: Display the tileset palette area:
        static int palette_scale = 1;
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
        // NOTE: Display the current layer
        char layer_string[124];
        snprintf(layer_string, 124, "Layer %d", current_layer);
        DisplayString(graphics_buffer, layer_string,&font, 340, 15, 3);  
               

        // SECTION: Zoom Controls
        // NOTE: map zoom
        if (ks->state.Q && !ks->prev_state.Q && tile_scale > 1) {
            --tile_scale;
        } else if (ks->state.E && !ks->prev_state.E && tile_scale < 10) {
            ++tile_scale;
        }
        // NOTE: palette zoom
        if (ks->state.C && !ks->prev_state.C && palette_scale > 1) {
            --palette_scale;
        } else if (ks->state.V && !ks->prev_state.V && tile_scale < 5) {
            ++palette_scale;
        }

        
        // SECTION: Grid Controls 
        if (ks->state.G && !ks->prev_state.G) {
            grid_enable = !grid_enable;
        }
        
        // SECTION: Collider display Controls
        if (ks->state.W && !ks->prev_state.W) {
            colliders_enable = !colliders_enable;
        }


        //SECTION: Offset controls
        if (ks->state.I && !ks->prev_state.I && tilemap_y_offset > 0) {
            --tilemap_y_offset;
        }
        if (ks->state.J && !ks->prev_state.J && tilemap_x_offset > 0) {
            --tilemap_x_offset;
        }
        if (ks->state.K && !ks->prev_state.K && tilemap_y_offset < tilemap.height) {
            ++tilemap_y_offset;
        }
        if (ks->state.L && !ks->prev_state.L && tilemap_x_offset < tilemap.width) {
            ++tilemap_x_offset;
        }
        
        

        // SECTION: Layer controls
        if (ks->state.A && !ks->prev_state.A && current_layer > 0) {
            --current_layer;
        }
        if (ks->state.D && !ks->prev_state.D && current_layer < NUMBER_OF_LAYERS - 1) {
            ++current_layer;
        }
        if (ks->state.S && !ks->prev_state.S) {
            show_all_layers = !show_all_layers;
        }
        
        
        // SECTION: Panel Control
        if (ks->state.Z && !ks->prev_state.Z && TILEMAP_PANEL_WIDTH - TILE_WIDTH*palette_scale > 512) {
            TILEMAP_PANEL_WIDTH -= TILE_WIDTH * palette_scale;
        }
        if (ks->state.X && !ks->prev_state.X && TILEMAP_PANEL_WIDTH + TILE_WIDTH*palette_scale < 1048) {
            TILEMAP_PANEL_WIDTH += TILE_WIDTH * palette_scale;
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
        if (ks->state.MBL && curr_tool == PAINT_TOOL && PointRectCollision(tilemap_panel, mouse_pos)) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH) + tilemap_x_offset;
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT) + tilemap_y_offset;
            
            tile_x = (tile_x >= TILEMAP_WIDTH || mouse_pos.x - tilemap_panel.x < 0) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT || mouse_pos.y - tilemap_panel.y < 0)  ? -1 : tile_y; 
            //NOTE: FOR DEBUGGING ONLY
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the current selected tile type:
                SetTilemapTile(&tilemap, tile_x, tile_y, current_layer, curr_selected_tile+1); 
            }
        }
        // NOTE: Erase Tool
        if (ks->state.MBL && curr_tool == ERASE_TOOL && PointRectCollision(tilemap_panel, mouse_pos)) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT);
            
            tile_x = (tile_x >= TILEMAP_WIDTH || mouse_pos.x - tilemap_panel.x < 0) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT || mouse_pos.y - tilemap_panel.y < 0)  ? -1 : tile_y; 
            
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the empty tile (0 value):
                SetTilemapTile(&tilemap, tile_x, tile_y, current_layer, 0); 
            }
        }
        if (ks->state.MBR && curr_tool == ERASE_TOOL && PointRectCollision(tilemap_panel, mouse_pos)) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT);
            
            tile_x = (tile_x >= TILEMAP_WIDTH || mouse_pos.x - tilemap_panel.x < 0) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT || mouse_pos.y - tilemap_panel.y < 0)  ? -1 : tile_y; 
            
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the empty tile (0 value):
                int target_tile = GetTilemapTile(&tilemap, tile_x, tile_y, current_layer);
                if (target_tile != 0) {
                    FillToolAtCoordinate(tile_x, tile_y, current_layer, target_tile, 0); 
                }
            }
        }
        // NOTE: Fill Tool
        if (ks->state.MBL && curr_tool == FILL_TOOL && PointRectCollision(tilemap_panel, mouse_pos)) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT);
            
            tile_x = (tile_x >= TILEMAP_WIDTH || mouse_pos.x - tilemap_panel.x < 0) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT || mouse_pos.y - tilemap_panel.y < 0)  ? -1 : tile_y; 
            
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the empty tile (0 value):
                int target_tile = GetTilemapTile(&tilemap, tile_x, tile_y, current_layer);
                if (target_tile != curr_selected_tile + 1) {
                    FillToolAtCoordinate(tile_x, tile_y, current_layer, target_tile, curr_selected_tile + 1); 
                }
            }
        }
        // NOTE: Collider Tool
        if (ks->state.MBL && curr_tool == COLLIDER_TOOL && PointRectCollision(tilemap_panel, mouse_pos)) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT);
            
            tile_x = (tile_x >= TILEMAP_WIDTH || mouse_pos.x - tilemap_panel.x < 0) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT || mouse_pos.y - tilemap_panel.y < 0)  ? -1 : tile_y; 
            //NOTE: FOR DEBUGGING ONLY
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the current selected tile type:
                SetTilemapCollider(&tilemap, tile_x, tile_y, 1); 
            }
        }
        if (ks->state.MBR && curr_tool == COLLIDER_TOOL && PointRectCollision(tilemap_panel, mouse_pos)) {
            int tile_x = (mouse_pos.x - tilemap_panel.x - tile_scale)/(tile_scale*TILE_WIDTH);
            int tile_y = (mouse_pos.y - tilemap_panel.y - tile_scale)/(tile_scale*TILE_HEIGHT);
            
            tile_x = (tile_x >= TILEMAP_WIDTH || mouse_pos.x - tilemap_panel.x < 0) ? -1 : tile_x; 
            tile_y = (tile_y >= TILEMAP_HEIGHT || mouse_pos.y - tilemap_panel.y < 0)  ? -1 : tile_y; 
            //NOTE: FOR DEBUGGING ONLY
            //char coords[124];
            //snprintf(coords, 124, "{%d, %d}, %d", tile_x, tile_y, tile_i);
            //DisplayString(graphics_buffer, coords, &font, 800, 680, 5);

            if (tile_x >= 0 && tile_y >= 0) {
                // NOTE: Change that tile to the current selected tile type:
                SetTilemapCollider(&tilemap, tile_x, tile_y, 0); 
            }
        }


         
        // SECTION: Do tilemap saving if save button is pressed.
        if (ks->state.MBL && !ks->prev_state.MBL && PointRectCollision(save_button_collider, mouse_pos)) {
            // TODO: Export the current tilemap
            FILE* file = fopen(TILEMAP_FILE_NAME, "w");
            uint32_t width = (uint32_t)TILEMAP_WIDTH;
            uint32_t height = (uint32_t)TILEMAP_HEIGHT;
            uint32_t num_layers = (uint32_t)NUMBER_OF_LAYERS;
            uint32_t padding = (uint32_t)0;
            fwrite(&width, 4, 1, file);
            fwrite(&height, 4, 1, file);
            fwrite(&num_layers, 4, 1, file); 
            fwrite(&padding, 4, 1, file);
            
            int last_x = 0;
            int last_y = 0;
            // NOTE: Write tilemap sprite data
            for (int y = 0; y < tilemap.height; ++y) {
                last_y = y;
                for (int x = 0; x < tilemap.width; ++x) {
                    last_x = x;
                    for (int l = 0; l < tilemap.num_layers; ++l) {
                        uint16_t tile_type = GetTilemapTile(&tilemap, x, y, l) + 27; 
                        fwrite(&(tile_type), 2, 1, file); 
                    }
                }
            }

            printf("%d, %d", last_x, last_y);

            // NOTE: Write tilemap collider data
            for (int y = 0; y < tilemap.height; ++y) {
                for (int x = 0; x < tilemap.width; ++x) {
                    uint8_t collider_type = GetTilemapCollider(&tilemap, x, y);
                    fwrite(&(collider_type), 1, 1, file); 
                }
            }

            
            printf("Saved tilemap \n");
            fclose(file); 
        }
        
    } 
    

}


void UserFree() {
  
}

