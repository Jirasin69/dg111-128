// Tetris (single player, console) - standard C + conio.h / windows.h
// Controls: A/Left = move left, D/Right = move right, W/Up = rotate,
//           S/Down = soft drop, Space = hard drop, Q = quit
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define FIELD_W 10
#define FIELD_H 20

/* 7 tetrominoes, each drawn in a 4x4 grid ('X' = block) */
static const char SHAPES[7][17] = {
    "..X...X...X...X.", // I
    "..X..XX..X......", // Z
    ".X...XX...X.....", // S
    ".....XX..XX.....", // O
    "..X...XX..X.....", // T
    "..X...X...XX....", // L
    ".X...X..XX......"  // J
};

static int field[FIELD_H][FIELD_W]; // 0 = empty, 1 = locked block

static int score = 0;
static int lines = 0;
static int level = 1;

/* map (px, py) inside the 4x4 grid to an index after rotating r * 90 degrees */
int rotateIndex(int px, int py, int r)
{
    switch (r % 4)
    {
    case 0:
        return py * 4 + px;
    case 1:
        return 12 + py - (px * 4);
    case 2:
        return 15 - (py * 4) - px;
    default:
        return 3 - py + (px * 4);
    }
}

/* return 1 if the piece fits at (x, y) with rotation r */
int fits(int piece, int r, int x, int y)
{
    for (int py = 0; py < 4; py++)
    {
        for (int px = 0; px < 4; px++)
        {
            if (SHAPES[piece][rotateIndex(px, py, r)] != 'X')
            {
                continue;
            }
            int fx = x + px;
            int fy = y + py;
            if (fx < 0 || fx >= FIELD_W || fy >= FIELD_H)
            {
                return 0;
            }
            if (fy >= 0 && field[fy][fx])
            {
                return 0;
            }
        }
    }
    return 1;
}

/* copy the piece into the field */
void lockPiece(int piece, int r, int x, int y)
{
    for (int py = 0; py < 4; py++)
    {
        for (int px = 0; px < 4; px++)
        {
            if (SHAPES[piece][rotateIndex(px, py, r)] == 'X' && y + py >= 0)
            {
                field[y + py][x + px] = 1;
            }
        }
    }
}

/* remove full rows, update score / lines / level. Return rows cleared */
int clearLines(void)
{
    int cleared = 0;
    for (int y = FIELD_H - 1; y >= 0; y--)
    {
        int full = 1;
        for (int x = 0; x < FIELD_W; x++)
        {
            if (!field[y][x])
            {
                full = 0;
                break;
            }
        }
        if (full)
        {
            // move every row above down by one
            for (int yy = y; yy > 0; yy--)
            {
                memcpy(field[yy], field[yy - 1], sizeof(field[yy]));
            }
            memset(field[0], 0, sizeof(field[0]));
            cleared++;
            y++; // check this row again
        }
    }

    static const int points[5] = {0, 100, 300, 500, 800};
    score += points[cleared] * level;
    lines += cleared;
    level = lines / 10 + 1;
    return cleared;
}

/* draw the whole frame with one printf so it does not flicker */
void render(int piece, int r, int x, int y, int next)
{
    char buf[4096];
    int len = 0;

    // put cursor back to top-left instead of clearing the screen
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD origin = {0, 0};
    SetConsoleCursorPosition(h, origin);

    for (int fy = 0; fy < FIELD_H; fy++)
    {
        len += sprintf(buf + len, "<!");
        for (int fx = 0; fx < FIELD_W; fx++)
        {
            int filled = field[fy][fx];
            // is the falling piece covering this cell?
            int px = fx - x;
            int py = fy - y;
            if (px >= 0 && px < 4 && py >= 0 && py < 4 &&
                SHAPES[piece][rotateIndex(px, py, r)] == 'X')
            {
                filled = 1;
            }
            len += sprintf(buf + len, filled ? "[]" : " .");
        }
        len += sprintf(buf + len, "!>");

        // side panel
        switch (fy)
        {
        case 1:
            len += sprintf(buf + len, "   SCORE: %-8d", score);
            break;
        case 2:
            len += sprintf(buf + len, "   LINES: %-8d", lines);
            break;
        case 3:
            len += sprintf(buf + len, "   LEVEL: %-8d", level);
            break;
        case 5:
            len += sprintf(buf + len, "   NEXT:            ");
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        {
            int row = fy - 6;
            len += sprintf(buf + len, "   ");
            for (int col = 0; col < 4; col++)
            {
                len += sprintf(buf + len, SHAPES[next][row * 4 + col] == 'X' ? "[]" : "  ");
            }
            len += sprintf(buf + len, "    ");
            break;
        }
        case 12:
            len += sprintf(buf + len, "   A/D or Left/Right: move");
            break;
        case 13:
            len += sprintf(buf + len, "   W or Up: rotate         ");
            break;
        case 14:
            len += sprintf(buf + len, "   S or Down: soft drop    ");
            break;
        case 15:
            len += sprintf(buf + len, "   Space: hard drop        ");
            break;
        case 16:
            len += sprintf(buf + len, "   Q: quit                 ");
            break;
        default:
            len += sprintf(buf + len, "                           ");
            break;
        }
        len += sprintf(buf + len, "\n");
    }
    len += sprintf(buf + len, "<!====================!>\n");
    len += sprintf(buf + len, "  \\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\n");

    fputs(buf, stdout);
}

/* hide the blinking console cursor */
void hideCursor(void)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(h, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(h, &info);
}

int main(void)
{
    srand((unsigned)time(NULL));
    hideCursor();
    system("cls");

    int piece = rand() % 7;
    int next = rand() % 7;
    int rot = 0;
    int x = FIELD_W / 2 - 2;
    int y = 0;
    int gameOver = 0;
    int quit = 0;

    DWORD lastDrop = GetTickCount();

    render(piece, rot, x, y, next);

    while (!gameOver && !quit)
    {
        int dirty = 0;
        int lockNow = 0;

        /* ---- input ---- */
        while (kbhit())
        {
            int key = getch();
            if (key == 0 || key == 224)
            {
                // arrow keys come as two bytes: prefix, then code
                int code = getch();
                key = (code == 75) ? 'a' : (code == 77) ? 'd' : (code == 72) ? 'w' : (code == 80) ? 's' : 0;
            }
            switch (key)
            {
            case 'a':
            case 'A':
                if (fits(piece, rot, x - 1, y))
                {
                    x--;
                    dirty = 1;
                }
                break;
            case 'd':
            case 'D':
                if (fits(piece, rot, x + 1, y))
                {
                    x++;
                    dirty = 1;
                }
                break;
            case 'w':
            case 'W':
                if (fits(piece, rot + 1, x, y))
                {
                    rot = (rot + 1) % 4;
                    dirty = 1;
                }
                break;
            case 's':
            case 'S':
                if (fits(piece, rot, x, y + 1))
                {
                    y++;
                    score += 1;
                    lastDrop = GetTickCount();
                    dirty = 1;
                }
                else
                {
                    lockNow = 1;
                }
                break;
            case ' ':
                while (fits(piece, rot, x, y + 1))
                {
                    y++;
                    score += 2;
                }
                lockNow = 1;
                dirty = 1;
                break;
            case 'q':
            case 'Q':
                quit = 1;
                break;
            }
        }

        /* ---- gravity ---- */
        int interval = 800 - (level - 1) * 70;
        if (interval < 100)
        {
            interval = 100;
        }
        if (!lockNow && GetTickCount() - lastDrop >= (DWORD)interval)
        {
            if (fits(piece, rot, x, y + 1))
            {
                y++;
                dirty = 1;
            }
            else
            {
                lockNow = 1;
            }
            lastDrop = GetTickCount();
        }

        /* ---- lock piece and spawn the next one ---- */
        if (lockNow)
        {
            lockPiece(piece, rot, x, y);
            clearLines();

            piece = next;
            next = rand() % 7;
            rot = 0;
            x = FIELD_W / 2 - 2;
            y = 0;
            lastDrop = GetTickCount();
            dirty = 1;

            if (!fits(piece, rot, x, y))
            {
                gameOver = 1;
            }
        }

        if (dirty)
        {
            render(piece, rot, x, y, next);
        }

        Sleep(10);
    }

    if (gameOver)
    {
        printf("\n  GAME OVER!  Final score: %d  Lines: %d  Level: %d\n", score, lines, level);
    }
    else
    {
        printf("\n  Bye! Score: %d\n", score);
    }
    return 0;
}
