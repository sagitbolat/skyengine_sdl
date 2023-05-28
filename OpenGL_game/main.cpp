#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"
#include "../Engine/sky_random.cpp"


#define PIXELS_PER_UNIT 16.0f // NOTE: Number of pixels in one unit of worldspace (used to keep pixel sprites scales consistent)


void Init(int* w, int* h, float* w_in_world_space, bool* fullscreen, fColor* clear_color) {
    //*w = 640;
    //*h = 480;
    *w_in_world_space = 15.0f;
    *clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
}

Sprite sprite = {0};
Transform transform = {0.0f};

Sprite bullet_sprite = {0};
Sprite enemy_sprite = {0};


void Awake(GameMemory* gm) {
    sky_srand(199213);
    // SECTION: Load Ship Sprite
    sprite = LoadSprite("ship.bmp");
    transform.position = {0.0f, 0.0f, 0.0f};
    transform.rotation = {0.0f, 0.0f, 0.0f};
    // NOTE: Scale the sprite based on its pixel size.
    transform.scale = {(float)sprite.pixel_width/PIXELS_PER_UNIT, (float)sprite.pixel_height/PIXELS_PER_UNIT, 1.0f};


    bullet_sprite = LoadSprite("bullet.bmp");
    
    enemy_sprite = LoadSprite("enemy.bmp");
}


#include "object_pools.h"


BulletPool bullet_pool;
const int BULLET_POOL_SIZE = 80;
EnemyPool enemy_pool;
const int ENEMY_POOL_SIZE = 30;


void Start(GameState* gs, KeyboardState* ks) {
    bullet_pool.AllocAndInit(BULLET_POOL_SIZE);  
    enemy_pool.AllocAndInit(ENEMY_POOL_SIZE);  
}

void Update(GameState* gs, KeyboardState* ks, double dt) {
    float player_speed = 6.0f;
    float rotation_speed_scaled = 315.0f * ((float)dt / 1000.0f);
    float bullet_speed_scaled = 10.0f * ((float)dt / 1000.0f); 
    float fire_delay = 40.0f; //NOTE: A firedelay of 10 makes the bullet fire every 10 milliseconds. 1000 means every second
    float enemy_speed_scaled = 6.0f * ((float)dt / 1000.0f); 
    float enemy_spawn_delay = 500.0f; //NOTE: A spawndelay of 10 makes an enemy spawn every 10 milliseconds. 1000 means every second
     
    Vector3 player_movement_direction = {};
    
    
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
        transform.position.x = transform.position.x + (player_movement_direction.x * player_speed * ((float)dt / 1000.0f));
        transform.position.y = transform.position.y + (player_movement_direction.y * player_speed * ((float)dt / 1000.0f));
    }

    
    if (ks->state.Q) {
        transform.rotation.z += rotation_speed_scaled;
    }
    if (ks->state.E) {
        transform.rotation.z -= rotation_speed_scaled;
    }   


    static float fire_timer = 0;
    fire_timer += (float)dt;
    bool reset_timer = false;
    if (ks->state.MBL && fire_timer > fire_delay) {
        //transform.position = transform.position + UpVector(transform) * player_speed;
        bullet_pool.SpawnBullet(transform);
        reset_timer=true;
    }
    if (ks->state.MBR && fire_timer > fire_delay) {
        //transform.position = transform.position + UpVector(transform) * player_speed;
        bullet_pool.SpawnBullet(transform);
        reset_timer=true;
    }
    if (reset_timer) {
        fire_timer=0;
        reset_timer = false;
    }
    
    
    static float enemy_spawn_timer = 0;
    enemy_spawn_timer += (float)dt;
    if (enemy_spawn_timer > enemy_spawn_delay) {
        //transform.position = transform.position + UpVector(transform) * player_speed;
        enemy_pool.SpawnEnemy({0.0f, main_camera.height/2, 0.0f}, transform.position);
        enemy_spawn_timer = 0;
    }
    
   
   
    bullet_pool.UpdateBulletState(bullet_speed_scaled);    
    enemy_pool.UpdateEnemyState(enemy_speed_scaled);


    for (int i = 0; i < bullet_pool.pool_size; ++i) {
        if (!bullet_pool.is_active[i]) continue;
        DrawSprite(bullet_sprite, bullet_pool.data[i], main_camera);
    }
    for (int i = 0; i < enemy_pool.pool_size; ++i) {
        if (!enemy_pool.is_active[i]) continue;
        DrawSprite(enemy_sprite, enemy_pool.data[i], main_camera);
    }
    DrawSprite(sprite, transform, main_camera);
}


void UserFree() {

}

