// save.h — บันทึก/โหลด GameState ด้วย binary file I/O
#ifndef SAVE_H
#define SAVE_H
#include "game.h"

int saveGame(const GameState *g, const char *filename);
int loadGame(GameState *g, const char *filename);

#endif // SAVE_H
