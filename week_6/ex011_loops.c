#include <stdio.h>

int main() {
printf("Using for loop:\n");
for (int i = 1; i <= 10; i++) {
printf("%d\n", i);
}

printf("while loop:\n");
int i = 1;
while (i <= 10) {
printf("%d\n", i);
i++;
}

printf("Using do-while loop:\n");
i = 1;
do 
{
    printf("%d\n", i);
    i++;        
} while (i <= 10);
int sum=0;
i=1;
while(i<=10)
{
    sum += i;
    i++;
}
printf("sum of 1 to 10 is %d\n", sum);
return 0;
}