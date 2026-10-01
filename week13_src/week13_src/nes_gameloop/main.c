/*
 * main.c — NES-style FSM Game Loop ด้วย Raylib
 *
 * dotnes เทียบ:
 *   ppu_wait_nmi()          → SetTargetFPS(60) + delta time
 *   GameState enum          → typedef enum { TITLE, PLAYING, GAME_OVER }
 *   rand8()                 → GetRandomValue(0, 255)
 *   music_tick()            → UpdateMusicStream() ทุก frame
 *
 * States: TITLE → PLAYING → GAME_OVER → TITLE
 *
 * Compile:
 *   gcc -std=c99 main.c -o gameloop -lraylib -lm -lwinmm -lgdi32
 */

#include "raylib.h"
#include <string.h>

#define NES_W   320
#define NES_H   240
#define SCALE   2.5f
#define WIN_W   ((int)(NES_W * SCALE))
#define WIN_H   ((int)(NES_H * SCALE))
#define SPR     8   /* tile/sprite ขนาด 8×8 */

/* NES Palette — 4 สีที่เลือกใช้ */
#define C_BG      CLITERAL(Color){ 0,   30,  116, 255 }  /* 0x01 dark blue */
#define C_WHITE   CLITERAL(Color){ 236, 238, 236, 255 }  /* 0x30 */
#define C_RED     CLITERAL(Color){ 152, 34,  32,  255 }  /* 0x16 */
#define C_GREEN   CLITERAL(Color){ 8,   124, 0,   255 }  /* 0x1A */
#define C_YELLOW  CLITERAL(Color){ 212, 136, 32,  255 }  /* 0x27 */
#define C_GRAY    CLITERAL(Color){ 84,  84,  84,  255 }  /* 0x00 */
#define C_BLACK   CLITERAL(Color){ 0,   0,   0,   255 }

/* --- FSM: GameState (Week 12 pattern) --- */
typedef enum {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

/* --- Entity structs --- */
typedef struct {
    float x, y;
    float vx, vy;
    int   active;
} Bullet;

typedef struct {
    float x, y;
    int   active;
    float spawnTimer;
} Enemy;

#define MAX_BULLETS  16
#define MAX_ENEMIES  8

/* --- Helper: วาดข้อความบน tile grid (เทียบ vram_write) --- */
static void TileText(const char *text, int col, int row, Color c) {
    DrawText(text, col * SPR * SCALE, row * SPR * SCALE, SPR * SCALE, c);
}

/* --- Helper: วาด sprite 8×8 (เทียบ oam_spr) --- */
static void DrawSpr(float x, float y, Color c) {
    DrawRectangle((int)(x * SCALE), (int)(y * SCALE),
                  SPR * SCALE, SPR * SCALE, c);
}

int main(void) {
    InitWindow(WIN_W, WIN_H, "dotnes → Raylib: FSM Game Loop");
    SetTargetFPS(60);   /* ppu_wait_nmi() — ล็อก 60fps */

    /* ---- Game state ---- */
    GameState state = STATE_TITLE;

    float playerX = NES_W / 2.0f - SPR / 2.0f;
    float playerY = NES_H - 24.0f;
    float playerSpeed = 80.0f;

    Bullet bullets[MAX_BULLETS] = {0};
    Enemy  enemies[MAX_ENEMIES] = {0};

    int   score      = 0;
    int   lives      = 3;
    float enemyTimer = 0.0f;
    float shootTimer = 0.0f;
    int   hiScore    = 0;
    int   blinkFrame = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        blinkFrame++;

        /* ================================================================
         * STATE UPDATE — เทียบกับ switch(gameState) ใน NES game
         * ================================================================ */
        switch (state) {

        case STATE_TITLE:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                /* reset game */
                playerX = NES_W / 2.0f - SPR / 2.0f;
                playerY = NES_H - 24.0f;
                score = 0;
                lives = 3;
                enemyTimer = 0.0f;
                memset(bullets, 0, sizeof(bullets));
                memset(enemies, 0, sizeof(enemies));
                state = STATE_PLAYING;
            }
            break;

        case STATE_PLAYING:
            /* pad_poll: เคลื่อนที่ผู้เล่น */
            if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A))
                playerX -= playerSpeed * dt;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
                playerX += playerSpeed * dt;

            if (playerX < 8)           playerX = 8;
            if (playerX > NES_W - 16)  playerX = NES_W - 16;

            /* ยิง bullet — auto fire ทุก 0.3s หรือกด SPACE */
            shootTimer += dt;
            if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_Z)) && shootTimer > 0.25f) {
                shootTimer = 0.0f;
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].x = playerX + 2;
                        bullets[i].y = playerY - 4;
                        bullets[i].vy = -160.0f;
                        bullets[i].active = 1;
                        break;
                    }
                }
            }

            /* อัปเดต bullet */
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                bullets[i].y += bullets[i].vy * dt;
                if (bullets[i].y < -8) bullets[i].active = 0;
            }

            /* spawn enemy — rand8() → GetRandomValue */
            enemyTimer += dt;
            if (enemyTimer > 1.0f) {
                enemyTimer = 0.0f;
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) {
                        enemies[i].x = (float)GetRandomValue(8, NES_W - 16);
                        enemies[i].y = 8.0f;
                        enemies[i].active = 1;
                        break;
                    }
                }
            }

            /* อัปเดต enemy ลงมา */
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].active) continue;
                enemies[i].y += 30.0f * dt;

                /* enemy ออกนอกจอ = เสียชีวิต 1 */
                if (enemies[i].y > NES_H) {
                    enemies[i].active = 0;
                    lives--;
                }

                /* ตรวจ bullet hit */
                for (int j = 0; j < MAX_BULLETS; j++) {
                    if (!bullets[j].active) continue;
                    float dx = bullets[j].x - enemies[i].x;
                    float dy = bullets[j].y - enemies[i].y;
                    if (dx > -SPR && dx < SPR && dy > -SPR && dy < SPR) {
                        enemies[i].active = 0;
                        bullets[j].active = 0;
                        score += 10;
                    }
                }

                /* ตรวจ player hit */
                float dx = playerX - enemies[i].x;
                float dy = playerY - enemies[i].y;
                if (dx > -SPR && dx < SPR && dy > -SPR && dy < SPR) {
                    enemies[i].active = 0;
                    lives--;
                }
            }

            if (lives <= 0) {
                if (score > hiScore) hiScore = score;
                state = STATE_GAME_OVER;
            }
            break;

        case STATE_GAME_OVER:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                state = STATE_TITLE;
            }
            break;
        }

        /* ================================================================
         * DRAW — BeginDrawing / EndDrawing (NES: คือหลัง ppu_wait_nmi)
         * ================================================================ */
        BeginDrawing();
            ClearBackground(C_BG);

            switch (state) {

            case STATE_TITLE:
                TileText("* NES SHOOTER *", 9, 5, C_YELLOW);
                TileText("RAYLIB EDITION",  10, 7, C_WHITE);
                /* กระพริบ PRESS START ทุก 30 frame */
                if ((blinkFrame / 30) % 2 == 0)
                    TileText("PRESS SPACE/ENTER", 7, 14, C_WHITE);
                TileText("HI-SCORE:", 11, 20, C_GRAY);
                DrawText(TextFormat("%06d", hiScore),
                         21 * SPR * SCALE, 20 * SPR * SCALE, SPR * SCALE, C_YELLOW);
                TileText("ARROWS: MOVE", 10, 24, C_GRAY);
                TileText("SPACE/Z: SHOOT", 9, 25, C_GRAY);
                break;

            case STATE_PLAYING:
                /* วาด enemy (oam_spr tile สีแดง) */
                for (int i = 0; i < MAX_ENEMIES; i++)
                    if (enemies[i].active)
                        DrawSpr(enemies[i].x, enemies[i].y, C_RED);

                /* วาด bullet */
                for (int i = 0; i < MAX_BULLETS; i++)
                    if (bullets[i].active)
                        DrawRectangle((int)(bullets[i].x * SCALE),
                                      (int)(bullets[i].y * SCALE),
                                      4, SPR * SCALE, C_YELLOW);

                /* วาด player */
                DrawSpr(playerX, playerY, C_GREEN);

                /* HUD */
                TileText("SC:", 1, 1, C_WHITE);
                DrawText(TextFormat("%06d", score),
                         4 * SPR * SCALE, 1 * SPR * SCALE, SPR * SCALE, C_YELLOW);
                TileText("LV:", 28, 1, C_WHITE);
                for (int i = 0; i < lives; i++)
                    DrawSpr(31 * (float)SPR + i * 10, SPR, C_GREEN);
                break;

            case STATE_GAME_OVER:
                TileText("GAME OVER", 12, 10, C_RED);
                TileText("SCORE:", 11, 13, C_WHITE);
                DrawText(TextFormat("%06d", score),
                         18 * SPR * SCALE, 13 * SPR * SCALE, SPR * SCALE, C_YELLOW);
                if (score >= hiScore && score > 0)
                    TileText("** NEW HI-SCORE! **", 7, 15, C_YELLOW);
                if ((blinkFrame / 30) % 2 == 0)
                    TileText("PRESS SPACE/ENTER", 4, 20, C_WHITE);
                break;
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
