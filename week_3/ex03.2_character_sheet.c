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

        printf("╔════════════════════════════╗");
        printf("\n║     DRAGONKNIGHT           ║\n");
        printf("╠════════════════════════════╣");
        printf("\n║Name: %-22s║\n", name);
        printf("║HP: %-1d /100                ║\n", HP);
        printf("║ATK: %-22d ║\n", Attack_power);
        printf("║DEF: %-22d ║\n", Defense);
        printf("║Level: %-20d ║\n", level);
        printf("╠════════════════════════════╣\n");
        printf("║HP Bar: [█████████████] 100%║\n");
        printf("║Power Score: %-14d ║\n", (Attack_power * 2 + Defense + HP / 10) * level);
        printf("╚════════════════════════════╝");
        return 0;
    }