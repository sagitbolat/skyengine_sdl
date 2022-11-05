#include "skyintrinsics.h"
#include <stdio.h>

void RotateSize(int* h, int* w, float angle); 
int main() {
    

    int w = 10;
    int h = 6;
    int w2 =10;
    int h2 = 6;
    RotateSize(&h, &w, -88.0);
    RotateSize(&h2, &w2, 88.0);

    printf("%d %d %d %d\n", w, h, w2, h2);

    return 0;
}

void RotateSize(int* h, int* w, float angle) {
    int rotated_width = *w;
    int rotated_height = *h;
    
    float cosine = CosDeg(angle);
    float sine = SinDeg(angle);
    
    
    angle = FloatMod(angle, 360.0f);
    
    printf("Angle: %f\n", angle);

    if (angle < 90) {
        rotated_width = (*w * cosine) + (*h * sine);
        rotated_height = (*w * sine) + (*h * cosine);
    } else if (angle == 90) {
        rotated_width = *h;
        rotated_height = *w;
    } else if (angle < 180) {
        int h_prime = *w;
        int w_prime = *h;
        float theta = angle - 90;
        
        float cos_theta = CosDeg(theta);
        float sin_theta = SinDeg(theta);

        rotated_width = (w_prime * cos_theta) + (h_prime * sin_theta);
        rotated_height = (w_prime * sin_theta) + (h_prime * cos_theta);
    } else if (angle == 180) {
       // NOTE: Do nothing, since rotating by 180 degrees does not change size. 
    } else if (angle < 270) {
        int h_prime = *w;
        int w_prime = *h;
        float theta = 180 - (angle - 180) - 90;
        
        float cos_theta = CosDeg(theta);
        float sin_theta = SinDeg(theta);

        rotated_width = (w_prime * cos_theta) + (h_prime * sin_theta);
        rotated_height = (w_prime * sin_theta) + (h_prime * cos_theta);
    } else if (angle == 270) {
        rotated_width = *h;
        rotated_height = *w;
    } else {
        float theta = 360.0f - angle;
        float cosine_prime = CosDeg(theta);
        float sine_prime = SinDeg(theta);
        rotated_width = (*w * cosine_prime) + (*h * sine_prime);
        rotated_height = (*w * sine_prime) + (*h * cosine_prime);
    }
    *w = rotated_width;
    *h = rotated_height;
}
