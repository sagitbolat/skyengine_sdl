#pragma once
#include "../Engine/skymath.h"
#include "tilemap.h"


struct EntityMap { // NOTE: Similar to a 2D tilemap, but holds entity ids instead of tile ids.
    int* map;       // NOTE: width x height x depth map. width and height matches tilemap. Can contain up to number of depth entities per tile.
    int  width;
    int  height;
    int  depth;
    int  GetID(int x, int y, int z);
    void SetID(int x, int y, int z, int id);
};
int EntityMap::GetID(int x, int y, int z) {
    return EntityMap::map[(y * EntityMap::width + x) * EntityMap::depth + z];
}
void EntityMap::SetID(int x, int y, int z, int id) {
    EntityMap::map[(y * EntityMap::width + x) * EntityMap::depth + z] = id;
}


struct EmissionTile {
    bool active;
    bool orientation;
    fColor color;
};
struct EmissionMap {
    EmissionTile* map;
    bool*   orientations; // NOTE: true is up-down orientation, false is left-right
    int     width;
    int     height;
    EmissionTile  GetEmissionTile(int x, int y);
    void    SetEmissionTile(int x, int y, EmissionTile tile);
};
EmissionTile EmissionMap::GetEmissionTile(int x, int y) {
    return EmissionMap::map[y * EmissionMap::width + x];
}
void EmissionMap::SetEmissionTile(int x, int y, EmissionTile tile) {
    EmissionMap::map[y * EmissionMap::width + x] = tile;
}



// SECTION: Components
struct EntityComponentMover { // NOTE: Whether movable by the player
    bool  active;
    float seconds_per_tile;
    float move_timer;
    bool  moving;
};
struct EntityComponentEmitter { // NOTE: Laser emitter struct
    bool active;
    fColor emission_color;
};

void EntityComponentMoverInit(EntityComponentMover* component, float seconds_per_tile, bool active = true) {
    component->active = active;
    component->seconds_per_tile = seconds_per_tile;
    component->move_timer = 0.0f;
    component->moving = false;
}
void EntityComponentEmitterInit(EntityComponentEmitter* component, fColor emission_color, bool active = true) {
    component->active = active;
    component->emission_color = emission_color;
}


// SECTION: Entities

struct Entity {
    int         id;
    bool        active; // NOTE: This is equivalent of a null state if this is false.
    Sprite      sprite;
    Transform   transform;
    Vector2Int  prev_position;
    Vector2Int  position;
    int         entity_layer; // NOTE: Layer 0: floor buttons and other features. Layer 1: Pushblocks, emitters, player etc. 
    EntityComponentMover movable;
    EntityComponentEmitter emitter;
}; 

void EntityInit (
    Entity*     entity, 
    int         id, 
    Sprite      sprite, 
    Vector2Int  position={0, 0},
    float       entity_layer=0,
    bool        active=true, 
    bool        movable_active=true, 
    float       seconds_per_tile=0.2f,
    bool        emitter_active=true,
    fColor      emitter_color={1.0f, 1.0f, 1.0f, 1.0f},
    bool        emission_active=false,
    fColor      emission_color={1.0f, 1.0f, 1.0f, 1.0f}
) {
    entity->id            = id;
    entity->sprite        = sprite;
    entity->active        = active;
    entity->prev_position = position; // NOTE: Cached for transform updates. Do not modify manually
    entity->position      = position;
    entity->entity_layer  = entity_layer;

    EntityComponentMoverInit(&entity->movable, seconds_per_tile, movable_active);
    EntityComponentEmitterInit(&entity->emitter, emitter_color, emitter_active);

    Transform transform = {0.0f};
    transform.position  = {(float)position.x, (float)position.y, float(entity_layer)};
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
    EntityInit(player, 0, sprite, init_position, 1, true, true, move_speed);
}
void PushblockInit(
    Entity* pushblock,
    int id,
    Sprite sprite,
    Vector2Int init_position,
    Entity player
) {
    EntityInit(pushblock, id, sprite, init_position, 1, true, true, player.movable.seconds_per_tile);
}
void StaticBlockInit(
    Entity* static_block,
    int id,
    Sprite sprite,
    Vector2Int init_position
) {
    EntityInit(static_block, id, sprite, init_position, 1, true, false);
}
void EmitterInit(
    Entity* emitter,
    int id,
    Sprite sprite,
    Vector2Int init_position,
    fColor emitter_color
) {
    EntityInit(emitter, id, sprite, init_position, 1, true, false, 0.2f, true, emitter_color);
}





// SECTION: Entity state change functions. Should be called only when you need to change a state of the entity. NOT EVERY FRAME

// NOTE: EntityComponentMover state change
// Returns true if moving was successful. false if was unable to move the entity
bool EntityMove(int entity_id, Vector2Int direction, Tilemap map, EntityMap entity_id_map, Entity* entity_array, int move_weight = 1) {
    if (move_weight < 0) return false;
    
    Entity* entity = &entity_array[entity_id];
    
    if (!entity->movable.active) return false;

    Vector2Int new_position = entity->position + direction;

    if (TestTileCollide(map, new_position)) return false;


    int new_entity_id = entity_id_map.GetID(new_position.x, new_position.y, entity->entity_layer);

    Entity* new_entity = &entity_array[new_entity_id]; 


    bool could_move_new_entity = false;
    if (new_entity_id >= 0 && new_entity->active) {
        could_move_new_entity = EntityMove(new_entity_id, direction, map, entity_id_map, entity_array, move_weight-1);
    } else if(new_entity_id < 0) {
        could_move_new_entity = true;
    } 

    if (could_move_new_entity) {
        entity->movable.moving = true;
        entity->position = entity->position + direction;

        entity_id_map.SetID(entity->position.x, entity->position.y, entity->entity_layer, entity->id);
        entity_id_map.SetID(entity->prev_position.x, entity->prev_position.y, entity->entity_layer, -1);
        
        return true;
    }
    return false;
}

// NOTE: EntityComponentEmitter state change. Sets layer 2 to laser tiles.
Vector2Int EntityEmit(int entity_id, Tilemap map, EntityMap entity_id_map, Entity* entity_array) {
    // TODO:
    return {0, 0}; 
}


// SECTION: Update methods. Should be called every frame (or call EntityUpdate, it encapsulates all other Entity Update functions, and checks
//          for active components).
void EntityUpdateMover(int entity_id, Entity* entity_array, float dt) {
    Entity* entity = &entity_array[entity_id];

    if (!entity->active) return;

    if (entity->movable.moving) entity->movable.move_timer += dt;
    float move_t = entity->movable.move_timer / (entity->movable.seconds_per_tile * 1000);

    entity->transform.position.y = Lerp((float)entity->prev_position.y, (float)entity->position.y, move_t);
    entity->transform.position.x = Lerp((float)entity->prev_position.x, (float)entity->position.x, move_t);
    //entity->transform.position.y = Lerp(entity->transform.position.y, (float)entity->position.y, move_t);
    //entity->transform.position.x = Lerp(entity->transform.position.x, (float)entity->position.x, move_t);

    if (move_t >= 1.0f) {
        entity->transform.position.y = (float)entity->position.y;
        entity->transform.position.x = (float)entity->position.x;


        entity->movable.move_timer = 0.0f;
        entity->movable.moving = false;
        entity->prev_position = entity->position;
    }
}

void EntityUpdate(int entity_id, Entity* entity_array, float dt) {
    EntityUpdateMover(entity_id, entity_array, dt);
}



// SECTION: Render methods. Should be called after update methods when you actually do the rendering.
void EntityRender(int entity_id, Entity* entity_array, Tilemap* tilemap=nullptr, EntityMap* entity_id_map=nullptr) {
    Entity* entity = &entity_array[entity_id];

    if (!entity->active) return;

    DrawSprite(entity->sprite, entity->transform, main_camera);

}
void EmissionRender(EmissionMap map, Sprite emission_sprite_sheet, GL_ID* shaders) {
    
    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            if (map.GetEmissionTile(x, y).active == false) continue;


            float uv_x_offset = (map.GetEmissionTile(x, y).orientation) ? 0.5f : 0.0f;

            ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
            ShaderSetVector(shaders, "top_right_uv", Vector2{0.5f, 1.0f});
            ShaderSetVector(shaders, "uv_offset", Vector2{uv_x_offset, 0.0f});
            ShaderSetVector(shaders, "i_color_multiplier", Vec4(map.GetEmissionTile(x,y).color));
            Transform transform = {0.0f};
            transform.position = Vector3{float(x), float(y), 2.0f};
            transform.rotation = Vector3{0.0f, 0.0f, 0.0f};
            transform.scale    = Vector3{1.0f, 1.0f, 1.0f}; 
            DrawSprite(emission_sprite_sheet, transform, main_camera);
            ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
            ShaderSetVector(shaders, "top_right_uv", Vector2{1.0f, 1.0f});
            ShaderSetVector(shaders, "uv_offset", Vector2{0.0f, 0.0f});

            EmissionTile tile = {false, false, {0.0f, 0.0f, 0.0f, 0.0f}};
            map.SetEmissionTile(x, y, tile);

        }
    }
}