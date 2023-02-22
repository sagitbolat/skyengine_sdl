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
ImageData enemy_sprite = {0};

void Awake(GameMemory* gm) {
    LoadBitmap(&gm->asset_storage, "ship.bmp", &player_sprite); 
    LoadBitmap(&gm->asset_storage, "bullet.bmp", &bullet_sprite);
    LoadBitmap(&gm->asset_storage, "enemy.bmp", &enemy_sprite);
}

Vector2* bullet_position_pool;
int bullet_pool_size = 15;

Vector2* enemy_position_pool;
int enemy_pool_size = 7;

void Start(GameState* gs, KeyboardState* ks) {
    bullet_position_pool = (Vector2*)malloc(sizeof(Vector2) * bullet_pool_size); 
    for (int b = 0; b < bullet_pool_size; ++b) {
        // NOTE: Set all bullet positions to be negative to start
        bullet_position_pool[b].x = -100;
        bullet_position_pool[b].y = -100;
    }

    enemy_position_pool = (Vector2*)malloc(sizeof(Vector2) * enemy_pool_size);
    for (int e = 0; e < enemy_pool_size; ++e) {
        enemy_position_pool[e].x = -100;
        enemy_position_pool[e].y = -100;
    }
}

int total_time = 0;
float x_pos = SCREEN_W / 2;
float y_pos = SCREEN_H + 10;

void Update(GameState* gs, KeyboardState* ks, int dt) {
    
    // SECTION: Timekeeping and mouse position caching
    total_time += dt;
    //Vector2Int mouse_pos = GetMousePosition();


    // SECTION: Updating enemy pool positions:
    static int curr_enemy_pointer = 0;
    for (int e = 0; e < enemy_pool_size; ++e) {
        // NOTE: increment all enemy positions downward.
        // If bullet is out of screen, set its position to -100.
        // NOTE: the -50 is a value that allows us to spawn enemies offscreen without them being tagged as inactive.
        if (enemy_position_pool[e].x < -50 || enemy_position_pool[e].y < -50) {
            continue;
        }
        enemy_position_pool[e].y += (float)(dt) * 0.5;

        if (enemy_position_pool[e].y < -50 || 
            enemy_position_pool[e].y > SCREEN_H + 10 ||
            enemy_position_pool[e].x < 0 ||
            enemy_position_pool[e].x > SCREEN_W + 10
        ) {
            enemy_position_pool[e].y = -100;
            enemy_position_pool[e].x = -100;
        }
    }
    // SECTION: Resetting enemy pool pointer if its at the end of the pool
    if (curr_enemy_pointer >= enemy_pool_size) {
        curr_enemy_pointer = 0;
    }



    // SECTION: Updating bullet pool positions
    static int curr_bullet_pointer = 0; // POINTER OF THE BULLER POOL 
    for (int b = 0; b < bullet_pool_size; ++b) {
        // NOTE: increment all bullet positions upwards.
        // If bullet is out of screen, set its position to -100.
        if (bullet_position_pool[b].x < 0 || bullet_position_pool[b].y < 0) {
            continue;
        }
        bullet_position_pool[b].y -= (float)(dt) * 1.0;

        if (bullet_position_pool[b].y > SCREEN_H + 10) {
            bullet_position_pool[b].y = -100;
            bullet_position_pool[b].x = -100;
        }
        if (bullet_position_pool[b].y < 0) {
            bullet_position_pool[b].y = -100;
            bullet_position_pool[b].x = -100;
        }
    }
    // SECTION: Resetting bullet pool pointer if its at the end of the pool
    if (curr_bullet_pointer >= bullet_pool_size) {
        curr_bullet_pointer = 0;
    }
    
   

    // SECTION: Logging Keystates
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
    
   

    // SECTION: Enemy Spawning
    const int SPAWNRATE = 400;
    static int spawn_timer = 0;
    spawn_timer += dt;
    if (spawn_timer > SPAWNRATE) {
        spawn_timer -= SPAWNRATE;
        enemy_position_pool[curr_enemy_pointer].x = x_pos;
        enemy_position_pool[curr_enemy_pointer].y = 0;
        curr_enemy_pointer += 1;
    }



    // SECTION: Shooting
    const int FIRERATE = 200; // NOTE: How often the bullet will shoot in ms.
    static int fire_timer = 0;
    fire_timer += dt;
    if (ks->state.SPACE && fire_timer > FIRERATE) {
        fire_timer = 0;
        bullet_position_pool[curr_bullet_pointer].y = y_pos;
        bullet_position_pool[curr_bullet_pointer].x = x_pos;
        curr_bullet_pointer += 1;
    }



    // SECTION: Deactivating bullets and enemies if they collide:
    for (int e = 0; e < enemy_pool_size; ++e) {
        if (enemy_position_pool[e].y < -50 || enemy_position_pool[e].x < -50) {
            continue;
        }
        for (int b = 0; b < bullet_pool_size; ++b) {
            if (bullet_position_pool[b].y < -50 || bullet_position_pool[b].x < -50) {
                continue;
            }
            float ex = enemy_position_pool[e].x;
            float ey = enemy_position_pool[e].y;
            float bx = bullet_position_pool[b].x;
            float by = bullet_position_pool[b].y;
            Rect enemy_collider = {0};
            Rect bullet_collider = {0};
            enemy_collider.x = ex - (4 * 5);
            enemy_collider.y = ey - (4 * 5);
            enemy_collider.width = (8 * 5);
            enemy_collider.height = (8 * 5);
            bullet_collider.x = bx - (1 * 5);
            bullet_collider.y = by - (1 * 5);
            bullet_collider.width = (2 * 5);
            bullet_collider.height = (2 * 5);
            if (RectCollision(enemy_collider, bullet_collider)) {
                enemy_position_pool[e].x = -100;
                enemy_position_pool[e].y = -100;
                bullet_position_pool[b].x = -100;
                bullet_position_pool[b].y = -100;

            }
        }
    }



    // SECTION: Rendering enemies
    for (int e = 0; e < enemy_pool_size; ++e) {
        if (enemy_position_pool[e].y < 0 || enemy_position_pool[e].x < 0) {
            continue;
        }
        int enemy_x = (int)enemy_position_pool[e].x;
        int enemy_y = (int)enemy_position_pool[e].y;
        
        BlitBitmapScaled(graphics_buffer, &enemy_sprite, (int)enemy_x, (int)enemy_y, 5, 1, true);
    }
    // SECTION: Rendering bullets
    for (int b = 0; b < bullet_pool_size; ++b) {
        if (bullet_position_pool[b].y < 0 || bullet_position_pool[b].x < 0) {
            continue;
        }
        int bullet_x = (int)bullet_position_pool[b].x;
        int bullet_y = (int)bullet_position_pool[b].y;

        BlitBitmapScaled(graphics_buffer, &bullet_sprite, (int)bullet_x, (int)bullet_y, 5, 1, true);
    }
    // SECTION: Rendering Player
    BlitBitmapScaled(graphics_buffer, &player_sprite, (int)x_pos, (int)y_pos, 5, 1, true);
    
    
    // SECTION: Print frame deltatime
    printf("%d\n", dt);       
}

void UserFree() {
    free(bullet_position_pool);
    free(enemy_position_pool);    
}
