// game.c — logic เท่านั้น: เดิน, ต่อสู้อัตโนมัติเมื่อชน enemy, ลงชั้นถัดไป
// ไฟล์นี้ไม่รู้จัก printf/scanf เลย — วาดจอเป็นหน้าที่ของ render.c
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

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

// ยิ่ง floor สูง enemy ยิ่งแข็งแรงขึ้น — ไม่ต้อง hardcode ทุก floor
static void generateEnemy(Enemy *e, int floor)
{
    snprintf(e->name, MAX_NAME, "Slime Lv.%d", floor);
    e->hp = 10 + floor * 4;
    e->attack = 3 + floor * 2;
    e->active = 1;
}

// เติม enemy pool ใหม่ทุกครั้งที่ขึ้นชั้น — reuse slot เดิม ไม่ malloc ใหม่
static void spawnFloorEnemies(GameState *g)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        generateEnemy(&g->enemies[i], g->floor);
        g->enemies[i].row = enemySpawnRow[i];
        g->enemies[i].col = enemySpawnCol[i];
    }
}

void initGame(GameState *g, const char *name)
{
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

// ต่อสู้อัตโนมัติ — แลกดาเมจจนฝ่ายใดฝ่ายหนึ่งตาย ไม่มีเมนูให้เลือก
static void doCombat(GameState *g, Enemy *e)
{
    while (e->hp > 0 && g->player.hp > 0)
    {
        e->hp -= g->player.attack;
        if (e->hp <= 0)
            break;
        int dmg = e->attack - g->player.defense;
        g->player.hp -= (dmg > 0 ? dmg : 1);
    }

    if (g->player.hp <= 0)
    {
        g->player.hp = 0;
        g->running = 0;
        strcpy(g->message, "You were defeated...");
        return;
    }

    e->active = 0;
    snprintf(g->message, sizeof(g->message), "Defeated %s!", e->name);
}

// enemy ที่ active ทุกตัวเดินเข้าหาผู้เล่น 1 ช่องต่อ turn
static void walkEnemies(GameState *g)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
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

// ถ้าผู้เล่นเดินไปทับ enemy ที่ active ให้เข้าสู้ทันที
static int checkCollisions(GameState *g)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        Enemy *e = &g->enemies[i];
        if (e->active && e->row == g->player.row && e->col == g->player.col)
        {
            doCombat(g, e);
            return 1;
        }
    }
    return 0;
}

static void tryMove(GameState *g, int dRow, int dCol)
{
    int nr = g->player.row + dRow;
    int nc = g->player.col + dCol;

    if (tilemap[nr][nc] == 1)
    {
        strcpy(g->message, "Blocked by a wall!");
        return;
    }

    g->player.row = nr;
    g->player.col = nc;
    strcpy(g->message, "You moved.");

    walkEnemies(g);
    int fought = checkCollisions(g);

    if (!fought && g->running && tilemap[g->player.row][g->player.col] == 2)
    {
        g->floor++;
        spawnFloorEnemies(g);
        g->player.row = 1;
        g->player.col = 1;
        snprintf(g->message, sizeof(g->message), "Descended to floor %d", g->floor);
    }
}

// อ่านคำสั่งผู้เล่น (มาจาก getPlayerInput() ใน render.c) แล้วอัปเดต state
void updateGame(GameState *g, char cmd)
{
    switch (cmd)
    {
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
