#pragma once
#include "../Engine/skymath.h"
#include "tilemap.h"

#define MOVE_SPEED 0.2f // NOTE: The player's movespeed measured in seconds per block.

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
    enum ORIENTATION_ENUM {HORIZONTAL, VERTICAL, CROSSED} orientation;
    Color color;
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
struct EntityComponentPlayer {
    bool active;
    Sprite up_sprite;
    Sprite down_sprite;
    Sprite left_sprite;
    Sprite right_sprite;
    enum DIRECTION_ENUM {UP, RIGHT, DOWN, LEFT, NEUTRAL} direction;
};
struct EntityComponentMover { // NOTE: Whether movable by the player
    bool  active;
    float seconds_per_tile;
    float move_timer;
    bool  moving;
};
struct EntityComponentEmitter { // NOTE: Laser emitter struct
    bool active;
    Sprite nozzle_sprite;
    Sprite indicator_sprite;
    Color emission_color;
    enum DIRECTION_ENUM {UP, RIGHT, DOWN, LEFT} direction;
};
struct EntityComponentReceiver { // NOTE: Laser reciever struct
    bool active;
    Sprite nozzle_sprite;
    Sprite indicator_sprite;
    Color accepted_color;
    bool signal_received; // NOTE: turns true if a laser hits the Reciever
    bool signal_accepted; // NOTE: turns true if the laser is of the correct color.
    Color signal_color;
};
#define MAX_CONNECTED_ACTIVATORS 5
struct EntityComponentDoor { // NOTE: A door that can be open or closed
    bool active;
    bool is_open;
    bool open_by_default; // NOTE: If true, the door will be open by default and closed when activated
    Sprite open_sprite;
    Sprite closed_sprite;
    Sprite open_by_default_open_sprite;
    Sprite open_by_default_closed_sprite;
    int connected_activators_ids[MAX_CONNECTED_ACTIVATORS];
    int num_connected_activators;
};
struct EntityComponentEndgoal {
    bool active;
};
struct EntityComponentButton {
    bool active;
    Sprite up_sprite;
    Sprite down_sprite;
    bool is_pressed;
};

void EntityComponentMoverInit(EntityComponentMover* component, float seconds_per_tile, bool active = true) {
    component->active = active;
    component->seconds_per_tile = seconds_per_tile;
    component->move_timer = 0.0f;
    component->moving = false;
}
void EntityComponentEmitterInit(EntityComponentEmitter* component, Color emission_color,  EntityComponentEmitter::DIRECTION_ENUM dir, bool active = true) {
    component->active = active;
    component->emission_color = emission_color;
    component->direction = dir;
}
void EntityComponentReceiverInit(EntityComponentReceiver* component, Color accepted_color, bool active=true) {
    component->active = active;
    component->accepted_color = accepted_color; // NOTE: The color that the receiver accepts as valid.
    component->signal_received = false;
    component->signal_accepted = false;
    component->signal_color = {0, 0, 0, 0};
}
void EntityComponentDoorInit(EntityComponentDoor* component, bool open_by_default = false, int connected_receivers[MAX_CONNECTED_ACTIVATORS] = nullptr, int num_connected_activators = 0, bool active=true) {
    component->active  = active;
    component->is_open = false;
    component->open_by_default = open_by_default;
    if (num_connected_activators == 0 || connected_receivers == nullptr) {
        for (int i = 0; i < MAX_CONNECTED_ACTIVATORS; ++i) {
            component->connected_activators_ids[i] = -1; 
        }
        component->num_connected_activators = 0;
    } else {
        for (int i = 0; i < MAX_CONNECTED_ACTIVATORS; ++i) {
            if (i < num_connected_activators) {
                component->connected_activators_ids[i] = connected_receivers[i];
            } else {
                component->connected_activators_ids[i] = -1;
            }
        }
        component->num_connected_activators = num_connected_activators;
    }
}
void EntityComponentButtonInit(EntityComponentButton* component, bool is_pressed = false, bool active = true) {
    component->active = active;
    component->is_pressed = is_pressed;
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
    EntityComponentPlayer   player;
    EntityComponentMover    movable;
    EntityComponentEmitter  emitter;
    EntityComponentReceiver receiver;
    EntityComponentDoor     door;
    EntityComponentEndgoal  endgoal;
    EntityComponentButton   button;
}; 


void PrintEntity(Entity e) {
    printf("############################\n");
    printf("ID:                     %d\n", e.id);
    printf("Active:                 %d\n", e.active);
    printf("Position:               {%d, %d}\n", e.position.x, e.position.y);
    printf("TransformPos:           {%f, %f, %f}\n", e.transform.position.x, e.transform.position.y, e.transform.position.z);
    printf("Layer:                  %d\n", e.entity_layer);
    printf("Player::Active:         %d\n", e.player.active);
    printf("Mover::Active           %d\n", e.movable.active);
    printf("Mover::Moving           %d\n", e.movable.moving);
    printf("Emitter::Active:        %d\n", e.emitter.active);
    printf("Emitter::Color:         {%d, %d, %d, %d}\n", e.emitter.emission_color.r, e.emitter.emission_color.g, e.emitter.emission_color.b, e.emitter.emission_color.a);
    printf("Receiver::Active:       %d\n", e.receiver.active);
    printf("Receiver::ColorReceived:{%d, %d, %d, %d}\n", e.receiver.signal_color.r, e.receiver.signal_color.g, e.receiver.signal_color.b, e.receiver.signal_color.a);
    printf("Receiver::ColorAccepted:{%d, %d, %d, %d}\n", e.receiver.accepted_color.r, e.receiver.accepted_color.g, e.receiver.accepted_color.b, e.receiver.accepted_color.a);
    printf("Receiver::SigReceived:  %d\n", e.receiver.signal_received);
    printf("Receiver::SigAccepted:  %d\n", e.receiver.signal_accepted);
    printf("Door::Active:           %d\n", e.door.active);
    printf("Door::Open:             %d\n", e.door.is_open);
    printf("Door::OpenByDefault:    %d\n", e.door.open_by_default);
    printf("Door::NumAttached       %d\n", e.door.num_connected_activators);
    printf("Door::AttachedReceivers:{%d", e.door.connected_activators_ids[0]);
    for (int i = 1; i < MAX_CONNECTED_ACTIVATORS; ++i) {
        
        printf(", %d", e.door.connected_activators_ids[i]);
    }
    printf("}\n");
    printf("Endgoal::Active:        %d\n", e.endgoal.active);
    printf("Button::Active:         %d\n", e.button.active);
    printf("Button::Pressed:        %d\n", e.button.is_pressed);
    printf("############################\n");

} 


void EntityInit (
    Entity*     entity, 
    int         id, 
    Sprite      sprite, 
    Vector2Int  position={0, 0},
    float       entity_layer=0,
    bool        active=true 
) {
    entity->id            = id;
    entity->sprite        = sprite;
    entity->active        = active;
    entity->prev_position = position; // NOTE: Cached for transform updates. Do not modify manually
    entity->position      = position;
    entity->entity_layer  = entity_layer;

    Transform transform = {0.0f};
    transform.position  = {(float)position.x, (float)position.y, float(entity_layer)};
    transform.rotation  = {0.0f, 0.0f, 0.0f};
    transform.scale     = {1.0f, 1.0f, 1.0f};
    
    entity->transform   = transform;

    // SECTION: Zero The entity components. Should be unnecessary but done just in case
    entity->player.active = false;
    entity->endgoal.active = false;
    EntityComponentMoverInit(&entity->movable, MOVE_SPEED, false);
    EntityComponentEmitterInit(&entity->emitter, {0, 0, 0, 0}, EntityComponentEmitter::DIRECTION_ENUM::DOWN, false);
    EntityComponentReceiverInit(&entity->receiver, {0, 0, 0, 0}, false);
    EntityComponentDoorInit(&entity->door, false, nullptr, 0, false);
    EntityComponentButtonInit(&entity->button, false, false);
}

void PlayerInit(
    Entity* player,
    int id,
    Sprite sprite,
    Sprite up_sprite,
    Sprite down_sprite,
    Sprite left_sprite,
    Sprite right_sprite,
    Vector2Int init_position
) {
    EntityInit(player, id, sprite, init_position, 1.0f);
    EntityComponentMoverInit(&player->movable, MOVE_SPEED, true);
    player->player.active = true;
    player->player.up_sprite    = up_sprite;
    player->player.down_sprite  = down_sprite;
    player->player.left_sprite  = left_sprite;
    player->player.right_sprite = right_sprite;
}
void PushblockInit(
    Entity* pushblock,
    int id,
    Sprite sprite,
    Vector2Int init_position
) {
    EntityInit(pushblock, id, sprite, init_position, 1.0f);
    EntityComponentMoverInit(&pushblock->movable, MOVE_SPEED, true);
}
void StaticBlockInit(
    Entity* static_block,
    int id,
    Sprite sprite,
    Vector2Int init_position
) {
    EntityInit(static_block, id, sprite, init_position, 1.0f);
}
void EmitterInit(
    Entity* emitter,
    int id,
    Sprite sprite,
    Sprite nozzle_sprite,
    Sprite indicator_sprite,
    Vector2Int init_position,
    Color emitter_color,
    bool emitter_movable,
    EntityComponentEmitter::DIRECTION_ENUM direction
) {
    EntityInit(emitter, id, sprite, init_position, 1.0f);
    EntityComponentEmitterInit(&emitter->emitter, emitter_color, direction, true);
    EntityComponentMoverInit(&emitter->movable, MOVE_SPEED, emitter_movable);
    emitter->emitter.nozzle_sprite = nozzle_sprite;
    emitter->emitter.indicator_sprite = indicator_sprite;
}
void ReceiverInit(
    Entity* receiver,
    int id,
    Sprite sprite,
    Sprite nozzle_sprite,
    Sprite indicator_sprite,
    Vector2Int init_position,
    Color accepted_signal_color,
    bool receiver_movable
) {
    EntityInit(receiver, id, sprite, init_position, 1.0f);
    EntityComponentReceiverInit(&receiver->receiver, accepted_signal_color, true);
    EntityComponentMoverInit(&receiver->movable, MOVE_SPEED, receiver_movable);
    receiver->receiver.nozzle_sprite = nozzle_sprite;
    receiver->receiver.indicator_sprite = indicator_sprite;
}
void DoorInit(
    Entity* door,
    int id,
    Sprite open_vertical_sprite,
    Sprite closed_vertical_sprite,
    Sprite open_horizontal_sprite,
    Sprite closed_horizontal_sprite,
    Vector2Int init_position,
    bool open_by_default = false,
    int connected_receivers[MAX_CONNECTED_ACTIVATORS] = nullptr,
    int num_connected_activators = 0
) {
    //EntityInit(door, id, sprite, init_position, 1.0f, true, false);

    EntityInit(door, id, open_vertical_sprite, init_position, 0.0f);
    EntityComponentDoorInit(&door->door, open_by_default, connected_receivers, num_connected_activators, true);
    door->door.open_sprite = open_vertical_sprite;
    door->door.closed_sprite = closed_vertical_sprite;
    door->door.open_by_default_open_sprite = open_horizontal_sprite;
    door->door.open_by_default_closed_sprite = closed_horizontal_sprite;
}
void EndgoalInit(
    Entity* endgoal,
    int id,
    Sprite goal_sprite,
    Vector2Int init_position
) {
    EntityInit(endgoal, id, goal_sprite, init_position, 0.0f);
    endgoal->endgoal.active = true;
}
void ButtonInit(
    Entity* button,
    int id,
    Sprite up_sprite,
    Sprite down_sprite,
    Vector2Int init_position
) {
    EntityInit(button, id, up_sprite, init_position, 0.0f, true);
    EntityComponentButtonInit(&button->button, false, true);
    button->button.up_sprite = up_sprite;
    button->button.down_sprite = down_sprite;
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

    // NOTE: Check for closed door:
    {
        int floor_entity_id = entity_id_map.GetID(new_position.x, new_position.y, 0);
        Entity floor_entity = entity_array[floor_entity_id];
        if (floor_entity_id >= 0 && 
            floor_entity.active && 
            floor_entity.door.active && 
            !floor_entity.door.is_open
        ) {
            return false;
        }
    }

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
// Returns the position of the object hit by the laser. Returns {-1,-1} if no emission can be performed
Vector2Int EntityUpdateEmit(int entity_id, Tilemap map, EntityMap entity_id_map, EmissionMap emission_map, Entity* entity_array) {
    if (entity_id < 0) return {-1, -1};

    Entity* entity = &entity_array[entity_id];

    if(!entity->active) return {-1, -1};
    if(!entity->emitter.active) return {-1, -1};     
    if(entity->movable.active && entity->movable.moving) return {-1, -1}; // NOTE: do not emit if the emitter is moving

    Vector2Int _direction = {0,0};
    EmissionTile::ORIENTATION_ENUM orient = EmissionTile::VERTICAL;
    if (entity->emitter.direction == EntityComponentEmitter::UP) {
        _direction.x = 0;
        _direction.y = 1;
        entity->transform.rotation.z = 0.0f;
    }
    else if (entity->emitter.direction == EntityComponentEmitter::DOWN ) {
        _direction.x = 0;
        _direction.y = -1;
        entity->transform.rotation.z = 180.0f;
    }
    else if (entity->emitter.direction == EntityComponentEmitter::LEFT ) {
        _direction.x = -1;
        _direction.y = 0;
        orient = EmissionTile::HORIZONTAL;
        entity->transform.rotation.z = 90.0f;
    }
    else if (entity->emitter.direction == EntityComponentEmitter::RIGHT) {
        _direction.x = 1;
        _direction.y = 0;
        orient = EmissionTile::HORIZONTAL;
        entity->transform.rotation.z = 270.0f;
    }


    Vector2Int curr_direction = {0,0};
    while (true) {
        curr_direction = curr_direction + _direction;
        Vector2Int new_position = entity->position + curr_direction;
        
        // NOTE: Layer 1 entities collision
        int new_entity_id = entity_id_map.GetID(new_position.x, new_position.y, 1);

        if (new_entity_id >= 0) {

            //SECTION: Activate the receiver if its hit
            Entity* new_entity = &entity_array[new_entity_id];
            if (new_entity->receiver.active) {
                new_entity->receiver.signal_color = entity->emitter.emission_color;
                new_entity->receiver.signal_received = true;
            }


            return (new_position - _direction);
        }

        //NOTE: Layer 0 (floor) entities collision (for doors when closed)
        int floor_entity_id = entity_id_map.GetID(new_position.x, new_position.y, 0);

        if (floor_entity_id >= 0) {

            //SECTION: Activate the receiver if its hit
            Entity* floor_entity = &entity_array[floor_entity_id];

            if (floor_entity->active && floor_entity->door.active && !floor_entity->door.is_open) {
                return (new_position - _direction);
            }
        }


        if (TestTileCollide(map, new_position)) {
            return (new_position - _direction);
        }

        if (new_position.x < 0 || new_position.y < 0) {
            return (new_position - _direction);
        } else if (new_position.x >= map.width || new_position.y >= map.height) {
            return (new_position - _direction);
        }

        EmissionTile tile = emission_map.GetEmissionTile(new_position.x, new_position.y);

        if (tile.active) {
            if (tile.orientation == orient) tile.color = tile.color + entity->emitter.emission_color;
            else {
                tile.orientation = EmissionTile::CROSSED;
                tile.color = tile.color + entity->emitter.emission_color;
            }
        } else {
            tile.active = true;
            tile.color = entity->emitter.emission_color;
            tile.orientation = orient;
        }

        emission_map.SetEmissionTile(new_position.x, new_position.y, tile);


    }

}


// SECTION: Update methods. Should be called every frame (or call EntityUpdate, it encapsulates all other Entity Update functions, and checks
//          for active components).
void EntityUpdateMover(int entity_id, Entity* entity_array, float dt) {
    Entity* entity = &entity_array[entity_id];

    if (!entity->active) return;
    if (!entity->movable.active) return;
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

void EntityUpdateReceiver(int entity_id, Entity* entity_array) {
    Entity* entity = &entity_array[entity_id];

    if (!entity->active || !entity->receiver.active) return;

    if (entity->receiver.signal_received) {
        if (CompareColor(entity->receiver.signal_color, entity->receiver.accepted_color)) {
            entity->receiver.signal_accepted = true;
        } 
        entity->receiver.signal_accepted = true;
    } else if (!entity->receiver.signal_received) {
        entity->receiver.signal_color = {0, 0, 0, 0};
        entity->receiver.signal_accepted = false;
    }

}

void EntityUpdateDoor(int entity_id, Entity* entity_array, EntityMap map) {
    Entity* door_entity = &entity_array[entity_id];

    if (!door_entity->active || 
        !door_entity->door.active 
    ) return;

    bool open_by_default = door_entity->door.open_by_default;
    door_entity->door.is_open = open_by_default;

    if (door_entity->door.num_connected_activators > 0) { 
        for (int i = 0; i < door_entity->door.num_connected_activators; ++i) {
            int id = door_entity->door.connected_activators_ids[i];
            if (id < 0) continue;
            Entity* activator_entity = &entity_array[id];
            
            if (!activator_entity->active) continue;

            if (activator_entity->receiver.active && activator_entity->receiver.signal_accepted) {
                door_entity->door.is_open = !open_by_default;
                break;
            } else if (activator_entity->button.active && activator_entity->button.is_pressed) {
                door_entity->door.is_open = !open_by_default;
                break;
            }
        }
    }
    int top_entity_id = map.GetID(door_entity->position.x, door_entity->position.y, 1);
    if (top_entity_id >= 0 && entity_array[top_entity_id].active) door_entity->door.is_open = true;
}

void EntityUpdateButton(int entity_id, Entity* entity_array, EntityMap map) {

    Entity* entity = &entity_array[entity_id];

    if (!entity->active || !entity->button.active) return;


    Vector2Int pos = entity->position;
    int top_entity_id = map.GetID(pos.x, pos.y, 1);

    if (top_entity_id < 0) entity->button.is_pressed = false;
    else entity->button.is_pressed = true;
}



// SECTION: Render methods. Should be called after update methods when you actually do the rendering.
void EntityRender(int entity_id, Entity* entity_array, GL_ID* shaders, bool level_transitioning = false) {
    Entity* entity = &entity_array[entity_id];

    if (!entity->active) return;

    if (entity->player.active) {
        switch(entity->player.direction) {
            case EntityComponentPlayer::DIRECTION_ENUM::UP: {
                DrawSprite(entity->player.up_sprite, entity->transform, main_camera);
                return;
            } break;
            case EntityComponentPlayer::DIRECTION_ENUM::DOWN: {
                DrawSprite(entity->player.down_sprite, entity->transform, main_camera);
                return;
            } break;
            case EntityComponentPlayer::DIRECTION_ENUM::LEFT: {
                DrawSprite(entity->player.left_sprite, entity->transform, main_camera);
                return;
            } break;
            case EntityComponentPlayer::DIRECTION_ENUM::RIGHT: {
                DrawSprite(entity->player.right_sprite, entity->transform, main_camera);
                return;
            } break;
            case EntityComponentPlayer::DIRECTION_ENUM::NEUTRAL: {
                DrawSprite(entity->sprite, entity->transform, main_camera);
                return;
            } break;
        }
    }


    // SECTION: Rendering door
    else if (entity->door.active) {
        Transform transform = entity->transform;
        if (entity->door.is_open) {
            if (entity->door.open_by_default) {
                DrawSprite(entity->door.open_by_default_open_sprite, transform, main_camera);
            } else {
                DrawSprite(entity->door.open_sprite, transform, main_camera);
            }
        } else {
            if (entity->door.open_by_default) {
                DrawSprite(entity->door.open_by_default_closed_sprite, transform, main_camera);
            } else {
                DrawSprite(entity->door.closed_sprite, transform, main_camera);
            }
        }
    }

    else if (entity->button.active) {
        Transform transform = entity->transform;
        if (entity->button.is_pressed) {
            DrawSprite(entity->button.down_sprite, transform, main_camera);
        } else {
            DrawSprite(entity->button.up_sprite, transform, main_camera);
        }

    }

    // SECTION: Emitter rendering
    else if (entity->emitter.active && !entity->movable.moving) {
        DrawSprite(entity->sprite, entity->transform, main_camera);
        Transform transform = entity->transform;
        transform.position.z += 0.1f;
        if (!level_transitioning) ShaderSetVector(shaders, "i_color_multiplier", Vec4(entity->emitter.emission_color));
        DrawSprite(entity->emitter.nozzle_sprite, transform, main_camera);
        if (!level_transitioning) ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
    }
    // SECTION: Receiver rendering
    else if (entity->receiver.active) {
        DrawSprite(entity->sprite, entity->transform, main_camera);
        Transform transform = entity->transform;
        transform.position.z += 0.1f;
        if (!level_transitioning) ShaderSetVector(shaders, "i_color_multiplier", Vec4(entity->receiver.accepted_color));
        DrawSprite(entity->receiver.indicator_sprite, transform, main_camera);
        if (!level_transitioning) ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
        transform.position.z += 0.1f;
        if (entity->receiver.signal_received) {
            if (!level_transitioning) ShaderSetVector(shaders, "i_color_multiplier", Vec4(entity->receiver.signal_color));
            DrawSprite(entity->receiver.nozzle_sprite, transform, main_camera);
        }
        if (!level_transitioning) ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});
        // NOTE: now that we finished rendering, we can mark receiver non-signaled.
        entity->receiver.signal_received = false;
    }
    else {
        DrawSprite(entity->sprite, entity->transform, main_camera);
    }
}
void EmissionRender(EmissionMap map, Sprite emission_sprite_sheet, GL_ID* shaders, bool level_transitioning = false) {
    
    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            EmissionTile tile = map.GetEmissionTile(x, y);
            if (tile.active == false) continue;

            float uv_x_offset = 0.0f;
            if (tile.orientation == EmissionTile::VERTICAL) uv_x_offset = (1.0f/3.0f);
            if (tile.orientation == EmissionTile::CROSSED) uv_x_offset = (2.0f/3.0f);

            ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
            ShaderSetVector(shaders, "top_right_uv", Vector2{1.0f/3.0f, 1.0f});
            ShaderSetVector(shaders, "uv_offset", Vector2{uv_x_offset, 0.0f});
            if (!level_transitioning) ShaderSetVector(shaders, "i_color_multiplier", Vec4(map.GetEmissionTile(x,y).color));
            Transform transform = {0.0f};
            transform.position = Vector3{float(x), float(y), 2.0f};
            transform.rotation = Vector3{0.0f, 0.0f, 0.0f};
            transform.scale    = Vector3{1.0f, 1.0f, 1.0f}; 
            DrawSprite(emission_sprite_sheet, transform, main_camera);
            ShaderSetVector(shaders, "bot_left_uv", Vector2{0.0f, 0.0f});
            ShaderSetVector(shaders, "top_right_uv", Vector2{1.0f, 1.0f});
            ShaderSetVector(shaders, "uv_offset", Vector2{0.0f, 0.0f});
            if (!level_transitioning) ShaderSetVector(shaders, "i_color_multiplier", Vector4{1.0f, 1.0f, 1.0f, 1.0f});

            tile = {false, EmissionTile::HORIZONTAL, Color{0, 0, 0, 0}};
            map.SetEmissionTile(x, y, tile);

        }
    }
}