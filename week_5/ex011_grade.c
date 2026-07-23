#include <stdio.h>

int main() {
    int grade;

    printf("Enter your grade (0-100): ");
    scanf("%d", &grade);

    if (grade >= 80) {
        printf("Your letter grade is: A (4.0)\n");
    } else if (grade >= 75) {
        printf("Your letter grade is: B+ (3.5)\n");
    } else if (grade >= 70) {
        printf("Your letter grade is: B (3.0)\n");
    } else if (grade >= 65) {
        printf("Your letter grade is: C+ (2.5)\n");
    } else if (grade >= 60) {
        printf("Your letter grade is: C (2.0)\n");
    } else if (grade >= 55) {
        printf("Your letter grade is: D+ (1.5)\n");
    } else if (grade >= 50) {
        printf("Your letter grade is: D (1.0)\n");
    }else {
        printf("Your letter grade is: F (0.0)\n");
    }

   if (grade >= 75) {
        printf("You have passed the course.\n");
    } else {
        printf("You have failed the course.\n");
    }
    return 0;
}