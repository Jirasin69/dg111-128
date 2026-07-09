#include <stdio.h>

int main() {
    char name[50];
    int age;
    double GPA;
    char Subject[50];
    printf("=== Enter Data ===\n");
    printf("Name: ");
    scanf("%s", name);
    printf("Age: ");
    scanf("%d", &age);
    printf("GPA: ");
    scanf("%lf", &GPA);
    printf("Favorite Subject: ");
    scanf("%s",Subject );

  
    printf("\n=== YOUR PERSONAL CARD ===\n");
    printf("Name: %s\n", name);
    printf("Age: %d Year Old\n", age);
    printf("GPA: %.2lf\n", GPA);
    printf("Favorite Subject: %s\n",Subject );
    
    

  return 0;  
}