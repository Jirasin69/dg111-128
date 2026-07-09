#include <stdio.h>
    int main() {
        char name[50];
        int HP;
        int Attack_power;
        int Defense;
        int level;
        printf("=== Create Character ===\n");
        printf("Name: ");
        scanf("%s", name);
        printf("HP: ");
        scanf("%d", &HP); 
        printf("Attack Power: ");
        scanf("%d", &Attack_power);
        printf("Defense: ");
        scanf("%d", &Defense);
        printf("Level: ");
        scanf("%d", &level);

        printf("\n=== Character Summary ===\n");
        printf("Name: %s\n", name);
        printf("HP: %d\n", HP);
        printf("Attack Power: %d\n", Attack_power);
        printf("Defense: %d\n", Defense);
        printf("Level: %d\n", level);
        return 0;
    }