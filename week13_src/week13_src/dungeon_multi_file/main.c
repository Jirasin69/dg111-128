// main.c — game loop แบบ real-time: input -> update -> save เมื่อสั่ง (กด V) -> draw ทุกเฟรม
// ต่างจาก Week 12 ตรงที่ต้องวาดทุก frame (60 FPS) แม้ไม่มี input ใหม่
// เพราะ Raylib เป็น real-time loop ไม่ใช่ turn-based loop ที่รอ input ค้าง
#include <stdio.h>
#include "raylib.h"
#include "game.h"
#include "render.h"

int main(void) {
    GameState game;
    char name[MAX_NAME];

    printf("=== Dungeon Explorer (Raylib) ===\n");
    printf("Enter your name: ");
    scanf("%31s", name);
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
