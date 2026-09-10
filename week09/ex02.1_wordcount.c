#include <stdio.h>
#define MAX_LEN 200
// 2.1: Word Counter
int wordCount(const char* str) {
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
int main() {
char text[MAX_LEN];
printf("Enter text (spaces allowed): ");
fgets(text, MAX_LEN, stdin);
// fgets keeps the trailing '\n' — find the null terminator
// and overwrite the newline with '\0' if it's there (no
// <string.h>)
int len = 0;
while (text[len] != '\0') len++;
if (len > 0 && text[len - 1] == '\n') {
text[len - 1] = '\0';
}
printf("wordCount(\"%s\") = %d\n", text, wordCount(text));
return 0;
}