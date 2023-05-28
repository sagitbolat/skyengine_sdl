#pragma once
#include <stdint.h>
#include "sky_structs.h"
#include "skymath.h"
#include "skyengine.h"
#include "skyintrinsics.h"



// SECTION: 2D Collision:


// NOTE: Given two Oriented Bounding Boxes (OBBs) in the form of transforms, 
// check if they overlap on a given axis
// NOTE: This is an internal function for the SAT collision test.
bool OverlapOnAxis(Transform obb1, Transform obb2, Vector3 axis) {
    // Project OBB1 onto the axis
    float obb1_min = VectorDot(axis, obb1.position) - VectorDot(axis, obb1.scale);
    float obb1_max = VectorDot(axis, obb1.position) + VectorDot(axis, obb1.scale);

    // Project OBB2 onto the axis
    float obb2_min = VectorDot(axis, obb2.position) - VectorDot(axis, obb2.scale);
    float obb2_max = VectorDot(axis, obb2.position) + VectorDot(axis, obb2.scale);

    // Check for overlap
    if (obb1_max < obb2_min || obb2_max < obb1_min) {
        return false;
    }
    return true;
}


// NOTE: Given two OBBs in the form of transforms, test if they are colliding
// NOTE: Uses the Separating Axis Theorem (SAT).
bool TestCollision(Transform obb1, Transform obb2) {
    // NOTE: A and B are the two colliding rectangles
    Vector3 p_a = obb1.position; // Position of A
    Vector3 a_x = RightVector(obb1); // Local x axis of A
    Vector3 a_y = UpVector(obb1); // Local y axis of A
    float w_a = obb1.scale.x / 2; // Half-width of A
    float h_a = obb1.scale.y / 2; // Half-height of A

    Vector3 p_b = obb2.position; // Position of B
    Vector3 b_x = RightVector(obb2); // Local x axis of B
    Vector3 b_y = UpVector(obb2); // Local  y axis of A
    float w_b = obb2.scale.x / 2; // Half-width of B
    float h_b = obb2.scale.y / 2; // Half-height of B
    
    Vector3 T = p_b - p_a;
    
    // Check against axis a_x:
    float Ta_x = fAbs(VectorDot(T, a_x));
    float Ta_x_test = w_a + fAbs(VectorDot((w_b * b_x), a_x)) + fAbs(VectorDot((h_b * b_y), a_x)); 
    if (Ta_x > Ta_x_test) {
        return false;
    }

    // Check against axis a_y:
    float Ta_y = fAbs(VectorDot(T, a_y));
    float Ta_y_test = h_a + fAbs(VectorDot((w_b * b_x), a_y)) + fAbs(VectorDot((h_b * b_y), a_y)); 
    if (Ta_y > Ta_y_test) {
        return false;
    }

    // Check against axis b_x:
    float Tb_x = fAbs(VectorDot(T, b_x));
    float Tb_x_test = w_b + fAbs(VectorDot((w_a * a_x), b_x)) + fAbs(VectorDot((h_a * a_y), b_x)); 
    if (Tb_x > Tb_x_test) {
        return false;
    }

    // Check against axis a_y:
    float Tb_y = fAbs(VectorDot(T, b_y));
    float Tb_y_test = h_b + fAbs(VectorDot((w_a * a_x), b_y)) + fAbs(VectorDot((h_a * a_y), b_y)); 
    if (Tb_y > Tb_y_test) {
        return false;
    }
    return true;
    // SECTION: old code
    //// Calculating the axes of each OBB
    //Vector3 axes[4] = {
    //    RotateZ({1, 0, 0}, obb1.rotation.z),
    //    RotateZ({0, 1, 0}, obb1.rotation.z),
    //    RotateZ({1, 0, 0}, obb2.rotation.z),
    //    RotateZ({0, 1, 0}, obb2.rotation.z),
    //    //ForwardVector(obb1), RightVector(obb1)
    //};


    //// Project onto axis[i]
    //for (int i = 0; i < 4; ++i) {
    //    //printf("Axis %d: {%f, %f, %f}\n", i, axes[i].x, axes[i].y, axes[i].z);
    //    if (!OverlapOnAxis(obb1, obb1, axes[i])) return false; 
    //}

    //return true;
}