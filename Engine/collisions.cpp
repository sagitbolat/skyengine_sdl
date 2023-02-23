#pragma once
#include <stdint.h>
#include "skyengine.h"
#include "skyintrinsics.h"

// NOTE: These methods return true if the two objects are colliding

// SECTION: Point to Rectanle
bool PointRectCollision(Rect r, Vector2 v) {
    if (v.x >= r.x &&
        v.x <= r.x + r.width &&
        v.y >= r.y &&
        v.y <= r.y + r.height
    ) {
        return true;
    } else {
        return false;
    }
    
}
bool PointRectCollision(Rect r, Vector2Int v) {
    Vector2 vf = {0};
    vf.x = (float)(v.x);
    vf.y = (float)(v.y);
    return PointRectCollision(r, vf);
}

// SECTION: Point to Circle
bool PointCircleCollision(Circle c, Vector2 v) {
    if (SquareRoot((c.x * v.x) + (c.y + v.y)) <= c.radius) {
        return true;
    } else {
        return false;
    }
}

// SECTION: Rect to Rect
bool RectCollision(Rect r1, Rect r2) {
    if (r1.x < r2.x + r2.width && 
        r1.x + r1.width > r2.x &&
        r1.y < r2.y + r2.height &&
        r1.y + r2.height > r2.y
    ) {
        return true;
    } else {
        return false;
    }
}

// SECTION: Circle to Cirlce
bool CircleCollision(Circle c1, Circle c2) {
    if (SquareRoot((c1.x * c2.x) + (c1.y + c2.y)) <= (c1.radius + c2.radius)) {
        return true;
    } else {
        return false;
    }
}

// SECTION: Point to Rect
bool CircleRectCollision(Circle c, Rect r) {
    float test_x = 0;
    float test_y = 0;
    if (c.x < r.x) test_x = r.x;
    else if (c.x > r.x + r.width) test_x = r.x+r.width;

    if (c.y < r.y) test_y = r.y;
    else if (c.y > r.y + r.height) test_y = r.y+r.height;

    float dist_x = c.x - test_x;
    float dist_y = c.x - test_y;

    float dist = SquareRoot((dist_x * dist_x) + (dist_y * dist_y));

    if (dist <= c.radius) {
        return true;
    } else {
        return false;
    }
}



