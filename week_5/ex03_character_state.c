#include <stdio.h>

int main() {

    int max_hp;
    int Get_damage;
    int temp;
    _Bool poisoned;
    int attack_count;

    printf("HP: ");
    scanf("%d", &max_hp);
    printf("Get damage: ");
    scanf("%d", &Get_damage);
    printf("Poisoned: ");
    scanf("%d", &temp);
    poisoned = temp ? 1 : 0;
    printf("Attack count: ");
    scanf("%d", &attack_count);

    printf("=== Character Status ===\n");
    int hp = max_hp - Get_damage;
        if (hp < 0) hp = 0; 

    if (hp <= 0)
    {
        printf("DIE!\n");
    }
    else if (poisoned == 1)
    {
        printf("POISONED\n");
    }
    else if (hp * 100 / max_hp < 25 && hp > 0)
    {
        printf("CRITICAL\n");
    }
    else
    {
        printf("NORMAL\n");
    }
    
    int filled = hp * 10 / max_hp;
    printf("HP : [");
    for (int i = 0; i < 10; i++) {
    printf(i < filled ? "█" : "-");
    }
    printf("] %d/%d", hp, max_hp);
    
    if (attack_count %5 ==0) {
        printf("\nUltimate! ready ");
    }
    else {
        printf("\n!!!NO Ultimate! ready!!!");
    }
    return 0;
}
