
#pragma once
#include "../Engine/sky_structs.h"

#define MAX_ENTITIES 256

struct HealthComponent { // 8 bytes
    uint32_t max_health;
    uint32_t current_health;
};

struct MovementComponent { // ~ 12 bytes assuming int is 32bit(or 4 byte) and Vector2Int is 64bit (or 8 byte).
    Vector2Int position; 
    uint32_t tiles_per_turn;
};

struct CombatComponent { // ~ 12 bytes
    uint32_t armor_class;
    uint32_t damage_per_hit;
    uint32_t cooldown_per_hit;
};

struct PlayerEntity{ // ~ 24 bytes these are pointers (8 bytes each)
    bool* active;
    HealthComponent* health;
    MovementComponent* movement;
    CombatComponent* combat;
};

struct MonsterEntity { // ~ 24 bytes because these are pointers
    bool* active;
    HealthComponent* health;
    MovementComponent* movement;
    CombatComponent* combat;
};

struct EntityManager {

    PlayerEntity players;
    MonsterEntity monsters;

    int InitializePools(ArenaAllocator* scene_arena) {
        players.active = (bool*)ArenaAllocateAsset(scene_arena, sizeof(bool) * MAX_ENTITIES);
        players.health = (HealthComponent*)ArenaAllocateAsset(scene_arena, sizeof(HealthComponent) * MAX_ENTITIES);
        players.movement = (MovementComponent*)ArenaAllocateAsset(scene_arena, sizeof(MovementComponent) * MAX_ENTITIES);
        players.combat = (CombatComponent*)ArenaAllocateAsset(scene_arena, sizeof(CombatComponent) * MAX_ENTITIES);
        
        monsters.active = (bool*)ArenaAllocateAsset(scene_arena, sizeof(bool) * MAX_ENTITIES);
        monsters.health = (HealthComponent*)ArenaAllocateAsset(scene_arena, sizeof(HealthComponent) * MAX_ENTITIES);
        monsters.movement = (MovementComponent*)ArenaAllocateAsset(scene_arena, sizeof(MovementComponent) * MAX_ENTITIES);
        monsters.combat = (CombatComponent*)ArenaAllocateAsset(scene_arena, sizeof(CombatComponent) * MAX_ENTITIES);
        return 0;
    }
    


};