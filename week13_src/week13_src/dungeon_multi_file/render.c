// render.c — Week 13 upgrade: เปลี่ยน printf/system("cls")/getch() เป็น Raylib
// ไฟล์นี้ไฟล์เดียวที่เปลี่ยน (+ main.c) — game.c และ save.c เหมือน Week 12 ทุกบรรทัด
#include "raylib.h"
#include "render.h"

// เดิมคือ printBar() แบบ [####    ] — ตอนนี้วาดเป็นสี่เหลี่ยมแทน
static void drawHpBar(int cur, int max, int x, int y, int width, int height) {
    if (max < 1)
        max = 1;
    int filled = (cur * width) / max;
    DrawRectangle(x, y, width, height, DARKGRAY);
    DrawRectangle(x, y, filled, height, RED);
    DrawRectangleLines(x, y, width, height, WHITE);
}

// วาด tilemap เป็นสี่เหลี่ยมสี แล้ววาด enemy (แดง) และผู้เล่น (น้ำเงิน) ทับด้านบน
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
        DrawText("[WASD/Arrows] Move   [V] Save   [Q] Quit   [ESC] Close",
                  10, hudY + 80, 16, GRAY);
    EndDrawing();
}

void renderGameOver(const GameState *g) {
    BeginDrawing();
        ClearBackground(BLACK);
        DrawText("GAME OVER", SCREEN_W / 2 - 110, SCREEN_H / 2 - 40, 30, RED);
        DrawText(TextFormat("Player: %s   Floor reached: %d", g->player.name, g->floor),
                  SCREEN_W / 2 - 180, SCREEN_H / 2 + 10, 18, WHITE);
        DrawText("Close the window to exit", SCREEN_W / 2 - 110, SCREEN_H / 2 + 40, 16, GRAY);
    EndDrawing();
}

// เปลี่ยนจาก getch() (conio.h) เป็น IsKeyPressed() ของ Raylib
// ยังคงคืนค่าเป็น char ตัวเดียวเหมือนเดิม — updateGame() ใน game.c ไม่ต้องแก้อะไรเลย
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
}
