#include "Engine/SDL_sky.cpp"
#include "Engine/bmp.cpp"
#include "Engine/tilemaps.cpp"
#include "Engine/collisions.cpp"



int SCREEN_H = 720;
int SCREEN_W = 1296;

// SECTION: main
void Init(int* w, int* h) {
    *w = SCREEN_W;
    *h = SCREEN_H;
}


ImageData player_sprite = {0};
ImageData bullet_sprite = {0};

void Awake(GameMemory* gm) {
    LoadBitmap(&gm->asset_storage, "ship.bmp", &player_sprite); 
    LoadBitmap(&gm->asset_storage, "bullet.bmp", &bullet_sprite);
}

Vector2Int* bullet_position_pool;
int bullet_pool_size = 30;

void Start(GameState* gs, KeyboardState* ks) {
    bullet_position_pool = (Vector2Int*)malloc(sizeof(Vector2Int) * bullet_pool_size * 2); 
    for (int b = 0; b < bullet_pool_size; ++b) {
        // NOTE: Set all bullet positions to be negative to start
        bullet_position_pool[b].x = -100;
        bullet_position_pool[b].y = -100;
    }
}

int total_time = 0;
float x_pos = SCREEN_W / 2;
float y_pos = SCREEN_H + 10;

void Update(GameState* gs, KeyboardState* ks, int dt) {

    total_time += dt;
    
    Vector2Int mouse_pos = GetMousePosition();

    static int curr_bullet_pointer = 0; // POINTER OF THE BULLER POOL 
    for (int b = 0; b < bullet_pool_size; ++b) {
        // NOTE: increment all bullet positions upwards.
        // If bullet is out of screen, set its position to -100.
        if (bullet_position_pool[b].x < 0 || bullet_position_pool[b].y < 0) {
            continue;
        }
        bullet_position_pool[b].y -= dt * 2;

        if (bullet_position_pool[b].y > SCREEN_H + 10) {
            bullet_position_pool[b].y = -100;
            bullet_position_pool[b].x = -100;
        }
        if (bullet_position_pool[b].y < 0) {
            bullet_position_pool[b].y = -100;
            bullet_position_pool[b].x = -100;
        }
    }
    


    if (ks->state.S && y_pos < SCREEN_H) {
        y_pos += dt * 1;
    }
    if (ks->state.D && x_pos < SCREEN_W) {
        x_pos += dt * 1;
    }
    if (ks->state.W && y_pos > 0) {
        y_pos -= dt * 1;
    }
    if (ks->state.A && x_pos > 0) {
        x_pos -= dt * 1;
    }
    

    // NOTE: Shooting
    const int FIRERATE = 200; // NOTE: How often the bullet will shoot in ms.
    static int fire_timer = 0;
    fire_timer += dt;
    if (ks->state.SPACE && fire_timer > FIRERATE) {
        fire_timer = 0;
        bullet_position_pool[curr_bullet_pointer].y = y_pos;
        bullet_position_pool[curr_bullet_pointer].x = x_pos;
        curr_bullet_pointer += 1;
    }
    
    if (curr_bullet_pointer >= bullet_pool_size) {
        curr_bullet_pointer = 0;
    }

    for (int b = 0; b < bullet_pool_size; ++b) {
        if (bullet_position_pool[b].y < 0 || bullet_position_pool[b].x < 0) {
            continue;
        }
        int bullet_x = bullet_position_pool[b].x;
        int bullet_y = bullet_position_pool[b].y;
        

        BlitBitmapScaled(graphics_buffer, &bullet_sprite, (int)bullet_x, (int)bullet_y, 2, 1, true);
    }
    BlitBitmapScaled(graphics_buffer, &player_sprite, (int)x_pos, (int)y_pos, 5, 1, true);
    
    

    // SECTION: Collision test
    Rect ship_collider = {0};
    ship_collider.x = (float)(x_pos - (8 * 5));
    ship_collider.y = (float)(y_pos - (8 * 5));
    ship_collider.width = (float)(16*5);
    ship_collider.height = (float)(16*5);

    Vector2 mouse_pos_float = {0};
    mouse_pos_float.x = (float)mouse_pos.x;
    mouse_pos_float.y = (float)mouse_pos.y;
    
    if (PointRectCollision(ship_collider, mouse_pos_float)) {
        printf("Mouse over ship\n");
    }
}

void UserFree() {
        
}
