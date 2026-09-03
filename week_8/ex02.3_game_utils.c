#include <stdio.h>
 #include <stdlib.h>
 #include <time.h>

 int randomRange(int min, int max);

 int main(void) {
 srand((unsigned int)time(NULL));
 int diceMin = 1;
 for (int i = 0; i < 5; i++) {
 printf("dice roll: %d\n", randomRange(diceMin, 6));
 }
 printf("diceMin เดิม = %d\n", diceMin);
 return 0;
 }

 int randomRange(int min, int max) {
 return rand() % (max - min + 1) + min;
 }