// game.h — struct รวม + header guard + prototype ทั้งหมด
// include ได้จากทุกไฟล์ (main.c, game.c, render.c, save.c)
#ifndef GAME_H
#define GAME_H

#define MAX_NAME    32
#define MAX_ENEMIES 2
#define ROWS        6
#define COLS        10
#define SAVE_FILE   "dungeon_save.bin"

// tilemap คงที่: 0 = floor, 1 = wall, 2 = stairs down — เก็บจริงใน game.c
// แต่ render.c ต้องใช้วาดจอด้วย จึงประกาศ extern ไว้ที่นี่
extern int tilemap[ROWS][COLS];

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
    Enemy  enemies[MAX_ENEMIES];  // enemy pool (Week 10: Object Pool pattern)
    int    floor;
    int    running;        // 1 = playing, 0 = game over
    char   message[128];   // ข้อความล่าสุดที่จะโชว์
} GameState;

// ── game logic (game.c) ──
void initGame(GameState *g, const char *name);
void updateGame(GameState *g, char cmd);

// ── render (render.c) ──
void renderGame(const GameState *g);
void renderGameOver(const GameState *g);
char getPlayerInput(void);

// ── save / load (save.c) ──
int saveGame(const GameState *g, const char *filename);
int loadGame(GameState *g, const char *filename);

#endif // GAME_H
