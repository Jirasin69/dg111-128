#include <stdio.h>
#define MAX 50
int countVowel(char[]);
int main(void) {
char text[MAX];
int cVowel;
printf("Enter text : ");
scanf("%s", text);
cVowel = countVowel(text);
printf("Text : [%s] has %d vowels\n", text, cVowel);
return 0;
}
int countVowel(char t[]) {
int i = 0, count = 0;
while (i < MAX && t[i] != '\0') {
if (t[i] == 'A' || t[i] == 'a' || t[i] == 'E' || t[i] ==
'e' || t[i] == 'I' || t[i] == 'i' ||
t[i] == 'O' || t[i] == 'o' || t[i] == 'U' || t[i] ==
'u')
count++;
i++;
}
return count;
}