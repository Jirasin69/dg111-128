#include <stdio.h>

int main() {
    char name[50];
    int age;
    double GPA;
    char Major [50];
    printf("=== Enter Data ===\n");
    printf("Name: ");
    scanf("%s", name);
    printf("Age: ");
    scanf("%d", &age);
    printf("GPA: ");
    scanf("%lf", &GPA);
    printf("Major: ");
    scanf("%s", Major);

    printf("┌──────────────────────────────────┐");
    printf("\n│=== YOUR PERSONAL CARD ===        │\n");
    printf("│Name: %-27.9s │\n", name);
    printf("│Age: %-1d Year Old                   │\n", age);
    printf("│GPA: %-28.2lf │\n", GPA);
    printf("│Major: %-26s │\n", Major);
    printf("└──────────────────────────────────┘");
    return 0;  
}