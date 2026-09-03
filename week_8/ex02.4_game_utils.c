#include <stdio.h>

 float percentOf(int current, int total);

 int main(void) {
 int score = 35, total = 100;
 float percent = percentOf(score, total);
 printf("score/total เดิม = %d/%d\n", score, total);
 printf("percent = %.1f%%\n", percent);
 return 0;
 }

 float percentOf(int current, int total) {
 return (float)current / total * 100.0f;
 }