#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"
#include "../Engine/sky_random.cpp"
#include "../Engine/collisions.cpp"

#define PIXELS_PER_UNIT 16.0f // NOTE: Number of pixels in one unit of worldspace (used to keep pixel sprites scales consistent)
#include "object_pools.h"



// PLAYER
struct Player {
    Transform transform;
    Sprite sprite;
    int curr_health;
};

Player player = {0};


// SPRITES
Sprite bullet_sprite = {0};
Sprite enemy_sprite = {0};


BulletPool bullet_pool;
const int BULLET_POOL_SIZE = 80;
EnemyPool enemy_pool;
const int ENEMY_POOL_SIZE = 30;

const int PLAYER_MAX_HEALTH = 3;
 
void GameAwake() {
    sky_srand(199213);

    // SECTION: Load Sprites and init player
    player.sprite = LoadSprite("ship.bmp");
    player.transform.position = {0.0f, 0.0f, 0.0f};
    player.transform.rotation = {0.0f, 0.0f, 0.0f};
    // NOTE: Scale the sprite based on its pixel size.
    player.transform.scale = {(float)player.sprite.pixel_width/PIXELS_PER_UNIT, (float)player.sprite.pixel_height/PIXELS_PER_UNIT, 1.0f};

    bullet_sprite = LoadSprite("bullet.bmp");
    
    enemy_sprite = LoadSprite("enemy.bmp");
}


void GameStart(GameState* gs, KeyboardState* ks, double dt) {
    bullet_pool.AllocAndInit(BULLET_POOL_SIZE);  
    enemy_pool.AllocAndInit(ENEMY_POOL_SIZE);  

    player.curr_health = PLAYER_MAX_HEALTH;
}

void GameUpdate(GameState* gs, KeyboardState* ks, double dt) {
    float player_speed = 6.0f;
    float rotation_speed_scaled = 315.0f * ((float)dt / 1000.0f);
    float bullet_speed_scaled = 10.0f * ((float)dt / 1000.0f); 
    float fire_delay = 40.0f; //NOTE: A firedelay of 10 makes the bullet fire every 10 milliseconds. 1000 means every second
    float enemy_speed_scaled = 6.0f * ((float)dt / 1000.0f); 
    float enemy_spawn_delay = 500.0f; //NOTE: A spawndelay of 10 makes an enemy spawn every 10 milliseconds. 1000 means every second
     
    Vector3 player_movement_direction = {};
    
    // SECTION: input    
    if (ks->state.W) {
        player_movement_direction.y += 1.0f;
    }
    if (ks->state.A) {
        player_movement_direction.x -= 1.0f;
    }
    if (ks->state.S) {
        player_movement_direction.y -= 1.0f;
    }
    if (ks->state.D) {
        player_movement_direction.x += 1.0f;
    }
    player_movement_direction = Normalize(player_movement_direction);
    if (!isnan(player_movement_direction.x) && !isnan(player_movement_direction.y)) {
        player.transform.position.x = player.transform.position.x + (player_movement_direction.x * player_speed * ((float)dt / 1000.0f));
        player.transform.position.y = player.transform.position.y + (player_movement_direction.y * player_speed * ((float)dt / 1000.0f));
    }

    
    if (ks->state.Q) {
        player.transform.rotation.z += rotation_speed_scaled;
    }
    if (ks->state.E) {
        player.transform.rotation.z -= rotation_speed_scaled;
    }   



    // SECTION: Firing
    static float fire_timer = 0;
    fire_timer += (float)dt;
    bool reset_timer = false;
    if (ks->state.MBL && fire_timer > fire_delay) {
        //transform.position = transform.position + UpVector(transform) * player_speed;
        bullet_pool.SpawnBullet(player.transform, bullet_sprite);
        reset_timer=true;
    }
    if (ks->state.MBR && fire_timer > fire_delay) {
        //transform.position = transform.position + UpVector(transform) * player_speed;
        bullet_pool.SpawnBullet(player.transform, bullet_sprite);
        reset_timer=true;
    }
    if (reset_timer) {
        fire_timer=0;
        reset_timer = false;
    }
    
    
    static float enemy_spawn_timer = 0;
    enemy_spawn_timer += (float)dt;
    if (enemy_spawn_timer > enemy_spawn_delay) {
        Vector3 spawn_pos = {0.0f, 0.0f, 0.0f};
        // NOTE: Sets a random spawn point on the perimiter of the screen:
        float half_h = main_camera.height / 2;
        float half_w = main_camera.width / 2;
        int side = sky_rand() % 4;
        switch (side) {
            case 0: {
                // Top side of screen
                spawn_pos.x = sky_rand_float(-half_w, half_w);
                spawn_pos.y = half_h;
            } break;
            case 1: {
                // Bottom side of screen
                spawn_pos.x = sky_rand_float(-half_w, half_w);
                spawn_pos.y = -half_h;
            } break;
            case 2: {
                // Left side of screen
                spawn_pos.x = -half_w;
                spawn_pos.y = sky_rand_float(-half_h, half_h);
            } break;
            case 3: {
                // Right side of screen
                spawn_pos.x = half_w;
                spawn_pos.y = sky_rand_float(-half_h, half_h);
            } break;

        }
        spawn_pos.z = -0.5;
        enemy_pool.SpawnEnemy(spawn_pos, player.transform.position, enemy_sprite);
        enemy_spawn_timer = 0;
    }
    
   
   
    bullet_pool.UpdateBulletState(bullet_speed_scaled);    
    enemy_pool.UpdateEnemyState(enemy_speed_scaled);


    // SECTION: Test for collision
    for (int en = 0; en < enemy_pool.pool_size; ++en) {

        if (!enemy_pool.is_active[en]) continue;

        bool player_collision = TestCollision(enemy_pool.data[en], player.transform);
        if (player_collision) {
            // TODO: Handle game over and health here:
            enemy_pool.is_active[en] = false;
            player.curr_health--;
        }

        for (int bul = 0; bul < bullet_pool.pool_size; ++bul) {
            
            if (!bullet_pool.is_active[bul]) continue;

            bool collision = TestCollision(enemy_pool.data[en], bullet_pool.data[bul]);
            
            // NOTE: If colliding with an enemy, handle the collision
            if (!collision) continue;
            // HANDLING IT NOW
            enemy_pool.is_active[en] = false;
            bullet_pool.is_active[bul] = false;
        }
    } 



    // SECTION: Rendering
    for (int i = 0; i < bullet_pool.pool_size; ++i) {
        if (!bullet_pool.is_active[i]) continue;
        DrawSprite(bullet_sprite, bullet_pool.data[i], main_camera);
    }
    for (int i = 0; i < enemy_pool.pool_size; ++i) {
        if (!enemy_pool.is_active[i]) continue;
        DrawSprite(enemy_sprite, enemy_pool.data[i], main_camera);
        //Debug::DrawRectangle2D(enemy_pool.data[i], enemy_sprite.shader_id, main_camera, 3.0f);
    }
    DrawSprite(player.sprite, player.transform, main_camera);
    //Debug::DrawRectangle2D(player.transform, player.sprite.shader_id, main_camera, 3.0f);
    return;
}
void GameClose(GameState* gs, KeyboardState* ks, double dt) {
    return;
}

void GameFree() {
    return;
}