// Flappy Bird (console version) - standard C + conio.h / windows.h
//
// Rewritten in plain C from the idea of "Terminal Bird" by Ibrahimbag (a Flappy Bird
// clone that used ncurses, SDL2 sound, SQLite and cJSON). This version uses only the
// C standard library plus conio.h / windows.h, like the other games in this folder.
//
// Press Space (or W / Up) to flap. Fly through the gaps between the pipes.
// Every pipe you pass gives +1 point. Q quits, R restarts after game over.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define FIELD_W 60      // width of the play field (characters)
#define FIELD_H 20      // height of the play field (rows)
#define BIRD_X 10       // the bird stays in this column
#define PIPE_W 4        // width of a pipe
#define PIPE_GAP 6      // height of the gap in a pipe
#define PIPE_SPACING 24 // distance between two pipes
#define MAX_PIPES 4

#define TICK_MS 70      // time between two steps of the game
#define GRAVITY 0.25f   // how fast the bird speeds up while falling
#define JUMP_SPEED -1.2f // speed right after a flap (negative = up)
#define MAX_FALL 1.2f   // fastest falling speed

#define SCORE_FILE "highscore.txt"

#define KEY_ESC 27
#define KEY_SPACE 32

typedef struct
{
    int x;      // left column of the pipe
    int gapTop; // first row of the gap
    int passed; // 1 after the bird flew through this pipe
    int active; // 1 = on screen, 0 = free slot
} Pipe;

static Pipe pipes[MAX_PIPES];
static float birdY;
static float birdSpeed;
static int score;
static int best;

/* ---------- console helpers ---------- */

void clearScreen(void)
{
    system("cls");
}

void hideCursor(void)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(h, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(h, &info);
}

/* ---------- best score in a file (file I/O) ---------- */

int loadBest(void)
{
    int value = 0;
    FILE *f = fopen(SCORE_FILE, "r");
    if (f != NULL)
    {
        if (fscanf(f, "%d", &value) != 1)
        {
            value = 0;
        }
        fclose(f);
    }
    return value;
}

void saveBest(int value)
{
    FILE *f = fopen(SCORE_FILE, "w");
    if (f != NULL)
    {
        fprintf(f, "%d\n", value);
        fclose(f);
    }
}

/* ---------- pipes ---------- */

// create a new pipe on the right side. The gap moves at most 5 rows from the last gap
void spawnPipe(int previousGap)
{
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (!pipes[i].active)
        {
            int minTop = 1;
            int maxTop = FIELD_H - PIPE_GAP - 1;
            int gap = previousGap + rand() % 11 - 5;
            if (gap < minTop)
            {
                gap = minTop;
            }
            if (gap > maxTop)
            {
                gap = maxTop;
            }
            pipes[i].x = FIELD_W;
            pipes[i].gapTop = gap;
            pipes[i].passed = 0;
            pipes[i].active = 1;
            return;
        }
    }
}

// x of the right-most active pipe (-1 if there is none)
int lastPipeIndex(void)
{
    int last = -1;
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (pipes[i].active && (last == -1 || pipes[i].x > pipes[last].x))
        {
            last = i;
        }
    }
    return last;
}

void resetGame(void)
{
    birdY = FIELD_H / 2;
    birdSpeed = 0.0f;
    score = 0;
    for (int i = 0; i < MAX_PIPES; i++)
    {
        pipes[i].active = 0;
    }
    spawnPipe(FIELD_H / 2 - PIPE_GAP / 2);
}

// return 1 if the bird hit the ground, the ceiling or a pipe
int birdCrashed(void)
{
    int by = (int)(birdY + 0.5f);
    if (by < 0 || by >= FIELD_H)
    {
        return 1;
    }
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (pipes[i].active && BIRD_X >= pipes[i].x && BIRD_X < pipes[i].x + PIPE_W)
        {
            if (by < pipes[i].gapTop || by >= pipes[i].gapTop + PIPE_GAP)
            {
                return 1;
            }
        }
    }
    return 0;
}

/* ---------- drawing ---------- */

// draw the whole frame with one fputs so it does not flicker
void render(void)
{
    char field[FIELD_H][FIELD_W + 1];
    for (int y = 0; y < FIELD_H; y++)
    {
        for (int x = 0; x < FIELD_W; x++)
        {
            field[y][x] = ' ';
        }
        field[y][FIELD_W] = '\0';
    }

    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (!pipes[i].active)
        {
            continue;
        }
        for (int x = pipes[i].x; x < pipes[i].x + PIPE_W; x++)
        {
            if (x < 0 || x >= FIELD_W)
            {
                continue;
            }
            for (int y = 0; y < FIELD_H; y++)
            {
                if (y < pipes[i].gapTop || y >= pipes[i].gapTop + PIPE_GAP)
                {
                    field[y][x] = '#';
                }
            }
        }
    }

    int by = (int)(birdY + 0.5f);
    if (by >= 0 && by < FIELD_H)
    {
        field[by][BIRD_X] = 'O';
        field[by][BIRD_X + 1] = '>';
    }

    char buf[4096];
    int len = 0;

    // put cursor back to top-left instead of clearing the screen
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD origin = {0, 0};
    SetConsoleCursorPosition(h, origin);

    len += sprintf(buf + len, " SCORE: %-5d  BEST: %-5d   Space = flap, Q = quit\n", score, best);
    len += sprintf(buf + len, "+------------------------------------------------------------+\n");
    for (int y = 0; y < FIELD_H; y++)
    {
        len += sprintf(buf + len, "|%s|\n", field[y]);
    }
    len += sprintf(buf + len, "+------------------------------------------------------------+\n");

    fputs(buf, stdout);
}

/* ---------- game ---------- */

// wait for one of the keys. Return the key that was pressed
int waitForKey(const char *keys)
{
    while (1)
    {
        int key = getch();
        for (int i = 0; keys[i] != '\0'; i++)
        {
            if (key == keys[i] || (key >= 'A' && key <= 'Z' && key + 32 == keys[i]))
            {
                return keys[i];
            }
        }
    }
}

// play one round. Return 1 if the player wants to quit the program
int playRound(void)
{
    resetGame();
    clearScreen();
    render();

    DWORD lastTick = GetTickCount();

    while (1)
    {
        /* ---- input ---- */
        while (kbhit())
        {
            int key = getch();
            if (key == 0 || key == 224)
            {
                int code = getch();
                key = (code == 72) ? 'w' : 0; // Up arrow
            }
            if (key == KEY_SPACE || key == 'w' || key == 'W')
            {
                birdSpeed = JUMP_SPEED; // flap
            }
            else if (key == 'q' || key == 'Q' || key == KEY_ESC)
            {
                return 1;
            }
        }

        /* ---- one step of the game ---- */
        if (GetTickCount() - lastTick >= TICK_MS)
        {
            lastTick = GetTickCount();

            // bird: gravity, then move
            birdSpeed += GRAVITY;
            if (birdSpeed > MAX_FALL)
            {
                birdSpeed = MAX_FALL;
            }
            birdY += birdSpeed;

            // pipes move left by one column
            for (int i = 0; i < MAX_PIPES; i++)
            {
                if (!pipes[i].active)
                {
                    continue;
                }
                pipes[i].x--;
                if (!pipes[i].passed && pipes[i].x + PIPE_W <= BIRD_X)
                {
                    pipes[i].passed = 1;
                    score++;
                }
                if (pipes[i].x + PIPE_W < 0)
                {
                    pipes[i].active = 0; // left the screen
                }
            }

            // add a new pipe when the last one is far enough from the right edge
            int last = lastPipeIndex();
            if (last == -1 || pipes[last].x <= FIELD_W - PIPE_SPACING)
            {
                spawnPipe(last == -1 ? FIELD_H / 2 - PIPE_GAP / 2 : pipes[last].gapTop);
            }

            render();

            if (birdCrashed())
            {
                return 0;
            }
        }

        Sleep(5);
    }
}

int main(void)
{
    srand((unsigned)time(NULL));
    best = loadBest();

    clearScreen();
    printf("=== FLAPPY BIRD (console) ===\n\n");
    printf("  Space / W / Up : flap\n");
    printf("  Q / Esc        : quit\n\n");
    printf("  Fly through the gaps between the pipes (#).\n");
    printf("  Every pipe you pass gives +1 point.\n\n");
    printf("Press Space to start...");
    waitForKey(" ");

    hideCursor();

    while (1)
    {
        int quit = playRound();
        if (quit)
        {
            break;
        }

        if (score > best)
        {
            best = score;
            saveBest(best);
        }

        printf("\n  GAME OVER!  Score: %d   Best: %d\n", score, best);
        printf("  Press R to play again or Q to quit... ");
        if (waitForKey("rq") == 'q')
        {
            break;
        }
    }

    printf("\n");
    return 0;
}
