// Mini Project Template (Raylib) - DG111 Computer Programming
// C99 + Raylib 5.0
//
// HOW TO USE THIS TEMPLATE
//   1. Write your idea in PROJECT.md first (spec-first).
//   2. Compile and play the demo once, so you know what each part does.
//   3. Search for "TODO" - numbered places where you add your own ideas.
//   4. Keep the steps of the game loop. Put your code inside the right step.
//
// DEMO GAME: move the blue square and collect the yellow stars before the
//            time runs out. Stars disappear after a few seconds, so be quick!
//
// Every frame runs the same steps, in this order:
//   1. TIME   : dt = GetFrameTime()
//   2. INPUT  : read the keyboard into an Input struct
//   3. UPDATE : game logic only (no drawing here)
//   4. DRAW   : BeginDrawing() ... EndDrawing()
//   (SetTargetFPS keeps the speed: no need to Sleep)
//
// Compile: gcc -std=c99 main.c -o game -lraylib -lm -lwinmm -lgdi32
#include "raylib.h"
#include <stdio.h>
#include <string.h>

/* ---------- settings ---------- */

// TODO 1: change the size, speed and time to fit your game
#define SCREEN_W 800
#define SCREEN_H 600
#define HUD_H 40                 // top bar for score and time

#define PLAYER_SIZE 32.0f
#define PLAYER_SPEED 250.0f      // pixels per second
#define ITEM_RADIUS 10.0f

#define MAX_ITEMS 8              // object pool size (no malloc during the game)
#define GAME_TIME 30.0f          // seconds per round
#define ITEM_LIFE 4.0f           // seconds before a star disappears
#define MESSAGE_TIME 2.0f        // seconds a message stays on screen

// seconds between two new stars, for difficulty 1 (easy), 2, 3 (hard)
static const float SPAWN_TIME[3] = {1.5f, 1.0f, 0.6f};
static const char *DIFFICULTY_NAME[3] = {"EASY", "NORMAL", "HARD"};

/* ---------- data ---------- */

// TODO 2: add more states if you need them (e.g. STATE_PAUSED, STATE_HELP)
//         then add a case for it in updateGame() AND drawGame()
typedef enum
{
    STATE_TITLE,
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

typedef struct
{
    Vector2 pos;    // center of the star
    float life;     // seconds left before it disappears
    int active;     // 1 = in use, 0 = free slot in the pool
} Item;

// keyboard state for this frame
typedef struct
{
    float moveX, moveY;  // -1, 0 or 1 while a key is held down
    int start;           // Enter or Space pressed
    int digit;           // '0'..'9' if a number key was pressed, else 0
    int other;           // 1 if a key we do not use was pressed
} Input;

// TODO 3: add the data your game needs (lives, level, enemies[], ...)
typedef struct
{
    GameState state;
    Vector2 player;              // top-left corner of the player
    Item items[MAX_ITEMS];
    int difficulty;              // 0 = easy, 1 = normal, 2 = hard
    int score, hiScore;
    float timeLeft;              // seconds left in this round
    float spawnTimer;            // counts up to SPAWN_TIME[difficulty]
    float blinkTimer;            // for blinking text
    char message[64];            // short message on the title screen
    float messageTimer;          // seconds left to show the message
} Game;

/* ---------- 2. INPUT ---------- */

void readInput(Input *in)
{
    memset(in, 0, sizeof(*in));

    // held keys: smooth movement
    // TODO 4: add the keys your game needs
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        in->moveY -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        in->moveY += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        in->moveX -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        in->moveX += 1.0f;

    // pressed keys: one event per press
    int key;
    while ((key = GetKeyPressed()) != 0)
    {
        if (key == KEY_ENTER || key == KEY_SPACE)
            in->start = 1;
        else if (key >= KEY_ZERO && key <= KEY_NINE)
            in->digit = '0' + (key - KEY_ZERO);
        else if (key != KEY_W && key != KEY_A && key != KEY_S && key != KEY_D &&
                 key != KEY_UP && key != KEY_DOWN && key != KEY_LEFT && key != KEY_RIGHT)
            in->other = 1;
    }
}

/* ---------- 3. UPDATE ---------- */

void showMessage(Game *g, const char *text)
{
    strncpy(g->message, text, sizeof(g->message) - 1);
    g->message[sizeof(g->message) - 1] = '\0';
    g->messageTimer = MESSAGE_TIME;
}

void resetGame(Game *g, int difficulty)
{
    g->difficulty = difficulty;
    g->player.x = SCREEN_W / 2.0f - PLAYER_SIZE / 2.0f;
    g->player.y = (SCREEN_H + HUD_H) / 2.0f - PLAYER_SIZE / 2.0f;
    g->score = 0;
    g->timeLeft = GAME_TIME;
    g->spawnTimer = 0.0f;
    memset(g->items, 0, sizeof(g->items));
}

// take a free item from the pool and put it at a random place
void spawnItem(Game *g)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!g->items[i].active)
        {
            g->items[i].pos.x = (float)GetRandomValue(20, SCREEN_W - 20);
            g->items[i].pos.y = (float)GetRandomValue(HUD_H + 20, SCREEN_H - 20);
            g->items[i].life = ITEM_LIFE;
            g->items[i].active = 1;
            return;
        }
    }
}

// move by (dx, dy) pixels but stay inside the play area
void movePlayer(Game *g, float dx, float dy)
{
    g->player.x += dx;
    g->player.y += dy;
    if (g->player.x < 0)
        g->player.x = 0;
    if (g->player.x > SCREEN_W - PLAYER_SIZE)
        g->player.x = SCREEN_W - PLAYER_SIZE;
    if (g->player.y < HUD_H)
        g->player.y = HUD_H;
    if (g->player.y > SCREEN_H - PLAYER_SIZE)
        g->player.y = SCREEN_H - PLAYER_SIZE;
}

// collect every item touching the player. Return how many were collected
int collectItems(Game *g)
{
    Rectangle box = {g->player.x, g->player.y, PLAYER_SIZE, PLAYER_SIZE};
    int count = 0;
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        Item *it = &g->items[i];
        if (it->active && CheckCollisionCircleRec(it->pos, ITEM_RADIUS, box))
        {
            it->active = 0;
            count++;
        }
    }
    return count;
}

void updateTitle(Game *g, const Input *in)
{
    // check the input: only 1, 2 or 3 starts the game
    if (in->digit >= '1' && in->digit <= '3')
    {
        resetGame(g, in->digit - '1');
        g->state = STATE_PLAYING;
    }
    else if (in->digit != 0 || in->other || in->start)
    {
        showMessage(g, "Invalid key! Press 1, 2 or 3");
    }
}

void updatePlaying(Game *g, const Input *in, float dt)
{
    // delta time movement: speed * dt
    movePlayer(g, in->moveX * PLAYER_SPEED * dt, in->moveY * PLAYER_SPEED * dt);

    // a new star every SPAWN_TIME[difficulty] seconds
    g->spawnTimer += dt;
    if (g->spawnTimer >= SPAWN_TIME[g->difficulty])
    {
        g->spawnTimer = 0.0f;
        spawnItem(g);
    }

    // stars get older and disappear
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        Item *it = &g->items[i];
        if (!it->active)
            continue;
        it->life -= dt;
        if (it->life <= 0.0f)
            it->active = 0;
    }

    g->score += collectItems(g) * 10;

    // TODO 5: add your own game rules here
    //         (enemies that move, obstacles, lives, levels, power-ups, ...)

    g->timeLeft -= dt;
    if (g->timeLeft <= 0.0f)
    {
        g->timeLeft = 0.0f;
        if (g->score > g->hiScore)
            g->hiScore = g->score;
        // TODO 6: save the hi-score to a file (fopen / fprintf / fclose)
        g->state = STATE_GAME_OVER;
    }
}

void updateGameOver(Game *g, const Input *in)
{
    if (in->start)
        g->state = STATE_TITLE;
}

// FSM switch #1: update the current state
void updateGame(Game *g, const Input *in, float dt)
{
    g->blinkTimer += dt;
    if (g->messageTimer > 0.0f)
        g->messageTimer -= dt;

    switch (g->state)
    {
    case STATE_TITLE:
        updateTitle(g, in);
        break;
    case STATE_PLAYING:
        updatePlaying(g, in, dt);
        break;
    case STATE_GAME_OVER:
        updateGameOver(g, in);
        break;
    }
}

/* ---------- 4. DRAW ---------- */

// 1 for half a second, 0 for half a second
int blinkOn(const Game *g)
{
    return (int)(g->blinkTimer * 2.0f) % 2 == 0;
}

// draw text centered on the screen at height y
void drawTextCenter(const char *text, int y, int size, Color color)
{
    int w = MeasureText(text, size);
    DrawText(text, (SCREEN_W - w) / 2, y, size, color);
}

void drawTitle(const Game *g)
{
    // TODO 7: put your game name and instructions here
    drawTextCenter("MY MINI PROJECT", 110, 50, GOLD);
    drawTextCenter("Collect the stars!", 180, 24, RAYWHITE);
    drawTextCenter("Choose difficulty:", 260, 24, RAYWHITE);
    drawTextCenter("1  EASY     2  NORMAL     3  HARD", 300, 24, SKYBLUE);
    drawTextCenter(TextFormat("HI-SCORE: %d", g->hiScore), 370, 24, GOLD);
    drawTextCenter("WASD / ARROWS: MOVE     ESC: QUIT", 430, 20, GRAY);
    if (g->messageTimer > 0.0f && blinkOn(g))
        drawTextCenter(g->message, 500, 24, RED);
}

void drawPlaying(const Game *g)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        const Item *it = &g->items[i];
        if (!it->active)
            continue;
        // blink in the last second before it disappears
        if (it->life > 1.0f || blinkOn(g))
            DrawCircleV(it->pos, ITEM_RADIUS, GOLD);
    }

    // TODO 8: draw your own objects here (shapes or textures)
    DrawRectangleV(g->player, (Vector2){PLAYER_SIZE, PLAYER_SIZE}, SKYBLUE);

    // HUD bar
    DrawRectangle(0, 0, SCREEN_W, HUD_H, (Color){20, 20, 40, 255});
    DrawText(TextFormat("SCORE: %d", g->score), 16, 10, 20, RAYWHITE);
    DrawText(TextFormat("TIME: %d", (int)(g->timeLeft + 0.99f)), 360, 10, 20, RAYWHITE);
    DrawText(DIFFICULTY_NAME[g->difficulty], SCREEN_W - 120, 10, 20, SKYBLUE);
}

void drawGameOver(const Game *g)
{
    drawTextCenter("TIME UP!", 160, 50, RED);
    drawTextCenter(TextFormat("SCORE: %d", g->score), 260, 30, RAYWHITE);
    if (g->score > 0 && g->score >= g->hiScore)
        drawTextCenter("** NEW HI-SCORE! **", 320, 24, GOLD);
    if (blinkOn(g))
        drawTextCenter("PRESS ENTER / SPACE", 420, 24, RAYWHITE);
}

// FSM switch #2: draw the current state
void drawGame(const Game *g)
{
    BeginDrawing();
    ClearBackground((Color){10, 10, 25, 255});
    switch (g->state)
    {
    case STATE_TITLE:
        drawTitle(g);
        break;
    case STATE_PLAYING:
        drawPlaying(g);
        break;
    case STATE_GAME_OVER:
        drawGameOver(g);
        break;
    }
    EndDrawing();
}

/* ---------- main: the game loop ---------- */

int main(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "DG111 Mini Project");
    SetTargetFPS(60);

    Game game = {0};
    game.state = STATE_TITLE;
    // TODO 6: load the hi-score from a file here
    // TODO 9: load textures / sounds here (LoadTexture, InitAudioDevice, ...)

    while (!WindowShouldClose()) // ESC or the close button ends the loop
    {
        // 1. TIME
        float dt = GetFrameTime();

        // 2. INPUT
        Input in;
        readInput(&in);

        // 3. UPDATE
        updateGame(&game, &in, dt);

        // 4. DRAW
        drawGame(&game);
    }

    // TODO 9: unload textures / sounds here (UnloadTexture, CloseAudioDevice, ...)
    CloseWindow();
    return 0;
}
