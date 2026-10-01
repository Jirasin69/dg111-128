/*
 * main.c — Maze Escape (Raylib C99 Edition)
 *
 * ดัดแปลงจาก: marsdevx/maze-escape (https://github.com/marsdevx/maze-escape)
 * พอร์ตจาก C + MiniLibX สู่ Raylib 5.x C99 สำหรับชุดการเรียนรู้เกมสไตล์ NES ในวิชา DG111
 *
 * คุณสมบัติ:
 *  - หน้าจอมาตรฐาน 800 × 600 px (HUD ด้านบน 70 px + พื้นที่เขาวงกตตรงกลาง 495 px + แถบช่วยเหลือ 35 px)
 *  - ระบบคำนวณ Tile Size และจัดกึ่งกลางหน้าจออัตโนมัติตามขนาดของแต่ละด่าน (11×11 ถึง 29×15)
 *  - 4 ด่านปริศนาเขาวงกต (levels/lvl1 ถึง lvl4 พร้อมระบบ Built-in Fallback)
 *  - 6 ธีมกราฟิก: Pacman, Adventurer, Chicken, Pokemon, Space-Ship, Time-Adventure
 *  - สลับธีมแบบเรียลไทม์ด้วยปุ่ม [T] หรือคลิกเลือก
 *  - สลับด่านได้ด้วยปุ่ม [1-4] หรือเล่นตามลำดับ 1 -> 2 -> 3 -> 4
 *  - ตัวละครเคลื่อนที่แบบ Smooth Interpolation และกลับด้านซ้าย-ขวาตามทิศทางการเดิน
 *  - ระบบอนุภาค (Particles) เมื่อเก็บไอเทม และพลุเฉลิมฉลองเมื่อผ่านด่าน
 */

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define WIN_W 800
#define WIN_H 600

#define MAX_MAP_W 35
#define MAX_MAP_H 35
#define MAX_LEVELS 4
#define MAX_PARTICLES 128

/* 6 ธีมกราฟิก */
typedef enum ThemeType
{
    THEME_PACMAN = 0,
    THEME_ADVENTURER,
    THEME_CHICKEN,
    THEME_POKEMON,
    THEME_SPACESHIP,
    THEME_TIME_ADVENTURE,
    THEME_COUNT
} ThemeType;

static const char *THEME_NAMES[THEME_COUNT] = {
    "PACMAN",
    "ADVENTURER",
    "CHICKEN",
    "POKEMON",
    "SPACE-SHIP",
    "TIME-ADVENTURE"};

static const char *THEME_DIRS[THEME_COUNT] = {
    "pacman",
    "adventurer",
    "chicken",
    "pokemon",
    "space-ship",
    "time-adventure"};

/* พื้นผิวสำหรับ 1 ธีม */
typedef struct ThemeTextures
{
    Texture2D bg;
    Texture2D wall;
    Texture2D item;
    Texture2D exit;
    Texture2D player;
    bool loaded;
} ThemeTextures;

/* แผนที่เขาวงกต */
typedef struct MazeMap
{
    char grid[MAX_MAP_H][MAX_MAP_W];
    int width;
    int height;
    int startX;
    int startY;
    int exitX;
    int exitY;
    int totalCollectibles;
} MazeMap;

/* อนุภาคเอฟเฟกต์ */
typedef struct Particle
{
    Vector2 pos;
    Vector2 vel;
    Color color;
    float size;
    float life;
    float maxLife;
    bool active;
} Particle;

/* สถานะเกม */
typedef enum GameState
{
    STATE_TITLE = 0,
    STATE_PLAYING,
    STATE_LEVEL_CLEAR,
    STATE_GAME_COMPLETE
} GameState;

typedef struct Game
{
    ThemeTextures themes[THEME_COUNT];
    ThemeType currentTheme;
    GameState state;

    int currentLevel; /* 1 ถึง 4 */
    MazeMap originalMap;
    MazeMap map;

    int playerX;
    int playerY;
    float visualX;
    float visualY;
    bool facingLeft;

    int moveCount;
    int collectedCount;
    float messageTimer;
    char message[64];

    /* ข้อมูลการแสดงผลตาราง */
    int tileSize;
    int boardStartX;
    int boardStartY;

    Particle particles[MAX_PARTICLES];
    float animTime;
} Game;

/*******************************************************************************
 * Built-in Default Levels (Fallback หากไม่พบไฟล์ levels/)
 *******************************************************************************/

static const char *DEFAULT_LVL1 =
    "11111111111\n"
    "10000P1C001\n"
    "11111011101\n"
    "10001000001\n"
    "10101111101\n"
    "1C100000001\n"
    "11101110101\n"
    "10001C00101\n"
    "10111111101\n"
    "10000E10001\n"
    "11111111111\n";

static const char *DEFAULT_LVL2 =
    "111111111111111\n"
    "1000001P0000001\n"
    "101110101011101\n"
    "10001C001010001\n"
    "111011101010111\n"
    "10000C10101C001\n"
    "101110101111101\n"
    "101000101000101\n"
    "101010111010101\n"
    "101010100C10101\n"
    "101011101110101\n"
    "101000001000101\n"
    "101111111011101\n"
    "1C00001E001C001\n"
    "111111111111111\n";

static const char *DEFAULT_LVL3 =
    "1111111111111111111\n"
    "10000C101P000010001\n"
    "1011101010111010101\n"
    "101000101000101C101\n"
    "1010111011101011101\n"
    "101000001C001000001\n"
    "1011111011111110101\n"
    "10001000100000001C1\n"
    "111C101110111111111\n"
    "1000100C00100000001\n"
    "1011111111101111101\n"
    "10000000100C1000001\n"
    "1110111010111111111\n"
    "1000100010001000001\n"
    "1011101110101011101\n"
    "1010000000100000101\n"
    "101C11101E1111101C1\n"
    "1111111111111111111\n";

static const char *DEFAULT_LVL4 =
    "11111111111111111111111111111\n"
    "10100001C0000000010C001000011\n"
    "10000101101110010100001110111\n"
    "1011110100001001C1C00C0011101\n"
    "10000101000011000111011000101\n"
    "10100001011111111100010010001\n"
    "10111001000000000100010110001\n"
    "1010CC01111111110110110100111\n"
    "10101000000000010010100100101\n"
    "101111110001000100101011E0101\n"
    "1000000111110101000010C111111\n"
    "1C0010010C01C101010111111C001\n"
    "1111100110P101011100C00000011\n"
    "100C0000000001000001100010011\n"
    "11111111111111111111111111111\n";

/*******************************************************************************
 * ตัวจัดการโหลดข้อมูล Assets & Map
 *******************************************************************************/

static void LoadTheme(Game *game, ThemeType t)
{
    if (game->themes[t].loaded)
        return;

    char path[256];
    const char *dir = THEME_DIRS[t];

    snprintf(path, sizeof(path), "textures/%s/bg.png", dir);
    game->themes[t].bg = LoadTexture(path);

    snprintf(path, sizeof(path), "textures/%s/wall.png", dir);
    game->themes[t].wall = LoadTexture(path);

    snprintf(path, sizeof(path), "textures/%s/item.png", dir);
    game->themes[t].item = LoadTexture(path);

    snprintf(path, sizeof(path), "textures/%s/exit.png", dir);
    game->themes[t].exit = LoadTexture(path);

    snprintf(path, sizeof(path), "textures/%s/player.png", dir);
    game->themes[t].player = LoadTexture(path);

    game->themes[t].loaded = true;
}

static void UnloadAllThemes(Game *game)
{
    for (int i = 0; i < THEME_COUNT; i++)
    {
        if (game->themes[i].loaded)
        {
            UnloadTexture(game->themes[i].bg);
            UnloadTexture(game->themes[i].wall);
            UnloadTexture(game->themes[i].item);
            UnloadTexture(game->themes[i].exit);
            UnloadTexture(game->themes[i].player);
            game->themes[i].loaded = false;
        }
    }
}

/* อ่านแผนที่จากไฟล์ หรือใช้ String สำรอง */
static bool LoadMapData(int levelNum, MazeMap *outMap)
{
    memset(outMap, 0, sizeof(*outMap));

    char filename[64];
    snprintf(filename, sizeof(filename), "levels/lvl%d", levelNum);

    char *fileText = LoadFileText(filename);
    const char *src = fileText;
    if (!src)
    {
        /* ใช้ Built-in Default Map */
        if (levelNum == 1)
            src = DEFAULT_LVL1;
        else if (levelNum == 2)
            src = DEFAULT_LVL2;
        else if (levelNum == 3)
            src = DEFAULT_LVL3;
        else if (levelNum == 4)
            src = DEFAULT_LVL4;
        else
            src = DEFAULT_LVL1;
    }

    int r = 0, c = 0;
    int maxCols = 0;
    for (const char *p = src; *p != '\0'; p++)
    {
        if (*p == '\r')
            continue;
        if (*p == '\n')
        {
            if (c > maxCols)
                maxCols = c;
            r++;
            c = 0;
            if (r >= MAX_MAP_H)
                break;
            continue;
        }

        if (c < MAX_MAP_W && r < MAX_MAP_H)
        {
            char tile = *p;
            outMap->grid[r][c] = tile;
            if (tile == 'P')
            {
                outMap->startX = c;
                outMap->startY = r;
            }
            else if (tile == 'E')
            {
                outMap->exitX = c;
                outMap->exitY = r;
            }
            else if (tile == 'C')
            {
                outMap->totalCollectibles++;
            }
            c++;
        }
    }
    if (c > 0)
    {
        if (c > maxCols)
            maxCols = c;
        r++;
    }

    outMap->width = maxCols;
    outMap->height = r;

    if (fileText)
        UnloadFileText(fileText);
    return (outMap->width > 0 && outMap->height > 0);
}

/*******************************************************************************
 * ระบบอนุภาค (Particles)
 *******************************************************************************/

static void SpawnParticles(Game *game, Vector2 center, Color col, int count, float speed)
{
    for (int i = 0; i < count; i++)
    {
        for (int p = 0; p < MAX_PARTICLES; p++)
        {
            if (!game->particles[p].active)
            {
                game->particles[p].active = true;
                game->particles[p].pos = center;
                float angle = (float)GetRandomValue(0, 360) * (PI / 180.0f);
                float spd = (float)GetRandomValue(20, (int)(speed * 100)) / 100.0f;
                game->particles[p].vel = (Vector2){cosf(angle) * spd, sinf(angle) * spd};
                game->particles[p].color = col;
                game->particles[p].size = (float)GetRandomValue(3, 7);
                game->particles[p].life = 0.0f;
                game->particles[p].maxLife = (float)GetRandomValue(30, 80) / 100.0f;
                break;
            }
        }
    }
}

static void UpdateParticles(Game *game, float dt)
{
    for (int p = 0; p < MAX_PARTICLES; p++)
    {
        if (!game->particles[p].active)
            continue;
        game->particles[p].life += dt;
        if (game->particles[p].life >= game->particles[p].maxLife)
        {
            game->particles[p].active = false;
            continue;
        }
        game->particles[p].pos.x += game->particles[p].vel.x * dt;
        game->particles[p].pos.y += game->particles[p].vel.y * dt;
        game->particles[p].vel.y += 60.0f * dt; /* แรงโน้มถ่วงเล็กน้อย */
    }
}

static void DrawParticles(Game *game)
{
    for (int p = 0; p < MAX_PARTICLES; p++)
    {
        if (!game->particles[p].active)
            continue;
        float alpha = 1.0f - (game->particles[p].life / game->particles[p].maxLife);
        Color c = Fade(game->particles[p].color, alpha);
        DrawCircleV(game->particles[p].pos, game->particles[p].size * alpha, c);
    }
}

/*******************************************************************************
 * คำนวณขนาดและตำแหน่งหน้ากระดาน (Board Geometry)
 *******************************************************************************/

static void RecalculateBoardGeometry(Game *game)
{
    const int availableW = WIN_W - 40;
    const int availableH = 490; /* 565 - 75 */

    int tileW = availableW / game->map.width;
    int tileH = availableH / game->map.height;

    game->tileSize = (tileW < tileH) ? tileW : tileH;
    if (game->tileSize > 48)
        game->tileSize = 48;
    if (game->tileSize < 16)
        game->tileSize = 16;

    int boardW = game->tileSize * game->map.width;
    int boardH = game->tileSize * game->map.height;

    game->boardStartX = (WIN_W - boardW) / 2;
    game->boardStartY = 75 + (availableH - boardH) / 2;
}

/*******************************************************************************
 * จัดการเริ่มเกมและเปลี่ยนด่าน
 *******************************************************************************/

static void StartLevel(Game *game, int levelNum)
{
    game->currentLevel = levelNum;
    LoadMapData(levelNum, &game->originalMap);
    memcpy(&game->map, &game->originalMap, sizeof(MazeMap));

    game->playerX = game->map.startX;
    game->playerY = game->map.startY;
    game->visualX = (float)game->playerX;
    game->visualY = (float)game->playerY;
    game->facingLeft = false;

    game->moveCount = 0;
    game->collectedCount = 0;
    game->messageTimer = 0.0f;
    game->message[0] = '\0';
    game->state = STATE_PLAYING;

    RecalculateBoardGeometry(game);
}

/*******************************************************************************
 * การเคลื่อนที่และกฎของเกม (Gameplay & Rules)
 *******************************************************************************/

static void ShowMessage(Game *game, const char *msg, float duration)
{
    strncpy(game->message, msg, sizeof(game->message) - 1);
    game->messageTimer = duration;
}

static void TryMovePlayer(Game *game, int dx, int dy)
{
    if (game->state != STATE_PLAYING)
        return;

    int targetX = game->playerX + dx;
    int targetY = game->playerY + dy;

    if (dx < 0)
        game->facingLeft = true;
    else if (dx > 0)
        game->facingLeft = false;

    /* ชนขอบเขต */
    if (targetX < 0 || targetX >= game->map.width || targetY < 0 || targetY >= game->map.height)
    {
        return;
    }

    char targetTile = game->map.grid[targetY][targetX];

    /* ชนกำแพง */
    if (targetTile == '1')
    {
        return;
    }

    /* เดินได้ */
    game->playerX = targetX;
    game->playerY = targetY;
    game->moveCount++;

    /* เก็บไอเทม */
    if (targetTile == 'C')
    {
        game->map.grid[targetY][targetX] = '0';
        game->collectedCount++;

        /* เอฟเฟกต์ประกายแสงเมื่อเก็บ */
        Vector2 center = {
            game->boardStartX + (targetX + 0.5f) * game->tileSize,
            game->boardStartY + (targetY + 0.5f) * game->tileSize};
        SpawnParticles(game, center, GOLD, 18, 140.0f);

        if (game->collectedCount == game->map.totalCollectibles)
        {
            ShowMessage(game, "ALL ITEMS COLLECTED! EXIT IS NOW OPEN!", 3.0f);
        }
    }

    /* เข้าประตูทางออก */
    else if (targetTile == 'E')
    {
        if (game->collectedCount >= game->map.totalCollectibles)
        {
            /* ผ่านด่าน! */
            Vector2 exitCenter = {
                game->boardStartX + (targetX + 0.5f) * game->tileSize,
                game->boardStartY + (targetY + 0.5f) * game->tileSize};
            SpawnParticles(game, exitCenter, GREEN, 40, 220.0f);

            if (game->currentLevel < MAX_LEVELS)
            {
                game->state = STATE_LEVEL_CLEAR;
            }
            else
            {
                game->state = STATE_GAME_COMPLETE;
            }
        }
        else
        {
            int left = game->map.totalCollectibles - game->collectedCount;
            char buf[64];
            snprintf(buf, sizeof(buf), "EXIT LOCKED! COLLECT %d MORE ITEM%s!", left, (left > 1) ? "S" : "");
            ShowMessage(game, buf, 2.0f);
        }
    }
}

/*******************************************************************************
 * วาดหน้าจอ (Rendering)
 *******************************************************************************/

static void DrawGame(Game *game)
{
    ThemeTextures *tex = &game->themes[game->currentTheme];
    float t = game->animTime;

    /* 1. วาดแถบ HUD ด้านบน (0 to 75) */
    DrawRectangle(0, 0, WIN_W, 72, (Color){20, 24, 34, 255});
    DrawLine(0, 72, WIN_W, 72, (Color){55, 65, 85, 255});

    DrawText("MAZE ESCAPE", 20, 16, 24, GOLD);
    DrawText("[NES RETRO EDITION]", 20, 44, 12, LIGHTGRAY);

    /* ข้อมูลด่านและธีม */
    DrawText(TextFormat("LEVEL: %d/%d", game->currentLevel, MAX_LEVELS), 230, 18, 16, RAYWHITE);
    DrawText(TextFormat("THEME: %s", THEME_NAMES[game->currentTheme]), 230, 42, 14, SKYBLUE);

    /* จำนวนก้าวและไอเทม */
    DrawText(TextFormat("MOVES: %d", game->moveCount), 420, 18, 16, RAYWHITE);

    int itemsLeft = game->map.totalCollectibles - game->collectedCount;
    if (itemsLeft == 0)
    {
        Color exitCol = ((int)(t * 4.0f) % 2 == 0) ? GREEN : LIME;
        DrawText("EXIT OPEN!", 420, 42, 16, exitCol);
    }
    else
    {
        DrawText(TextFormat("ITEMS: %d / %d", game->collectedCount, game->map.totalCollectibles), 420, 42, 14, YELLOW);
    }

    /* ปุ่มลัดย่อ */
    DrawText("[T] THEME", 680, 16, 12, LIGHTGRAY);
    DrawText("[R] RESTART", 680, 32, 12, LIGHTGRAY);
    DrawText("[1-4] SELECT", 680, 48, 12, LIGHTGRAY);

    /* 2. วาดกระดานเขาวงกต */
    int ts = game->tileSize;
    int sx = game->boardStartX;
    int sy = game->boardStartY;

    /* กรอบพื้นหลังกระดาน */
    DrawRectangle(sx - 4, sy - 4, game->map.width * ts + 8, game->map.height * ts + 8, (Color){10, 12, 18, 255});

    for (int r = 0; r < game->map.height; r++)
    {
        for (int c = 0; c < game->map.width; c++)
        {
            char tile = game->map.grid[r][c];
            Rectangle dest = {(float)(sx + c * ts), (float)(sy + r * ts), (float)ts, (float)ts};

            /* วาดพื้นหลังช่องเสมอ */
            Rectangle srcBg = {0, 0, (float)tex->bg.width, (float)tex->bg.height};
            DrawTexturePro(tex->bg, srcBg, dest, (Vector2){0, 0}, 0.0f, WHITE);

            if (tile == '1')
            {
                Rectangle srcWall = {0, 0, (float)tex->wall.width, (float)tex->wall.height};
                DrawTexturePro(tex->wall, srcWall, dest, (Vector2){0, 0}, 0.0f, WHITE);
            }
            else if (tile == 'C')
            {
                /* แอนิเมชันลอยดึ๋งๆ ของไอเทม */
                float bob = sinf(t * 5.0f + (c + r)) * 2.0f;
                Rectangle destItem = {dest.x, dest.y + bob, dest.width, dest.height};
                Rectangle srcItem = {0, 0, (float)tex->item.width, (float)tex->item.height};
                DrawTexturePro(tex->item, srcItem, destItem, (Vector2){0, 0}, 0.0f, WHITE);
            }
            else if (tile == 'E')
            {
                Rectangle srcExit = {0, 0, (float)tex->exit.width, (float)tex->exit.height};
                Color exitTint = (game->collectedCount >= game->map.totalCollectibles) ? WHITE : Fade(GRAY, 0.7f);
                DrawTexturePro(tex->exit, srcExit, dest, (Vector2){0, 0}, 0.0f, exitTint);

                /* หากประตูเปิด ให้วาดออร่าสีเขียวเรืองแสง */
                if (game->collectedCount >= game->map.totalCollectibles)
                {
                    DrawRectangleLinesEx(dest, 2.0f, Fade(GREEN, 0.6f + 0.4f * sinf(t * 6.0f)));
                }
            }
        }
    }

    /* 3. วาดตัวละครผู้เล่น (Smooth Visual Lerp) */
    Rectangle destPlayer = {
        sx + game->visualX * ts,
        sy + game->visualY * ts,
        (float)ts,
        (float)ts};
    Rectangle srcPlayer = {0, 0, (float)tex->player.width, (float)tex->player.height};
    if (game->facingLeft)
    {
        srcPlayer.width = -srcPlayer.width; /* กลับซ้าย-ขวา */
    }
    DrawTexturePro(tex->player, srcPlayer, destPlayer, (Vector2){0, 0}, 0.0f, WHITE);

    /* 4. วาดระบบอนุภาค */
    DrawParticles(game);

    /* 5. กล่องข้อความแจ้งเตือนกลางจอ (ถ้ามี) */
    if (game->messageTimer > 0.0f)
    {
        int msgW = MeasureText(game->message, 16);
        int boxW = msgW + 30;
        int boxH = 36;
        int boxX = (WIN_W - boxW) / 2;
        int boxY = sy + (game->map.height * ts) / 2 - boxH / 2;

        DrawRectangle(boxX, boxY, boxW, boxH, (Color){0, 0, 0, 220});
        DrawRectangleLines(boxX, boxY, boxW, boxH, GOLD);
        DrawText(game->message, boxX + 15, boxY + 10, 16, GOLD);
    }

    /* 6. แถบช่วยเหลือด้านล่าง */
    DrawRectangle(0, WIN_H - 35, WIN_W, 35, (Color){16, 20, 28, 255});
    DrawLine(0, WIN_H - 35, WIN_W, WIN_H - 35, (Color){45, 55, 75, 255});
    DrawText("CONTROLS: [WASD / ARROWS] MOVE  |  [T] THEME  |  [R] RESTART  |  [1-4] LEVEL",
             20, WIN_H - 24, 12, LIGHTGRAY);
    DrawText("Original: marsdevx | Raylib C99 Edition for DG111", 520, WIN_H - 24, 10, DARKGRAY);

    /* 7. Overlay เมื่อชนะด่าน */
    if (game->state == STATE_LEVEL_CLEAR)
    {
        DrawRectangle(0, 0, WIN_W, WIN_H, (Color){0, 0, 0, 180});
        DrawText("LEVEL COMPLETE!", WIN_W / 2 - MeasureText("LEVEL COMPLETE!", 36) / 2, 220, 36, GOLD);
        DrawText(TextFormat("You solved Level %d in %d moves!", game->currentLevel, game->moveCount),
                 WIN_W / 2 - MeasureText(TextFormat("You solved Level %d in %d moves!", game->currentLevel, game->moveCount), 20) / 2,
                 270, 20, RAYWHITE);

        DrawText("PRESS [SPACE / ENTER] FOR NEXT LEVEL",
                 WIN_W / 2 - MeasureText("PRESS [SPACE / ENTER] FOR NEXT LEVEL", 18) / 2,
                 330, 18, GREEN);
    }
    else if (game->state == STATE_GAME_COMPLETE)
    {
        DrawRectangle(0, 0, WIN_W, WIN_H, (Color){0, 0, 0, 200});
        DrawText("CONGRATULATIONS!", WIN_W / 2 - MeasureText("CONGRATULATIONS!", 40) / 2, 200, 40, GOLD);
        DrawText("YOU ESCAPED ALL MAZES!", WIN_W / 2 - MeasureText("YOU ESCAPED ALL MAZES!", 24) / 2, 255, 24, RAYWHITE);
        DrawText(TextFormat("Total Final Moves: %d", game->moveCount),
                 WIN_W / 2 - MeasureText(TextFormat("Total Final Moves: %d", game->moveCount), 20) / 2,
                 295, 20, YELLOW);

        DrawText("PRESS [SPACE / ENTER] TO PLAY AGAIN",
                 WIN_W / 2 - MeasureText("PRESS [SPACE / ENTER] TO PLAY AGAIN", 18) / 2,
                 355, 18, SKYBLUE);
    }
}

/*******************************************************************************
 * Main Loop
 *******************************************************************************/

int main(void)
{
    InitWindow(WIN_W, WIN_H, "Maze Escape - 800x600 (Raylib C99)");
    SetTargetFPS(60);

    Game game = {0};
    game.currentTheme = THEME_PACMAN;

    /* โหลดธีมทั้งหมดล่วงหน้า */
    for (int i = 0; i < THEME_COUNT; i++)
    {
        LoadTheme(&game, (ThemeType)i);
    }

    StartLevel(&game, 1);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (dt > 0.1f)
            dt = 0.1f;
        game.animTime += dt;

        if (game.messageTimer > 0.0f)
        {
            game.messageTimer -= dt;
        }

        /* 1. การควบคุมและการนำทาง */
        if (game.state == STATE_PLAYING)
        {
            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
                TryMovePlayer(&game, 0, -1);
            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
                TryMovePlayer(&game, 0, 1);
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
                TryMovePlayer(&game, -1, 0);
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
                TryMovePlayer(&game, 1, 0);

            if (IsKeyPressed(KEY_R))
            {
                StartLevel(&game, game.currentLevel);
                ShowMessage(&game, "LEVEL RESTARTED", 1.5f);
            }
        }
        else if (game.state == STATE_LEVEL_CLEAR)
        {
            /* ยิงพลุฉลอง */
            if (GetRandomValue(0, 10) == 0)
            {
                Vector2 firework = {(float)GetRandomValue(100, 700), (float)GetRandomValue(100, 400)};
                Color cols[] = {GOLD, RED, SKYBLUE, GREEN, PURPLE, ORANGE};
                SpawnParticles(&game, firework, cols[GetRandomValue(0, 5)], 25, 180.0f);
            }

            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
            {
                StartLevel(&game, game.currentLevel + 1);
            }
        }
        else if (game.state == STATE_GAME_COMPLETE)
        {
            if (GetRandomValue(0, 6) == 0)
            {
                Vector2 firework = {(float)GetRandomValue(100, 700), (float)GetRandomValue(80, 450)};
                Color cols[] = {GOLD, RED, SKYBLUE, GREEN, PURPLE, ORANGE, WHITE};
                SpawnParticles(&game, firework, cols[GetRandomValue(0, 6)], 30, 200.0f);
            }

            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
            {
                StartLevel(&game, 1);
            }
        }

        /* สลับธีม [T] */
        if (IsKeyPressed(KEY_T))
        {
            game.currentTheme = (ThemeType)((game.currentTheme + 1) % THEME_COUNT);
            ShowMessage(&game, TextFormat("THEME: %s", THEME_NAMES[game.currentTheme]), 1.5f);
        }

        /* เลือกระดับ [1-4] */
        if (IsKeyPressed(KEY_ONE))
            StartLevel(&game, 1);
        if (IsKeyPressed(KEY_TWO))
            StartLevel(&game, 2);
        if (IsKeyPressed(KEY_THREE))
            StartLevel(&game, 3);
        if (IsKeyPressed(KEY_FOUR))
            StartLevel(&game, 4);

        /* Smooth Lerp ตำแหน่งตัวละคร */
        game.visualX += (game.playerX - game.visualX) * 18.0f * dt;
        game.visualY += (game.playerY - game.visualY) * 18.0f * dt;

        UpdateParticles(&game, dt);

        /* 2. การวาดผล */
        BeginDrawing();
        ClearBackground((Color){14, 18, 26, 255});
        DrawGame(&game);
        EndDrawing();
    }

    UnloadAllThemes(&game);
    CloseWindow();

    return 0;
}
