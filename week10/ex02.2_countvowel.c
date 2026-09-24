#include <stdio.h>
#include <ctype.h>
#define MAX 50
// เดิน pointer เช่นกัน — เทียบกับเวอร์ชัน index ของ Week 9:
int countVowel(const char *t) {
int count = 0;
while (*t != '\0') {
char c = toupper(*t);
if (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c =='U')
count++;
t++;
}
return count;
}
int main(void) {
char text[MAX];
printf("Enter text: ");
scanf("%s", text);
int cVowel = countVowel(text);
printf("Text : [%s] has %d vowels\n", text, cVowel);
return 0;
}