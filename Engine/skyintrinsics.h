#pragma once
/********************************
 *
 * All General Math Operations
 *
 ********************************/

//TODO: implement platfrom specific versions and remove "math.h" dependency
#define _USE_MATH_DEFINES
#include "math.h" 

inline int Abs(int a) {
    return abs(a);
}
inline float fAbs(float a) {
    return fabs(a);
}

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

inline float TanRad(float rad) {
    return tan(rad);
}

inline float TanDeg(float deg) {
    return tan((M_PI / 180) * deg);
}

// NOTE: ModFloats. Different from fmod. This does not return negative numbers at all.
inline float FloatMod(float a, float b) {
    return (a - b * floor(a/b));
}

// NOTE: Returns the min of a and b. If a == b then Returns a.
inline int IntMin(int a, int b) {
    return a<b?a:b;
}
// NOTE: Returns the max of a and b. If a == b then Returns a.
inline int IntMax(int a, int b) {
    return a>=b?a:b;
}

// NOTE: Clamps value "val" between "min" and "max" and returns it. Original variable value is unchanged
inline int IntClamp(int val, int min, int max) {
    return (val < min) ? min : ((val > max) ? max : val);
}
inline float FloatClamp(float val, float min, float max) {
    return (val < min) ? min : ((val > max) ? max : val);
}
