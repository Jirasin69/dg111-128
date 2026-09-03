#include <stdio.h>

 int clamp(int value, int min, int max);

 int main(void) {
 int hp = 150;
 int safeHP = clamp(hp, 0, 100);
 printf("hp เดิม = %d\n", hp);
 printf("safeHP = %d\n", safeHP);
 return 0;
 }

 int clamp(int value, int min, int max) {
 if (value > max) value = max;
 if (value < min) value = min;
 return value;
 }