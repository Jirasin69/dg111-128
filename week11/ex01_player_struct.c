#include <stdio.h>
#include <string.h>
typedef struct {
char name[30];
int hp, max_hp, attack, defense, level, gold;
} Player;
Player createPlayer(const char* name, int hp, int atk, int def, int level) {
Player p;
strncpy(p.name, name, sizeof(p.name) - 1);
p.name[sizeof(p.name) - 1] = '\0';
p.hp = hp;
p.max_hp = hp;
p.attack = atk;
p.defense = def;
p.level = level;
p.gold = 0;
return p;
}
void displayPlayer(const Player* p) {
printf("Name: %s\n", p->name);
printf("Level: %d\n", p->level);
printf("HP: %d/%d\n", p->hp, p->max_hp);
printf("ATK: %d\n", p->attack);
printf("DEF: %d\n", p->defense);
printf("Gold: %d\n", p->gold);
}
// level + 1, attack/defense +10%, max_hp + 25
void levelUp(Player* p) {
p->level += 1;
p->attack = p->attack * 110 / 100;
p->defense = p->defense * 110 / 100;
p->max_hp += 25;
}
// returns 1 if hp > 0, otherwise 0
int isAlive(const Player* p) {
return p->hp > 0;
}
// reduce hp by dmg, clamped at 0
void takeDamage(Player* p, int dmg) {
p->hp -= dmg;
if (p->hp < 0) {
p->hp = 0;
}
}
int main() {
Player p = createPlayer("Dragon Knight", 100, 55, 40, 7);
p.gold = 2350;
takeDamage(&p, 15);
displayPlayer(&p);
printf("Alive: %s\n", isAlive(&p) ? "yes" : "no");
printf("\n--- Level Up ---\n");
levelUp(&p);
displayPlayer(&p);
printf("\n--- Take 999 damage ---\n");
takeDamage(&p, 999);
displayPlayer(&p);
printf("Alive: %s\n", isAlive(&p) ? "yes" : "no");
printf("\nPress Enter to exit...");
getchar();
return 0;
}