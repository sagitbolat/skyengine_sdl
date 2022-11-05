#pragma once
/********************************
 *
 * All General Math Operations
 *
 ********************************/

//TODO: implement platfrom specific versions and remove "math.h" dependency
#define _USE_MATH_DEFINES
#include "math.h" 

inline float SquareRoot(float a) {
	return sqrt(a);
}

inline float CosRad(float rad) {
	return cos(rad);
}

inline float CosDeg(float deg) {
	return cos((M_PI / 180) * deg);
}

inline float SinRad(float rad) {
	return sin(rad);
}

inline float SinDeg(float deg) {
	return sin((M_PI / 180) * deg);
}


// NOTE: ModFloats. Different from fmod. This does not return negative numbers at all.
inline float FloatMod(float a, float b) {
    return (a - b * floor(a/b));
}
