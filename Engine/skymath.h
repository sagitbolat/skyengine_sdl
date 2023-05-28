#pragma once
/********************************
 *
 * Intrinsic functions declarations.
 *
 ********************************/
#include "skyintrinsics.h"
#include "sky_structs.h"

/********************************
 *
 * Vector Math
 *
 ********************************/

#define SQUARE(a) ((a)*(a))
#define CUBE(a) ((a)*(a)*(a))


// SECTION: 2D Vectors

inline Vector2 I_Vector2() {
	return { 1, 0};
}
inline Vector3 J_Vector2() {
	return { 0, 1};
}

inline Vector2 Vec2(float x, float y) {
	return { x, y};
}
inline Vector2 Vec2(Vector2 v) {
	return { v.x, v.y};
}
inline Vector2 Vec2(Vector3 v) {
	return { v.x, v.y}; // NOTE: This initialization drops the z component of a vector.
}

inline Vector2 operator+(const Vector2 a, const Vector2 b) {
	return Vector2{ a.x + b.x, a.y + b.y};
}
inline Vector2 operator-(const Vector2 a, const Vector2 b) {
	return Vector2{ a.x - b.x, a.y - b.y};
}
inline Vector2 operator*(const Vector2 a, const float b) {
	return Vector2{ a.x * b, a.y * b};	
}
inline Vector2 operator*(const float b, const Vector2 a) {
	return a * b;	
}
inline Vector2 operator/(const Vector2 a, const float b) {
	return Vector2{ a.x / b, a.y / b}; 
}

// SECTION: 3D Vectors

inline Vector3 I_Vector3() {
	return { 1, 0, 0 };
}
inline Vector3 J_Vector3() {
	return { 0, 1, 0 };
}
inline Vector3 K_Vector3() {
	return { 0, 0, 1 };
}

inline Vector3 Vec3(float x, float y, float z) {
	return { x, y, z };
}
inline Vector3 Vec3(Vector2 v, float z) {
	return {v.x, v.y, z};
}
inline Vector3 Vec3(Vector3 v) {
	return { v.x, v.y, v.z };
}

inline Vector3 operator+(const Vector3 a, const Vector3 b) {
	return Vector3{ a.x + b.x, a.y + b.y, a.z + b.z };
}
inline Vector3 operator-(const Vector3 a, const Vector3 b) {
	return Vector3{ a.x - b.x, a.y - b.y, a.z - b.z };
}
inline Vector3 operator*(const Vector3 a, const float b) {
	return Vector3{ a.x * b, a.y * b, a.z * b };	
}
inline Vector3 operator*(const float b, const Vector3 a) {
	return a * b;	
}
inline Vector3 operator/(const Vector3 a, const float b) {
	return Vector3{ a.x / b, a.y / b, a.z / b }; 
}



// SECTION: Standard Vector Functions

inline float VectorDot(const Vector2 a, const Vector2 b) {
	return a.x * b.x + a.y * b.y;
}
inline float VectorDot(const Vector3 a, const Vector3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}


// NOTE: The cross product is not a valid operation in 2D space
inline Vector3 VectorCross(const Vector3 a, const Vector3 b){
	return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); 
}

float Magnitude(const Vector2 vec) {
	return SquareRoot(SQUARE(vec.x) + SQUARE(vec.y));
}
float Magnitude(const Vector3 vec) {	
	return SquareRoot(SQUARE(vec.x) + SQUARE(vec.y) + SQUARE(vec.z)); 
}

Vector2 Normalize(const Vector2 vec) {	
	return (vec / Magnitude(vec)); 
}
Vector3 Normalize(const Vector3 vec) {	
	return (vec / Magnitude(vec)); 
}


// SECTION: Rotation about an axis:

// NOTE: Rotates a vector about the X axis by an angle and returns it:
Vector3 RotateX(Vector3 v, float angle_deg) {
	float cos_angle = CosDeg(angle_deg);
	float sin_angle = SinDeg(angle_deg);

	return Vector3{
		v.x,
		cos_angle * v.y - sin_angle * v.z,
		sin_angle * v.y + cos_angle * v.z
	};
}
// NOTE: Rotates a vector about the Y axis by an angle and returns it:
Vector3 RotateY(Vector3 v, float angle_deg) {
	float cos_angle = CosDeg(angle_deg);
	float sin_angle = SinDeg(angle_deg);

	return Vector3{
		cos_angle * v.x + sin_angle * v.z,
		v.y,
		cos_angle * v.z - sin_angle * v.x
	};
}
// NOTE: Rotates a vector about the Z axis by an angle and returns it:
Vector3 RotateZ(Vector3 v, float angle_deg) {
	float cos_angle = CosDeg(angle_deg);
	float sin_angle = SinDeg(angle_deg);

	return Vector3{
		cos_angle * v.x - sin_angle * v.y,
		sin_angle * v.x + cos_angle * v.y,
		v.z
	};
}

// SECTION: More utils

// NOTE: Returns a rotation (euler angles) vector that represents the rotation that rotates a transform to
// point toward the target position 
// EX: Vector2LookAt({0, 0}, {1, 1}) will return {0, 0, 45}.
//                  . < (1, 1, 0)
//                .
//              .    <  This angle is 45 degrees
//(0, 0, 0) > .
// NOTE: This will always return a rotation with x and y components of 0, since 2D rotation is always about the
// z axis (to and away from camera).
Vector3 Vector2LookAt(Vector2 transform_position, Vector2 target_position) {
	Vector3 r = {0.0f, 0.0f, 0.0f};

	Vector2 diff = target_position - transform_position;
	
	r.z = ATan2Deg(diff.y, diff.x) - 90.0f;

	return r;
}

/********************************
 *
 * Matrix Math
 *
 ********************************/

Vector3 operator*(const Matrix3 m, const Vector3 v) {
	Vector3 r = {0.0f, 0.0f, 0.0f};

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			r.E[i] += v.E[j] * m.E[i][j];
		}
	}
	return r;	
}


// NOTE: For internal use only. Used for the forward vector.
Matrix3 RotationMatrix(Transform transform) {
	float Sx = SinDeg(transform.rotation.x);
    float Sy = SinDeg(transform.rotation.y);
    float Sz = SinDeg(transform.rotation.z);
    float Cx = CosDeg(transform.rotation.x);
    float Cy = CosDeg(transform.rotation.y);
    float Cz = CosDeg(transform.rotation.z);

	Matrix3 R = {0.0f};

	R.E[0][0]=Cy*Cz;
	R.E[0][1]=-Cy*Sz;
	R.E[0][2]=Sy;
	R.E[1][0]=Cz*Sx*Sy+Cx*Sz;
	R.E[1][1]=Cx*Cz-Sx*Sy*Sz;
	R.E[1][2]=-Cy*Sx;
	R.E[2][0]=-Cx*Cz*Sy+Sx*Sz;
	R.E[2][1]=Cz*Sx+Cx*Sy*Sz;
	R.E[2][2]=Cx*Cy;
	
	return R;
}
// SECTION: Direction Vectors
// NOTE: RETURN the transform's direction vectors (normalized):

// Forward is the negative z direction (away from camera)
Vector3 ForwardVector(Transform transform) {
	return RotationMatrix(transform) * Vector3{0, 0, -1}; 
}
// Backward is the positive z direction (toward camera) 
Vector3 BackwardVector(Transform transform) {
	return RotationMatrix(transform) * Vector3{0, 0, 1}; 
}
// Up is the positive y direction
Vector3 UpVector(Transform transform) {
	return RotationMatrix(transform) * Vector3{0, 1, 0}; 
}
// Down is the negative y direction
Vector3 DownVector(Transform transform) {
	return RotationMatrix(transform) * Vector3{0, -1, 0}; 
}
// Right is the positive x direction
Vector3 RightVector(Transform transform) {
	return RotationMatrix(transform) * Vector3{1, 0, 0}; 
}
// Left is the negative x direction
Vector3 LeftVector(Transform transform) {
	return RotationMatrix(transform) * Vector3{-1, 0, 0}; 
}