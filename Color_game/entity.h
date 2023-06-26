#pragma once
#include "../Engine/skymath.h"
#include "tilemap.h"


struct EntityMap { // NOTE: Similar to a 2D tilemap, but holds entity ids instead of tile ids.
    int* map;
    int  width;
    int  height;
    int  GetID(int x, int y);
    void SetID(int x, int y, int id);
};


// SECTION: Components

struct EntityComponentMoveable { // NOTE: Whether movable by the player
    bool  active;
    float seconds_per_tile;
    float move_timer;
    bool  moving;
};
void EntityComponentMovableInit(EntityComponentMoveable* component, float seconds_per_tile, bool active = true) {
    component->active = active;
    component->seconds_per_tile = seconds_per_tile;
    component->move_timer = 0.0f;
    component->moving = false;
}



// SECTION: Entities

struct Entity {
    int         id;
    bool        active; // NOTE: This is equivalent of a null state if this is false.
    Sprite      sprite;
    Transform   transform;
    Vector2Int  prev_position;
    Vector2Int  position;
    EntityComponentMoveable movable;
}; 

void EntityInit (
    Entity*     entity, 
    int         id, 
    Sprite      sprite, 
    Vector2Int  position={0, 0}, 
    bool        active=true, 
    bool        movable_active=true, 
    float       seconds_per_tile=0.2f
) {
    entity->id            = id;
    entity->sprite        = sprite;
    entity->active        = active;
    entity->prev_position = position; // NOTE: Cached for transform updates. Do not modify manually
    entity->position      = position;


    EntityComponentMovableInit(&entity->movable, seconds_per_tile, movable_active);

    Transform transform = {0.0f};
    transform.position  = {(float)position.x, (float)position.y, 1.0f};
    transform.rotation  = {0.0f, 0.0f, 0.0f};
    transform.scale     = {1.0f, 1.0f, 1.0f};
    
    entity->transform   = transform;
}

void PlayerInit(
    Entity* player,
    Sprite sprite,
    Vector2Int init_position,
    float move_speed=0.2f
) {
    EntityInit(player, 0, sprite, init_position, true, true, move_speed);
}
void PushblockInit(
    Entity* pushblock,
    int id,
    Sprite sprite,
    Vector2Int init_position,
    Entity player
) {
    EntityInit(pushblock, id, sprite, init_position, true, true, player.movable.seconds_per_tile);
}
void StaticBlockInit(
    Entity* static_block,
    int id,
    Sprite sprite,
    Vector2Int init_position
) {
    EntityInit(static_block, id, sprite, init_position, true, false);
}

int EntityMap::GetID(int x, int y) {
    return EntityMap::map[y * EntityMap::width + x];
}
void EntityMap::SetID(int x, int y, int id) {
    EntityMap::map[y * EntityMap::width + x] = id;
}


// NOTE: Returns true if moving was successful. false if was unable to move the entity
bool EntityMove(int entity_id, Vector2Int direction, Tilemap map, EntityMap entity_id_map, Entity* entity_array) {
    Entity* entity = &entity_array[entity_id];
    
    if (!entity->movable.active) return false;

    Vector2Int new_position = entity->position + direction;

    if (TestTileCollide(map, new_position)) return false;


    int new_entity_id = entity_id_map.GetID(new_position.x, new_position.y);
    Entity* new_entity = &entity_array[new_entity_id]; 

    if (new_entity_id < 0 || !new_entity->active) {
        entity->movable.moving = true;

        entity->position = entity->position + direction;

        entity_id_map.SetID(entity->position.x, entity->position.y, entity->id);
        entity_id_map.SetID(entity->prev_position.x, entity->prev_position.y, -1);
        
        return true;
    }
    
    
    if (!new_entity->movable.active) return false;

    bool could_move_new_entity = EntityMove(new_entity_id, direction, map, entity_id_map, entity_array);

    if (could_move_new_entity) {
        entity->movable.moving = true;

        entity->position = entity->position + direction;

        entity_id_map.SetID(entity->position.x, entity->position.y, entity->id);
        entity_id_map.SetID(entity->prev_position.x, entity->prev_position.y, -1);
        
        return true;
    } 
    
    
    return false;
}

void EntityUpdateTransform(int entity_id, Entity* entity_array, float dt) {
    Entity* entity = &entity_array[entity_id];

    if (!entity->active) return;

    if (entity->movable.moving) entity->movable.move_timer += dt;
    float move_t = entity->movable.move_timer / (entity->movable.seconds_per_tile * 1000);

    entity->transform.position.y = Lerp((float)entity->prev_position.y, (float)entity->position.y, move_t);
    entity->transform.position.x = Lerp((float)entity->prev_position.x, (float)entity->position.x, move_t);

    if (move_t >= 1.0f) {
        entity->transform.position.y = (float)entity->position.y;
        entity->transform.position.x = (float)entity->position.x;


        entity->movable.move_timer = 0.0f;
        entity->movable.moving = false;
        entity->prev_position = entity->position;
    }
}