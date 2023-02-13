#pragma once
#include "Engine/skyengine.h"

struct RectButton {
    bool is_active;
    bool is_hovered;
    int screen_pos_x, screen_pos_y;
    unsigned int width, height;
};

struct CircleButton {
    bool is_active;
    bool is_hovered;
    int screen_pos_x, screen_pos_y;
    unsigned int radius;
}
