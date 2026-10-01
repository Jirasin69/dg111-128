// dungeon_raylib.c — เกมเดียวกับ Week 12 (dungeon.c) แต่เปลี่ยน render + input เป็น Raylib
// โครงสร้างไฟล์/ฟังก์ชันอ้างอิงจาก docs/02-lecture/week_12/src/dungeon_single_file/README.md
// struct + game logic (setup/logic) เหมือนเดิมทุกฟังก์ชัน — เปลี่ยนแค่ render/input/main
// ตามที่ Week 12 บอกไว้: "Week 13 เปลี่ยนแค่ฟังก์ชันนี้เป็น IsKeyPressed() ส่วน game logic ไม่ต้องแตะ"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"

#define MAX_NAME    32
#define MAX_ENEMIES 2
#define ROWS        6
#define COLS        10
#define SAVE_FILE   "dungeon_save.bin"

#define TILE        48
#define SCREEN_W    (COLS * TILE)
#define SCREEN_H    (ROWS * TILE + 120)   // + พื้นที่ HUD ด้านล่าง

// ── struct (เหมือน Week 12 ทุกฟิลด์) ──────────────────────────

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

// ── game logic (เหมือน Week 12 ทุกฟังก์ชัน — ไม่รู้จัก Raylib เลย) ──

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

// ── render (display) — เปลี่ยนจาก printf เป็น Raylib ──────────
// game logic ด้านบนไม่ถูกแก้เลยสักบรรทัด — รู้จักแค่ GameState เท่านั้น

static void drawHpBar(int cur, int max, int x, int y, int width, int height) {
    if (max < 1)
        max = 1;
    int filled = (cur * width) / max;
    DrawRectangle(x, y, width, height, DARKGRAY);
    DrawRectangle(x, y, filled, height, RED);
    DrawRectangleLines(x, y, width, height, WHITE);
}

static void drawMap(const GameState *g) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            Color tileColor = (tilemap[r][c] == 1) ? DARKGRAY
                             : (tilemap[r][c] == 2) ? GOLD
                             : (Color){ 35, 35, 35, 255 };
            DrawRectangle(c * TILE, r * TILE, TILE - 2, TILE - 2, tileColor);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (g->enemies[i].active)
            DrawRectangle(g->enemies[i].col * TILE + 6, g->enemies[i].row * TILE + 6,
                          TILE - 14, TILE - 14, MAROON);
    }

    DrawRectangle(g->player.col * TILE + 6, g->player.row * TILE + 6,
                  TILE - 14, TILE - 14, BLUE);
}

void renderGame(const GameState *g) {
    BeginDrawing();
        ClearBackground(BLACK);   // เทียบเท่า system("cls") ในเวอร์ชัน terminal — แต่ต้องเรียกทุก frame
        drawMap(g);

        int hudY = ROWS * TILE + 10;
        DrawText(TextFormat("%s  Lv.%d  Floor:%d", g->player.name, g->player.level, g->floor),
                  10, hudY, 20, WHITE);
        DrawText("HP:", 10, hudY + 26, 20, WHITE);
        drawHpBar(g->player.hp, g->player.max_hp, 55, hudY + 26, 200, 20);
        DrawText(g->message, 10, hudY + 54, 18, LIGHTGRAY);
        DrawText("[WASD/Arrows] Move   [V] Save   [Q] Quit (ESC ปิดหน้าต่างได้เช่นกัน)",
                  10, hudY + 80, 16, GRAY);
    EndDrawing();
}

void renderGameOver(const GameState *g) {
    BeginDrawing();
        ClearBackground(BLACK);
        DrawText("GAME OVER", SCREEN_W / 2 - 110, SCREEN_H / 2 - 40, 30, RED);
        DrawText(TextFormat("Player: %s   Floor reached: %d", g->player.name, g->floor),
                  SCREEN_W / 2 - 180, SCREEN_H / 2 + 10, 18, WHITE);
        DrawText("ปิดหน้าต่างเพื่อออกจากเกม", SCREEN_W / 2 - 110, SCREEN_H / 2 + 40, 16, GRAY);
    EndDrawing();
}

// ── input ───────────────────────────────────────────────────
// เปลี่ยนจาก getch() (conio.h) เป็น IsKeyPressed() ของ Raylib
// ยังคงคืนค่าเป็น char ตัวเดียวเหมือนเดิม — updateGame() ด้านบนไม่ต้องแก้อะไรเลย

void getPlayerName(char *name) {
    printf("Enter your name: ");
    scanf("%31s", name);
}

// IsKeyPressed() คืน true แค่เฟรมเดียวตอนกดคีย์ลง — เหมือน getch() ตรงที่ "1 กด = 1 turn"
char getPlayerInput(void) {
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))    return 'w';
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))  return 's';
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))  return 'a';
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) return 'd';
    if (IsKeyPressed(KEY_V))                            return 'v';
    if (IsKeyPressed(KEY_Q))                            return 'q';
    return 0; // ไม่มีคีย์ที่สนใจถูกกดในเฟรมนี้ — main จะไม่เรียก updateGame()
    // หมายเหตุ: ESC ปิดหน้าต่างเองอัตโนมัติอยู่แล้ว (WindowShouldClose() คืน true)
    // เป็นพฤติกรรม default ของ Raylib ไม่ต้องดักจับเพิ่ม
}

// ── save / load (File I/O — เหมือน Week 12 ทุกบรรทัด ไม่เกี่ยวกับ Raylib) ──

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
// ต่างจาก Week 12 ตรงที่ต้องวาดทุก frame (60 FPS) แม้ไม่มี input ใหม่
// เพราะ Raylib เป็น real-time loop ไม่ใช่ turn-based loop ที่รอ input ค้าง

int main(void) {
    GameState game;
    char name[MAX_NAME];

    printf("=== Dungeon Explorer (Raylib) ===\n");
    getPlayerName(name);
    initGame(&game, name);

    InitWindow(SCREEN_W, SCREEN_H, "Dungeon Explorer - Raylib");
    SetTargetFPS(60);

    while (!WindowShouldClose() && game.running) {
        char cmd = getPlayerInput();        // INPUT (1 คีย์ = 1 turn เหมือน Week 12)
        if (cmd != 0) {
            updateGame(&game, cmd);          // UPDATE
            if (cmd == 'v' || cmd == 'V')
                saveGame(&game, SAVE_FILE);  // save เฉพาะตอนผู้เล่นสั่ง (กด V)
        }
        renderGame(&game);                   // DRAW — วาดทุก frame แม้ไม่มี input
    }

    while (game.running == 0 && !WindowShouldClose())
        renderGameOver(&game);

    CloseWindow();
    return 0;
}
