#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SAVE_FILE_PATH "save.dat"
typedef struct {
float x, y;
int hp, max_hp, level, score;
} GameData;
// Write game data to path. Returns 1 on success, 0 on failure.
int saveGame(const char* path, const GameData* d) {
FILE* f = fopen(path, "w");
if (!f) return 0;
fprintf(f, "POS:%.1f,%.1f\nHP:%d/%d\nLEVEL:%d\nSCORE:%d\n",
d->x, d->y, d->hp, d->max_hp, d->level, d->score);
fclose(f);
return 1;
}
// Returns 1 on success, 0 if the file is missing,
// -1 if the file is corrupted (data is left untouched in that case).
int loadGame(const char* path, GameData* d) {
FILE* f = fopen(path, "r");
if (!f) return 0;
GameData tmp;
int n = fscanf(f, "POS:%f,%f\nHP:%d/%d\nLEVEL:%d\nSCORE:%d\n",
&tmp.x, &tmp.y, &tmp.hp, &tmp.max_hp, &tmp.level, &tmp.score);
fclose(f);
if (n != 6) return -1;
*d = tmp;
return 1;
}
void printData(const GameData* d) {
printf("POS:%.1f,%.1f HP:%d/%d LEVEL:%d SCORE:%d\n",
d->x, d->y, d->hp, d->max_hp, d->level, d->score);
}
void randomAction(GameData* d) {
switch (rand() % 5) {
case 0: {
int dx = rand() % 41 - 20, dy = rand() % 41 - 20;
d->x += dx;
d->y += dy;
printf("[Move] You walked (%+d,%+d)\n", dx, dy);
break;
}
case 1: {
int dmg = 5 + rand() % 21, gain = 50 + rand() % 101;
d->hp -= dmg;
d->score += gain;
printf("[Fight] Defeated a monster! -%d HP, +%d score\n", dmg, gain);
break;
}
case 2: {
int heal = 10 + rand() % 21;
d->hp += heal;
if (d->hp > d->max_hp) d->hp = d->max_hp;
printf("[Potion] Found a potion, +%d HP\n", heal);
break;
}
case 3: {
int gain = 100 + rand() % 201;
d->score += gain;
printf("[Treasure] Found a chest, +%d score\n", gain);
break;
}
default: {
int dmg = 20 + rand() % 21;
d->hp -= dmg;
printf("[Trap] Ouch! -%d HP\n", dmg);
break;
}
}
if (d->hp <= 0) {
printf("You died! Respawn at (100,100) with full HP, score halved\n");
d->x = 100.0f;
d->y = 100.0f;
d->hp = d->max_hp;
d->score /= 2;
}
// Level up every 400 score per level (Lv.N needs N*400)
while (d->score >= d->level * 400) {
d->level++;
d->max_hp += 10;
d->hp = d->max_hp;
printf("LEVEL UP! Now Lv.%d (max HP %d)\n", d->level, d->max_hp);
}
}
int main() {
srand((unsigned)time(NULL));
GameData data = {234.5f, 189.0f, 85, 100, 7, 2350};
int choice;
while (1) {
printf("1. Save, 2. Load, 3. Random action, 4. Show state, 5. Quit: ");
if (scanf("%d", &choice) != 1) {
// invalid input (e.g. letters): clear the buffer, otherwise the loop never ends
int c;
while ((c = getchar()) != '\n' && c != EOF) {}
if (c == EOF) break;
continue;
}
if (choice == 1) {
if (saveGame(SAVE_FILE_PATH, &data)) printf("Saved to %s\n", SAVE_FILE_PATH);
else printf("Cannot open %s for writing\n", SAVE_FILE_PATH);
} else if (choice == 2) {
printf("Before load: ");
printData(&data);
int result = loadGame(SAVE_FILE_PATH, &data);
if (result == 1) {
printf("Loaded! ");
printData(&data);
} else if (result == 0) {
printf("No save file found (%s)\n", SAVE_FILE_PATH);
} else {
printf("Save file %s is corrupted\n", SAVE_FILE_PATH);
}
} else if (choice == 3) {
randomAction(&data);
printf("Now: ");
printData(&data);
} else if (choice == 4) {
printData(&data);
} else if (choice == 5) {
break;
}
}
return 0;
}