#include <stdio.h>

 float lerp(float a, float b, float t);

 int main(void) {
 float t = 0.5f;
 float pos = lerp(0, 100, t);
 printf("t ใน main = %.2f\n", t);
 printf("pos = %.2f\n", pos);
 return 0;
 }

 float lerp(float a, float b, float t) {
 float result = a + (b - a) * t;
 return result;
 }