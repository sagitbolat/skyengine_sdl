#pragma once
#include <stdint.h>
#include "sky_structs.h"
#include "skyengine.h"
#include "skyintrinsics.h"



// SECTION: 2D Collision:


// NOTE: Given two Oriented Bounding Boxes (OBBs) in the form of transforms, 
// check if they overlap on a given axis
// NOTE: This is an internal function for the SAT collision test.
bool OverlapOnAxis(Transform obb1, Transform obb1, Vector3 axis) {
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
    // Calculating the axes of each OBB
    Vector3 axes[2] = {
        RotateZ((1, 0, 0), obb1.rotation.z),
        RotateZ((0, 1, 0), obb1.rotation.z)
    };


    // Project onto axis[i]
    for (int i = 0; i < 2; ++i) {

        // Project OBB1 onto the axis
        float obb1_min = VectorDot(axes[i], obb1.position) - VectorDot(axes[i], obb1.scale);
        float obb1_max = VectorDot(axes[i], obb1.position) + VectorDot(axes[i], obb1.scale);

        float obb2_min = VectorDot(axes[i], obb2.position) - VectorDot(axes[i], obb2.scale);
        float obb2_max = VectorDot(axes[i], obb2.position) + VectorDot(axes[i], obb2.scale); 


        // Check for overlap
        if (obb1_max < obb2_min || obb2_max < obb1_min) {
            return false; // No overlap on this axis
        }
    }

    return true;
}