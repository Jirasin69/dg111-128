#include <stdio.h>
int main()
{
    int x;
    printf("x: ");
    scanf("%d", &x);
    int *ptr = &x;
    printf("x = %d\n", x);
    printf("&x = %p\n", (void *)&x);
    printf("ptr = %p\n", (void *)ptr);
    printf("*ptr = %d\n", *ptr);
    *ptr = 100;
    printf("x after modifying via pointer = %d\n", x);
    return 0;
}