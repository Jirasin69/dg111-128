#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// สุ่มลูกเต๋าแล้วเขียนผลลง *dice โดยตรง (ไม่ return ค่า)
void rollDice(int *dice) {
*dice = (rand() % 6) + 1;
}
// รับคาตอบจากผู้เล่นแล้วเขียนลง *guess โดยตรง
void getGuess(int *guess) {
do {
printf("Guess the number (1-6): ");
scanf("%d", guess);
} while (*guess < 1 || *guess > 6);
}
// เพิ่มตัวนับ round ผ่าน pointer — *round += 1 แทนการ return ค่าใหม่
void nextRound(int *round) {
(*round)++;
printf("\n--- Round %d ---\n", *round);
}
// รับ pointer ไปยัง score โดยตรง — แก้ *score ได้เลยโดยไม่ต้อง return ค่ากลับ
void checkGuess(int dice, int guess, int *score) {
printf("Dice: %d\n", dice);
if (dice == guess) {
printf("Correct! +3 points\n");
*score += 3;
} else if (abs(dice - guess) == 1) {
printf("Close! +1 point\n");
*score += 1;
} else {
printf("Incorrect! No points\n");
}
}
// อ่านอย่างเดียว — const int* ป้องกันไม่ให้ฟังก์ชันนี้แก้ *score โดยไม่ตั้งใจ
void displayScore(const int *score) {
printf("Current score: %d\n", *score);
}
// ถามผู้เล่นแล้วเขียนคาตอบ (เล่นต่อหรือไม่) ลง *keepPlaying โดยตรง
void askPlayAgain(int *keepPlaying) {
char answer;
printf("Do you want to play again? (y/n): ");
scanf(" %c", &answer);
*keepPlaying = (answer == 'y' || answer == 'Y');
}
int main() {
srand((unsigned)time(NULL));
int score = 0, round = 0, dice, guess, keepPlaying;
int *pScore = &score; // pointer เดียวที่ checkGuess และdisplayScore ใช้ร่วมกันทุก round
printf("Welcome to the Dice Guessing Game!\n");
do {
nextRound(&round);
rollDice(&dice);
getGuess(&guess);
checkGuess(dice, guess, pScore);
displayScore(pScore);
askPlayAgain(&keepPlaying);
} while (keepPlaying);
printf("\nGame Over! Played %d round(s) — Your final score:%d\n", round, score);
return 0;
}