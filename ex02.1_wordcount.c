#include <stdio.h>
#define MAX_LEN 200
// เดิน pointer ไล่ทีละตัวอักษรจนเจอ '\0' — ไม่ใช้ index เลย:
int wordCount(const char *str) {
int count = 0, inWord = 0;
while (*str) {
if (*str == ' ' || *str == '\t' || *str == '\n') {
inWord = 0;
} else if (!inWord) {
inWord = 1;
count++;
}
str++;
}
return count;
}
int main(void) {
char text[MAX_LEN];
printf("Enter text (spaces allowed): ");
fgets(text, MAX_LEN, stdin);
// fgets เก็บ '\n' ท้ายบรรทัดไว้ — เดิน pointer หาแล้วเขียนทับด้วย '\0' (ไม่ใช้index)
char *p = text;
while (*p != '\0') p++;
if (p != text && *(p - 1) == '\n') *(p - 1) = '\0';
printf("wordCount(\"%s\") = %d\n", text, wordCount(text));
return 0;
}