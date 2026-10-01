// save.c — บันทึก/โหลด GameState ทั้งก้อนด้วย fwrite/fread (binary)
#include <stdio.h>
#include "save.h"

int saveGame(const GameState *g, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f)
        return 0;
    fwrite(g, sizeof(GameState), 1, f);  // เขียนทั้ง struct รวดเดียว
    fclose(f);
    return 1;
}

int loadGame(GameState *g, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f)
        return 0;
    fread(g, sizeof(GameState), 1, f);
    fclose(f);
    return 1;
}
