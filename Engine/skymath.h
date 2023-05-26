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


inline Vector3 I_Vector3D() {
	return { 1, 0, 0 };
}
inline Vector3 J_Vector3D() {
	return { 0, 1, 0 };
}
inline Vector3 K_Vector3D() {
	return { 0, 0, 1 };
}

inline Vector3 Vec3(float x, float y, float z) {
	return { x, y, z };
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


inline float VectorDot(const Vector3 a, const Vector3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3 VectorCross(const Vector3 a, const Vector3 b){
	return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); 
}

float Magnitude(const Vector3 vec) {	
	return SquareRoot(SQUARE(vec.x) + SQUARE(vec.y) + SQUARE(vec.z)); 
}

Vector3 Normalize(const Vector3 vec) {	
	return (vec / Magnitude(vec)); 
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