#include <stdio.h>
int main(){
    printf("=== COMBAT SIMULATOR ===\n");
    int Player_Attack;
    int Enemy_Defense;
    int Hit_Number ;
    float Damage;
    float crit_hit;

    printf("Enter Player Attack: ");
    scanf("%d", &Player_Attack);
    printf("Enter Enemy Defense: ");
    scanf("%d", &Enemy_Defense);
    printf("Enter Hit Number: ");
    scanf("%d", &Hit_Number);

    printf("=== COMBAT SIMULATOR ===\n");
    printf("Base Damage: %d\n", Player_Attack - Enemy_Defense);
    if (Hit_Number % 5 == 0) {
        printf("Critical Hit!\n");
        Damage = (float)(Player_Attack - Enemy_Defense) * 1.5f;
    } else {
        Damage = (float)(Player_Attack - Enemy_Defense);
    }
    printf("Damage: %.1f\n", Damage);
    if (Hit_Number % 5 == 0 &&Hit_Number  != 0) {
        printf("*** CRITICAL HIT! x1.5 ***\n");
        Damage = 0;
    }
    else if (Hit_Number % 5 != 0) {
        printf("Not a critical hit.\n");
    }
    else if (Hit_Number == 0) {
        printf("No hits.\n");
    }

}