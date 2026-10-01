// dungeon.c — เกมเดียวกับเวอร์ชัน multi-file แต่รวมทุกอย่างไว้ไฟล์เดียว
// ไม่ต้องมี .h / header guard เลย เพราะมีไฟล์เดียว ไม่มีใคร #include ซ้ำ
// ข้อเสีย: struct, logic, render, save ปนกันหมด — ยิ่งเกมใหญ่ขึ้นยิ่งหาโค้ดยาก
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h> // kbhit()/getch() — เฉพาะ Windows, ใช้แบบเดียวกับ src/04-sample-console-games/Tetris/main.c

#define MAX_NAME    32
#define MAX_ENEMIES 2
#define ROWS        6
#define COLS        10
#define SAVE_FILE   "dungeon_save.bin"

// ── struct ──────────────────────────────────────────────────

typedef struct {
    char name[MAX_NAME];
    int  hp, max_hp, attack, defense;
    int  gold, level, exp;
    int  row, col;
} Player;

typedef struct {
    char name[MAX_NAME];
    int  hp, attack, row, col;
    int  active;          // 1 = ยังไม่ตาย
} Enemy;

typedef struct {
    Player player;
    Enemy  enemies[MAX_ENEMIES];
    int    floor;
    int    running;        // 1 = playing, 0 = game over
    char   message[128];
} GameState;

// 0 = floor, 1 = wall, 2 = stairs down
int tilemap[ROWS][COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

static const int enemySpawnRow[MAX_ENEMIES] = {1, 4};
static const int enemySpawnCol[MAX_ENEMIES] = {8, 2};

// ── game logic ──────────────────────────────────────────────
// (ทุกฟังก์ชันเขียน "ก่อน" จุดที่เรียกใช้ — ไฟล์เดียวไม่มี prototype ก็คอมไพล์ผ่าน)

static void generateEnemy(Enemy *e, int floor) {
    snprintf(e->name, MAX_NAME, "Slime Lv.%d", floor);
    e->hp = 10 + floor * 4;
    e->attack = 3 + floor * 2;
    e->active = 1;
}

static void spawnFloorEnemies(GameState *g) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        generateEnemy(&g->enemies[i], g->floor);
        g->enemies[i].row = enemySpawnRow[i];
        g->enemies[i].col = enemySpawnCol[i];
    }
}

void initGame(GameState *g, const char *name) {
    memset(g, 0, sizeof(GameState));
    strncpy(g->player.name, name, MAX_NAME - 1);
    g->player.hp = 20;
    g->player.max_hp = 20;
    g->player.attack = 6;
    g->player.defense = 2;
    g->player.row = 1;
    g->player.col = 1;
    g->floor = 1;
    g->running = 1;
    strcpy(g->message, "Walk into an enemy to fight. Reach '>' to descend.");
    srand((unsigned)time(NULL));
    spawnFloorEnemies(g);
}

static void doCombat(GameState *g, Enemy *e) {
    while (e->hp > 0 && g->player.hp > 0) {
        e->hp -= g->player.attack;
        if (e->hp <= 0)
            break;
        int dmg = e->attack - g->player.defense;
        g->player.hp -= (dmg > 0 ? dmg : 1);
    }

    if (g->player.hp <= 0) {
        g->player.hp = 0;
        g->running = 0;
        strcpy(g->message, "You were defeated...");
        return;
    }

    e->active = 0;
    snprintf(g->message, sizeof(g->message), "Defeated %s!", e->name);
}

static void walkEnemies(GameState *g) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &g->enemies[i];
        if (!e->active)
            continue;

        int dRow = (g->player.row > e->row) - (g->player.row < e->row);
        int dCol = (g->player.col > e->col) - (g->player.col < e->col);

        if (dRow != 0 && tilemap[e->row + dRow][e->col] != 1)
            e->row += dRow;
        else if (dCol != 0 && tilemap[e->row][e->col + dCol] != 1)
            e->col += dCol;
    }
}

static int checkCollisions(GameState *g) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &g->enemies[i];
        if (e->active && e->row == g->player.row && e->col == g->player.col) {
            doCombat(g, e);
            return 1;
        }
    }
    return 0;
}

static void tryMove(GameState *g, int dRow, int dCol) {
    int nr = g->player.row + dRow;
    int nc = g->player.col + dCol;

    if (tilemap[nr][nc] == 1) {
        strcpy(g->message, "Blocked by a wall!");
        return;
    }

    g->player.row = nr;
    g->player.col = nc;
    strcpy(g->message, "You moved.");

    walkEnemies(g);
    int fought = checkCollisions(g);

    if (!fought && g->running && tilemap[g->player.row][g->player.col] == 2) {
        g->floor++;
        spawnFloorEnemies(g);
        g->player.row = 1;
        g->player.col = 1;
        snprintf(g->message, sizeof(g->message), "Descended to floor %d", g->floor);
    }
}

void updateGame(GameState *g, char cmd) {
    switch (cmd) {
    case 'w':
    case 'W':
        tryMove(g, -1, 0);
        break;
    case 's':
    case 'S':
        tryMove(g, 1, 0);
        break;
    case 'a':
    case 'A':
        tryMove(g, 0, -1);
        break;
    case 'd':
    case 'D':
        tryMove(g, 0, 1);
        break;
    case 'v':
    case 'V':
        strcpy(g->message, "Game saved.");
        break;
    case 'q':
    case 'Q':
        g->running = 0;
        strcpy(g->message, "Game ended");
        break;
    default:
        strcpy(g->message, "Invalid command (wasd/V/Q)");
    }
}

// ── render (display) ────────────────────────────────────────

static void printBar(int cur, int max, int width) {
    if (max < 1)
        max = 1;
    int filled = (cur * width) / max;
    printf("[");
    for (int i = 0; i < width; i++)
        printf("%c", i < filled ? '#' : ' ');
    printf("] %d/%d", cur, max);
}

static void drawMap(const GameState *g) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            int enemyHere = 0;
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (g->enemies[i].active && g->enemies[i].row == r && g->enemies[i].col == c) {
                    enemyHere = 1;
                    break;
                }
            }

            if (g->player.row == r && g->player.col == c)
                printf("@");
            else if (enemyHere)
                printf("E");
            else if (tilemap[r][c] == 1)
                printf("#");
            else if (tilemap[r][c] == 2)
                printf(">");
            else
                printf(".");
        }
        printf("\n");
    }
}

void renderGame(const GameState *g) {
    system("cls");   // ล้างจอก่อนวาดเฟรมใหม่ทุกครั้ง (Linux/Mac ใช้ system("clear"))
    drawMap(g);
    printf("\n %s  Lv.%d  Floor:%d\n", g->player.name, g->player.level, g->floor);
    printf(" HP: ");
    printBar(g->player.hp, g->player.max_hp, 20);
    printf("\n %s\n", g->message);
    printf(" [wasd/arrows] Move   [V]Save   [Q]uit (no Enter needed)\n> ");
}

void renderGameOver(const GameState *g) {
    printf("\n=== GAME OVER ===\n");
    printf("Player: %s   Floor reached: %d\n", g->player.name, g->floor);
}

// ── input ───────────────────────────────────────────────────
// แยกออกจาก render โดยเจตนา — "วาดจอ" กับ "รับคำสั่งผู้เล่น" เป็นคนละหน้าที่
// Week 13 เปลี่ยนแค่ฟังก์ชันนี้เป็น IsKeyPressed() ส่วน render/game ไม่ต้องแตะ

// รับชื่อผู้เล่นตอนเริ่มเกม — แยกเป็นฟังก์ชันเหมือน getPlayerInput()
// แทนที่จะ scanf ลอยๆ ใน main() ทำให้ main() อ่านเป็นขั้นตอนที่เป็นธรรมชาติ
// (ชื่อเป็นข้อความยาว ต้องพิมพ์แล้วกด Enter อยู่ดี ต่างจาก getPlayerInput() ด้านล่าง)
void getPlayerName(char *name) {
    printf("Enter your name: ");
    scanf("%31s", name);
}

// getch() รับคีย์ทันที "โดยไม่ต้องกด Enter" (conio.h, เฉพาะ Windows)
// เหมือนกับที่ Tetris ใช้ — รองรับปุ่มลูกศรด้วย (มาเป็น 2 byte: prefix แล้วตามด้วยรหัสทิศทาง)
char getPlayerInput(void) {
    int key = getch();
    if (key == 0 || key == 224) {
        int code = getch();
        key = (code == 75) ? 'a' : (code == 77) ? 'd' : (code == 72) ? 'w' : (code == 80) ? 's' : 0;
    }
    return (char)key;
}

// ── save / load (File I/O) ────────────────────────────────────

int saveGame(const GameState *g, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f)
        return 0;
    fwrite(g, sizeof(GameState), 1, f);
    fclose(f);
    return 1;
}

int loadGame(GameState *g, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f)
        return 0;
    fread(g, sizeof(GameState), 1, f);
    fclose(f);
    return 1;
}

// ── main game loop ──────────────────────────────────────────

int main(void) {
    GameState game;
    char name[MAX_NAME];

    printf("=== Dungeon Explorer ===\n");
    getPlayerName(name);
    initGame(&game, name);

    while (game.running) {
        renderGame(&game);           // DRAW
        char cmd = getPlayerInput(); // INPUT
        updateGame(&game, cmd);      // UPDATE
        if (cmd == 'v' || cmd == 'V')
            saveGame(&game, SAVE_FILE); // save เฉพาะตอนผู้เล่นสั่ง (กด V)
    }

    renderGameOver(&game);
    return 0;
}
