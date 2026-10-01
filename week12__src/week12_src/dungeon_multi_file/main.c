// main.c — game loop: draw -> input -> update -> save เมื่อสั่ง (กด V)
#include <stdio.h>
#include "game.h"

int main(void) {
    GameState game;
    char name[MAX_NAME];

    printf("=== Dungeon Explorer ===\n");
    printf("Enter your name: ");
    scanf("%31s", name);
    initGame(&game, name);

    while (game.running) {
        renderGame(&game);           // DRAW
        char cmd = getPlayerInput(); // INPUT
        updateGame(&game, cmd);      // UPDATE
        if (cmd == 'v' || cmd == 'V')
            saveGame(&game, SAVE_FILE); // save เฉพาะตอนผู้เล่นสั่ง (กด V)
    }

    renderGameOver(&game);
    return 0;
}
