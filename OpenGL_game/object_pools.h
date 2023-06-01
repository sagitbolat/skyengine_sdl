#pragma once
// NOTE: This should only be included in the game's main file where it is currently,
// because it relies on the defenition of buller_sprite and enemy_sprite
#include "../Engine/sky_structs.h"
#include "../Engine/skymath.h"

struct BulletPool {
    Transform* data;
    bool* is_active;
    int pool_size;
    int curr_pointer;
    void AllocAndInit(unsigned int size) {
        this->data = (Transform*)calloc(size, sizeof(Transform));
        this->pool_size = size;
        this->curr_pointer = 0;
        this->is_active = (bool*)calloc(size, sizeof(bool));
    }
    void UpdateBulletState(float speed_scaled) {
        for (int e = 0; e < this->pool_size; ++e) {
            // NOTE: increment all bullet positions in the direction it faces.
            // If bullet is out of screen, set its position to -100.
            // NOTE: the -50 is a value that allows us to spawn enemies offscreen without them being tagged as inactive.
            if (!this->is_active[e]) {
                continue;
            }
            this->data[e].position = this->data[e].position + UpVector(this->data[e]) * speed_scaled; 
            
            
            float x = this->data[e].position.x;
            float y = this->data[e].position.y;

            float h_w = main_camera.width / 2;
            float h_h = main_camera.height / 2;
            // NOTE: Set bullet to be inactive if offscreen
            if (x > h_w || y > h_h || x < (-h_w) || y < (-h_h)) {
                this->is_active[e] = false;

            } 
        }

    }
    void SpawnBullet(Transform target_transform, Sprite bullet_sprite) {

        //NOTE: Set spawn position
        this->data[this->curr_pointer].position = target_transform.position;
        //NOTE: Offset the spawn position to be at the front of the ship instead of the center.
        this->data[this->curr_pointer].position = target_transform.position + (UpVector(target_transform) * (sky_rand_float(0.3f, 0.6f)));

        // NOTE: Make the z layer be behind the spaceship
        this->data[this->curr_pointer].position.z = -1.0f;
        
        // NOTE: Set spawn rotation
        this->data[this->curr_pointer].rotation = target_transform.rotation;
        // NOTE: Give the rotation a random offset to simulate bullet spread
        this->data[this->curr_pointer].rotation.z += (sky_rand_float(-10.0f, 10.0f));
        
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

struct EnemyPool {
    Transform* data;
    bool* is_active;
    int pool_size;
    int curr_pointer;
    void AllocAndInit(unsigned int size) {
        this->data = (Transform*)calloc(size, sizeof(Transform));
        this->pool_size = size;
        this->curr_pointer = 0;
        this->is_active = (bool*)calloc(size, sizeof(bool));
    }
    void UpdateEnemyState(float speed_scaled) {
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

    // NOTE: Spawns an enemy at a spawn_position and rotates it towards the target position
    void SpawnEnemy(Vector3 spawn_position, Vector3 target_position, Sprite enemy_sprite) {

        //NOTE: Set spawn position
        this->data[this->curr_pointer].position = spawn_position;
        
        // NOTE: Set spawn rotation
        this->data[this->curr_pointer].rotation = Vector2LookAt(Vec2(spawn_position), Vec2(target_position));
        
        // NOTE: Set the scale of the bullet sprite
        this->data[this->curr_pointer].scale = Vector3{(float)enemy_sprite.pixel_width/PIXELS_PER_UNIT, (float)enemy_sprite.pixel_height/PIXELS_PER_UNIT, 1.0f};
        
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