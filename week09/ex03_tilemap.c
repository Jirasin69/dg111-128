#include <stdio.h>
#define ROWS 8
#define COLS 12
// 0=floor, 1=wall, 2=water, 3=player_start
int tilemap[ROWS][COLS] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 3, 0, 0, 0, 2, 2, 0, 0, 0, 0, 1},
{1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1},
{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
{1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
// Print the whole tilemap: walk every row (i) and column (j),
// then decide one character to print per tile.
// '#' = WALL, '.' = FLOOR, '~' = WATER, '@' = PLAYER
void draw(int playerCol, int playerRow)
{
for (int i = 0; i < ROWS; i++) // i = row index
{
for (int j = 0; j < COLS; j++) // j = column index
{
if (i == playerRow && j == playerCol) // player isstanding on this tile
printf("@");
else if (tilemap[i][j] == 1) // tile value 1 = wall
printf("#");
else if (tilemap[i][j] == 2) // tile value 2 = water
printf("~");
else // tile value 0 (or 3 = start) = floor
printf(".");
}
printf("\n"); // end of row — move to the next line
}
}
int main()
{
int playerCol = 1, playerRow = 1; // player's startingposition — must be a floor tile
char move;
while (1)
{
draw(playerCol, playerRow);
printf("Move [wasd] or Quit [q]: ");
scanf(" %c", &move);
if (move == 'q')
break; // quit the game
// Update logic: compute the tentative next positionfrom the key pressed
int nextCol = playerCol, nextRow = playerRow;
if (move == 'w')
nextRow--; // up → move to the row above (row -1)
if (move == 's')
nextRow++; // down → move to the row below (row +1)
if (move == 'a')
nextCol--; // left → move to the column before (col- 1)
if (move == 'd')
nextCol++; // right → move to the column after (col+ 1)
// Collision check: commit the move only if thedestination isn't a wall (tile == 1)
if (tilemap[nextRow][nextCol] != 1)
{
playerCol = nextCol;
playerRow = nextRow;
}
}
return 0;
}