#pragma once
#include "../skyengine.h"

enum ButtonState {
    inactive = 0,
    hovered = 1,
    active = 2,
};

struct RectButton {
    enum ButtonState state;
    Rect collider;
    Vector2Int center_pos;
    ImageData inactive_sprite, active_sprite, hover_sprite;
    int sprite_scale;
};

/*struct CircleButton {
    enum ButtonState state;
    Circle collider;
    ImageData inactive_sprite, active_sprite, hover_sprite;
    int sprite_scale;
};*/



// SECTION: Rect Button
// NOTE: images must have same resolution size.
void InitButton(RectButton* button, int center_x, int center_y, int scale, ImageData inactive_sprite, ImageData hover_sprite, ImageData active_sprite) {
    
    if (
        inactive_sprite.width != hover_sprite.width || 
        inactive_sprite.width != active_sprite.width || 
        hover_sprite.width != active_sprite.width ||
        inactive_sprite.height != hover_sprite.height || 
        inactive_sprite.height != active_sprite.height || 
        hover_sprite.height != active_sprite.height
    ) {
        printf("ERROR: Button sprites do no match in size");
        return;    
    }

    
    int width = inactive_sprite.width;
    int height = inactive_sprite.height;
    
    Rect collider = {0};
    collider.x = center_x - (width * scale / 2);  
    collider.y = center_y - (height * scale / 2); 
    collider.width = width * scale; 
    collider.height = height * scale; 

    Vector2Int center_pos = {0};
    center_pos.x = center_x;
    center_pos.y = center_y;

    button->state = inactive;
    button->collider = collider;
    button->center_pos = center_pos;
    button->inactive_sprite = inactive_sprite;
    button->hover_sprite = hover_sprite;
    button->active_sprite = active_sprite;
    button->sprite_scale = scale;
}

void BlitButton(GameBitmapBuffer* graphics_buffer, RectButton* button) {
    ImageData* sprite_ptr;
    if (button->state == inactive) {
        sprite_ptr = &(button->inactive_sprite);
    } else if (button->state == hovered) {
        sprite_ptr = &(button->hover_sprite);
    } else if (button->state == active) {
        sprite_ptr = &(button->active_sprite);
    }

    BlitBitmapScaled (
        graphics_buffer, 
        sprite_ptr,
        button->center_pos.x,
        button->center_pos.y,
        button->sprite_scale,
        1, true
    );
}


