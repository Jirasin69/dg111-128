// render.c — แยก "วาดจอ" ออกจาก logic อย่างชัดเจน (game.c ไม่รู้จัก printf เลย)
// Week 13 upgrade: เปลี่ยนแค่ไฟล์นี้เป็น Raylib — game.c ไม่ต้องแตะ
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // kbhit()/getch() — เฉพาะ Windows, ใช้แบบเดียวกับ src/04-sample-console-games/Tetris/main.c
#include "render.h"

static void printBar(int cur, int max, int width) {
    if (max < 1)
        max = 1;
    int filled = (cur * width) / max;
    printf("[");
    for (int i = 0; i < width; i++)
        printf("%c", i < filled ? '#' : ' ');
    printf("] %d/%d", cur, max);
}

// วาด tilemap พร้อมผู้เล่น ('@') และ enemy ที่ active ('E') ทับด้านบน
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
