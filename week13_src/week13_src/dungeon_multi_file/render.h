// render.h — ฟังก์ชันวาดจอ + รับ input เท่านั้น (ไม่มี logic)
// Week 13: prototype เหมือน Week 12 ทุกตัว — เพิ่มแค่ขนาดหน้าต่าง Raylib
#ifndef RENDER_H
#define RENDER_H
#include "game.h"

#define TILE        48
#define SCREEN_W    (COLS * TILE)
#define SCREEN_H    (ROWS * TILE + 120)   // + พื้นที่ HUD ด้านล่าง

void renderGame(const GameState *g);
void renderGameOver(const GameState *g);
char getPlayerInput(void);

#endif // RENDER_H
