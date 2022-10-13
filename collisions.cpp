#include <stdint.h>
#include "skyengine.h"

// NOTE: Tests if the two rectangles are colliding or not.
// Returns: 1 if the rectangles are colliding, 0 if not.
int FRectCollison(FRect r1, FRect r2) {
    if (r1.x < r2.x + r2.width && 
        r1.x + r1.width > r2.x &&
        r1.y < r2.y + r2.height &&
        r1.y + r2.height > r2.y
    ) {
        return 1;
    } else {
        return 0;
    }
}
