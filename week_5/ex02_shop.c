#include <stdio.h>

int main() {
    int item;
    int Gold = 1000; // Starting gold for the player
    typedef enum {
        Health_Potion = 1,
        Mana_Potion,
        Iron_Sword,
        Leather_Armor,
    } Item;
    printf("Welcome to the shop!\n");
    printf("What would you like to buy?\n");
    printf("1. ❤️ Health Potion (50 Gold) (+50 HP)\n");
    printf("2. 💧 Mana Potion (80 Gold) (+30 MP)\n");
    printf("3. ⚔️ Iron Sword (500 Gold) (+20 ATK)\n");
    printf("4. 🛡️ Leather Armor (300 Gold) (+15 DEF)\n");
    printf("Enter your choice: ");
    int Shop;
    scanf("%d", &Shop);
    switch (Shop) {
        case 1:
            printf("You have purchased a Health Potion for 50 Gold.\n");
            printf("Gold remaining: %d Gold\n", Gold - 50);
            printf("You gained +50 HP!\n");
            break;
        case 2:
            printf("You have purchased a Mana Potion for 80 Gold.\n");
            printf("Gold remaining: %d Gold\n", Gold - 80);
            printf("You gained +30 MP!\n");
            break;
        case 3:
            printf("You have purchased an Iron Sword for 500 Gold.\n");
            printf("Gold remaining: %d Gold\n", Gold - 500);
            printf("You gained +20 ATK!\n");
            break;
        case 4:
            printf("You have purchased a Leather Armor for 300 Gold.\n");
            printf("Gold remaining: %d Gold\n", Gold - 300);
            printf("You gained +15 DEF!\n");
            break;
        default:
            printf("Invalid item selection.\n");
            break;
    }
   printf("Thank you for visiting the shop!\n");
    return 0;
}