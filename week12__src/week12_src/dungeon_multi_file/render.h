// render.h — ฟังก์ชันวาดจอ + รับ input เท่านั้น (ไม่มี logic)
#ifndef RENDER_H
#define RENDER_H
#include "game.h"

void renderGame(const GameState *g);
void renderGameOver(const GameState *g);
char getPlayerInput(void);

#endif // RENDER_H
