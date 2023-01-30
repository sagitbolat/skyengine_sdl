#pragma once
#include <limits.h>
#include <float.h>

int ArrMin(int* arr, size_t len) {
    int imin = INT_MAX;
    for (size_t i = 0; i < len; ++i) {
        if (arr[i] < imin) {
            imin = arr[i];
        }
    }
    return imin; 
}
int ArrMax(int* arr, size_t len) {
    int imax = INT_MIN;
    for (size_t i = 0; i < len; ++i) {
        if (arr[i] > imax) {
            imax = arr[i];
        }
    }
    return imax; 
}
int ArrMin(float* arr, size_t len) {
    float fmin = FLT_MAX;
    for (size_t i = 0; i < len; ++i) {
        if (arr[i] < fmin) {
            fmin = arr[i];
        }
    }
    return fmin; 
}
int ArrMax(float* arr, size_t len) {
    float fmax = FLT_MIN;
    for (size_t i = 0; i < len; ++i) {
        if (arr[i] > fmax) {
            fmax = arr[i];
        }
    }
    return fmax; 
}


struct Array2D { // NOTE: Represents a 2 dimensional non-jagged array.
    int width; // NOTE: The width of one line
    int size; // NOTE: Equivalent to width * height of the array
    void* array_data;
};
