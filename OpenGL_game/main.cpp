#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"
#include "../Engine/sky_random.cpp"


void Init(int* w, int* h, bool* fullscreen, fColor* clear_color) {
    //*w = 640;
    //*h = 480;
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
    transform.scale = {(float)sprite.pixel_width/32.0f, (float)sprite.pixel_height/32.0f, 1.0f};


    // SECTION: Load bullet sprite
    bullet_sprite = LoadSprite("bullet.bmp");
}

struct TransformPool {
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
    void UpdatePositions(float speed) {
        for (int e = 0; e < this->pool_size; ++e) {
            // NOTE: increment all enemy positions downward.
            // If bullet is out of screen, set its position to -100.
            // NOTE: the -50 is a value that allows us to spawn enemies offscreen without them being tagged as inactive.
            if (!this->is_active[e]) {
                continue;
            }
            this->data[e].position = this->data[e].position + UpVector(this->data[e]) * speed; 
        }

    }
    void SpawnTransform(Transform target_transform) {

        //NOTE: Set spawn position
        this->data[this->curr_pointer].position = target_transform.position;
        //NOTE: Offset the spawn position to be at the front of the ship instead of the center.
        this->data[this->curr_pointer].position = this->data[this->curr_pointer].position + (UpVector(this->data[this->curr_pointer]) * 0.4f);

        // NOTE: Make the z layer be behind the spaceship
        this->data[this->curr_pointer].position.z = -1.0f;
        
        // NOTE: Set spawn rotation
        this->data[this->curr_pointer].rotation = target_transform.rotation;
        // NOTE: Give the rotation a random offset to simulate bullet spread
        this->data[this->curr_pointer].rotation.z += (sky_rand_float(-5.0f, 5.0f));
        
        // NOTE: Set the scale of the bullet sprite
        this->data[this->curr_pointer].scale = Vector3{(float)bullet_sprite.pixel_width/32.0f, (float)bullet_sprite.pixel_height/32.0f, 1.0f};
        
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


TransformPool bullet_pool;
const int BULLET_POOL_SIZE = 20;

void Start(GameState* gs, KeyboardState* ks) {
    bullet_pool.AllocAndInit(BULLET_POOL_SIZE);  
}

void Update(GameState* gs, KeyboardState* ks, double dt) {
    float player_speed = 3.0f * ((float)dt / 1000.0f);
    float rotation_speed = 315.0f * ((float)dt / 1000.0f);
    float bullet_speed = 8.0f * ((float)dt / 1000.0f); 
    float fire_rate = 40.0f; //NOTE: A firerate of 10 makes the bullet fire every 10 milliseconds. 1000 means every second
     

    if (ks->state.W) {
        transform.position.y += player_speed;
    }
    if (ks->state.A) {
        transform.position.x -= player_speed;
    }
    if (ks->state.S) {
        transform.position.y -= player_speed;
    }
    if (ks->state.D) {
        transform.position.x += player_speed;
    }

    if (ks->state.Q) {
        transform.rotation.z += rotation_speed;
    }
    if (ks->state.E) {
        transform.rotation.z -= rotation_speed;
    }   

    static float fire_timer = 0;
    fire_timer += (float)dt;

    if (ks->state.SPACE && fire_timer > fire_rate) {
        //transform.position = transform.position + UpVector(transform) * player_speed;
        bullet_pool.SpawnTransform(transform);
        fire_timer = 0;
    }
    
    bullet_pool.UpdatePositions(bullet_speed);    

    for (int i = 0; i < bullet_pool.pool_size; ++i) {
        DrawSprite(bullet_sprite, bullet_pool.data[i], main_camera);
    }
    DrawSprite(sprite, transform, main_camera);

    //float speed_ps = player_speed * (1000.0f / (float)dt);
    //printf("Playe Speed %f. Delta time: %f. Speed per second: %f\n", player_speed, dt, speed_ps);
    
}


void UserFree() {

}

