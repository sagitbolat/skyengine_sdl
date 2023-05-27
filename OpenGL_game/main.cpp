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


void Awake(GameMemory* gm) {
    sky_srand(199213);
    // SECTION: Load Ship Sprite
    sprite = LoadSprite("ship.bmp");
    transform.position = {0.0f, 0.0f, 0.0f};
    transform.rotation = {0.0f, 0.0f, 0.0f};
    // NOTE: Scale the sprite based on its pixel size.
    transform.scale = {(float)sprite.pixel_width/PIXELS_PER_UNIT, (float)sprite.pixel_height/PIXELS_PER_UNIT, 1.0f};


    // SECTION: Load bullet sprite
    bullet_sprite = LoadSprite("bullet.bmp");
}

struct BulletPool {
    Transform* data;
    bool* is_active;
    int pool_size;
    int curr_pointer;
    void AllocAndInit(unsigned int size) {
        this->data = (Transform*)malloc(sizeof(Transform) * size);
        this->pool_size = size;
        this->curr_pointer = 0;
        this->is_active = (bool*)calloc(size, sizeof(bool));
    }
    void UpdateBulletState(float speed_scaled) {
        for (int e = 0; e < this->pool_size; ++e) {
            // NOTE: increment all enemy positions downward.
            // If bullet is out of screen, set its position to -100.
            // NOTE: the -50 is a value that allows us to spawn enemies offscreen without them being tagged as inactive.
            if (!this->is_active[e]) {
                continue;
            }
            this->data[e].position = this->data[e].position + UpVector(this->data[e]) * speed_scaled; 
        }

    }
    void SpawnBullet(Transform target_transform) {

        //NOTE: Set spawn position
        this->data[this->curr_pointer].position = target_transform.position;
        //NOTE: Offset the spawn position to be at the front of the ship instead of the center.
        this->data[this->curr_pointer].position = target_transform.position + (UpVector(target_transform) * (sky_rand_float(0.3f, 0.6f)));

        // NOTE: Make the z layer be behind the spaceship
        this->data[this->curr_pointer].position.z = -1.0f;
        
        // NOTE: Set spawn rotation
        this->data[this->curr_pointer].rotation = target_transform.rotation;
        // NOTE: Give the rotation a random offset to simulate bullet spread
        this->data[this->curr_pointer].rotation.z += (sky_rand_float(-30.0f, 30.0f));
        
        // NOTE: Set the scale of the bullet sprite
        this->data[this->curr_pointer].scale = Vector3{(float)bullet_sprite.pixel_width/PIXELS_PER_UNIT, (float)bullet_sprite.pixel_height/PIXELS_PER_UNIT, 1.0f};
        
        // NOTE: set bullet to be active
        this->is_active[this->curr_pointer] = true;
        
        // NOTE: incremenet pool pointer
        this->curr_pointer += 1;
        // SECTION: Resetting enemy pool pointer if its at the end of the pool
        if (this->curr_pointer >= this->pool_size) {
            this->curr_pointer = 0;
        }
    }
};


BulletPool bullet_pool;
const int BULLET_POOL_SIZE = 80;

void Start(GameState* gs, KeyboardState* ks) {
    bullet_pool.AllocAndInit(BULLET_POOL_SIZE);  
}

void Update(GameState* gs, KeyboardState* ks, double dt) {
    float player_speed = 6.0f;
    float rotation_speed_scaled = 315.0f * ((float)dt / 1000.0f);
    float bullet_speed_scaled = 10.0f * ((float)dt / 1000.0f); 
    float fire_delay = 40.0f; //NOTE: A firerate of 10 makes the bullet fire every 10 milliseconds. 1000 means every second
     
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
    
    bullet_pool.UpdateBulletState(bullet_speed_scaled);    

    for (int i = 0; i < bullet_pool.pool_size; ++i) {
        DrawSprite(bullet_sprite, bullet_pool.data[i], main_camera);
    }
    DrawSprite(sprite, transform, main_camera);

    //float speed_ps = player_speed * (1000.0f / (float)dt);
    //printf("Playe Speed %f. Delta time: %f. Speed per second: %f\n", player_speed, dt, speed_ps);
    
}


void UserFree() {

}

