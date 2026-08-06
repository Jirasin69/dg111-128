#include <stdio.h>

int main() {
    printf("1: Triangle\n");
    for (int i = 1; i <= 5; i++) {
        for (int j = 1; j <= i; j++) 
        {
            printf("*");
        }
        printf("\n");
    }
    printf("2: Square (n=4)\n");  
    int n = 4;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            printf("*");
        }
        printf("\n");
    }
    printf("3: Diamond (n=3)\n");
    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= n - i; j++) {
            printf(" ");
        }
        for (int j = 1; j <= 2 * i - 1; j++) {
            printf("*");
        }
        printf("\n");
    }
    for (int i = 2; i >= 1 ; i--) {
        for (int j = 1; j <= n - i; j++) {
            printf(" ");
        }
        for (int j = 1; j <= 2 * i - 1; j++) {
            printf("*");
        }
        printf("\n");
    }
    return 0;
}