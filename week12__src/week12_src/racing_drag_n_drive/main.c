// Drag 'n' Drive (console version) - standard C + conio.h / windows.h
//
// Rewritten in plain C from the idea of the game "Drag 'n' Drive" by
// Rupali Singh and Aniket Kumar (Apache License 2.0), which used Turbo C graphics.h.
//
// Drive your car left and right, dodge the falling obstacles and keep 3 lives.
// Each obstacle you pass gives +10 points, and the road gets faster as you score.
// Best scores are saved in myscore.txt.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define ROAD_W 24    // width of the road in characters
#define ROAD_H 20    // height of the road in rows
#define CAR_W 3      // width of the car and of an obstacle
#define MAX_LIVES 3
#define NUM_OBS 2    // obstacles on the road at the same time
#define TOP_SCORES 5
#define SCORE_FILE "myscore.txt"

#define KEY_ESC 27
#define KEY_ENTER 13
#define KEY_SPACE 32

static const char *CAR_GLYPH[4] = {"[#]", "<O>", "/^\\", "(=)"};

typedef struct
{
    int x; // left column of the obstacle
    int y; // row (negative = still waiting above the road)
} Obstacle;

static int topScores[TOP_SCORES]; // best scores, highest first
static int carChoice = 0;         // index into CAR_GLYPH

/* ---------- console helpers ---------- */

void clearScreen(void)
{
    system("cls");
}

void setCursorVisible(int visible)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(h, &info);
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(h, &info);
}

// wait until the player presses the given key
void waitForKey(int key)
{
    while (getch() != key)
    {
        ;
    }
}

/* ---------- high score file (file I/O) ---------- */

void loadScores(void)
{
    for (int i = 0; i < TOP_SCORES; i++)
    {
        topScores[i] = 0;
    }
    FILE *f = fopen(SCORE_FILE, "r");
    if (f == NULL)
    {
        return; // no file yet: all scores are 0
    }
    for (int i = 0; i < TOP_SCORES; i++)
    {
        if (fscanf(f, "%d", &topScores[i]) != 1)
        {
            break;
        }
    }
    fclose(f);
}

void saveScores(void)
{
    FILE *f = fopen(SCORE_FILE, "w");
    if (f == NULL)
    {
        return;
    }
    for (int i = 0; i < TOP_SCORES; i++)
    {
        fprintf(f, "%d\n", topScores[i]);
    }
    fclose(f);
}

// insert a score into the sorted list. Return 1 if it is the new best score
int addScore(int score)
{
    int pos = TOP_SCORES;
    for (int i = 0; i < TOP_SCORES; i++)
    {
        if (score > topScores[i])
        {
            pos = i;
            break;
        }
    }
    if (pos == TOP_SCORES)
    {
        return 0;
    }
    for (int i = TOP_SCORES - 1; i > pos; i--)
    {
        topScores[i] = topScores[i - 1];
    }
    topScores[pos] = score;
    saveScores();
    return pos == 0;
}

/* ---------- screens ---------- */

void showHighScores(void)
{
    clearScreen();
    printf("=== HIGH SCORES ===\n\n");
    for (int i = 0; i < TOP_SCORES; i++)
    {
        printf("  %d. %d\n", i + 1, topScores[i]);
    }
    printf("\nPress Enter to go back...");
    waitForKey(KEY_ENTER);
}

void showInstructions(void)
{
    clearScreen();
    printf("=== INSTRUCTIONS ===\n\n");
    printf("  Left / A   : move the car left\n");
    printf("  Right / D  : move the car right\n");
    printf("  Space      : pause / resume\n");
    printf("  Q / Esc    : back to the main menu\n\n");
    printf("  Dodge the falling obstacles (XXX).\n");
    printf("  You have %d lives. Each obstacle you pass gives +10 points.\n", MAX_LIVES);
    printf("  The road gets faster as your score grows.\n");
    printf("\nPress Enter to go back...");
    waitForKey(KEY_ENTER);
}

void showAbout(void)
{
    clearScreen();
    printf("=== ABOUT ===\n\n");
    printf("  Drag 'n' Drive - console version in standard C.\n");
    printf("  Based on the idea of the game by Rupali Singh and Aniket Kumar\n");
    printf("  (Apache License 2.0).\n");
    printf("\nPress Enter to go back...");
    waitForKey(KEY_ENTER);
}

void selectCar(void)
{
    clearScreen();
    printf("=== CHOOSE YOUR CAR ===\n\n");
    for (int i = 0; i < 4; i++)
    {
        printf("  %d) %s\n", i + 1, CAR_GLYPH[i]);
    }
    printf("\nInput: press 1, 2, 3 or 4 > ");
    while (1)
    {
        int key = getch();
        if (key >= '1' && key <= '4')
        {
            carChoice = key - '1';
            return;
        }
    }
}

/* ---------- game ---------- */

// time between two steps (milliseconds). Smaller = faster
int stepDelay(int score)
{
    if (score < 50)
        return 150;
    if (score < 100)
        return 135;
    if (score < 200)
        return 120;
    if (score < 300)
        return 105;
    if (score < 400)
        return 90;
    if (score < 600)
        return 75;
    return 60;
}

int levelOf(int score)
{
    return (150 - stepDelay(score)) / 15 + 1;
}

void placeObstacle(Obstacle *o, int y)
{
    o->x = rand() % (ROAD_W - CAR_W + 1);
    o->y = y;
}

void resetObstacles(Obstacle obs[])
{
    for (int i = 0; i < NUM_OBS; i++)
    {
        // obstacles start spread out, so they do not arrive together
        placeObstacle(&obs[i], -i * (ROAD_H / NUM_OBS));
    }
}

// return 1 if the car (column px) hits an obstacle
int hitsObstacle(const Obstacle obs[], int px)
{
    for (int i = 0; i < NUM_OBS; i++)
    {
        int dx = obs[i].x - px;
        if (obs[i].y == ROAD_H - 1 && dx > -CAR_W && dx < CAR_W)
        {
            return 1;
        }
    }
    return 0;
}

// draw the whole frame with one fputs so it does not flicker
void render(const Obstacle obs[], int px, int score, int lives, int scroll, int paused)
{
    char road[ROAD_H][ROAD_W + 1];
    for (int y = 0; y < ROAD_H; y++)
    {
        memset(road[y], ' ', ROAD_W);
        road[y][ROAD_W] = '\0';
        if ((y + scroll) % 4 < 2)
        {
            road[y][ROAD_W / 2] = '.'; // dashed center line
        }
    }
    for (int i = 0; i < NUM_OBS; i++)
    {
        if (obs[i].y >= 0 && obs[i].y < ROAD_H)
        {
            memcpy(&road[obs[i].y][obs[i].x], "XXX", CAR_W);
        }
    }
    memcpy(&road[ROAD_H - 1][px], CAR_GLYPH[carChoice], CAR_W);

    char buf[4096];
    int len = 0;

    // put cursor back to top-left instead of clearing the screen
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD origin = {0, 0};
    SetConsoleCursorPosition(h, origin);

    len += sprintf(buf + len, "+------------------------+\n");
    for (int y = 0; y < ROAD_H; y++)
    {
        len += sprintf(buf + len, "|%s|", road[y]);
        switch (y)
        {
        case 1:
            len += sprintf(buf + len, "  SCORE: %-8d", score);
            break;
        case 2:
            len += sprintf(buf + len, "  LEVEL: %-8d", levelOf(score));
            break;
        case 3:
            len += sprintf(buf + len, "  LIVES: ");
            for (int l = 0; l < MAX_LIVES; l++)
            {
                len += sprintf(buf + len, l < lives ? "<3 " : "   ");
            }
            len += sprintf(buf + len, "  ");
            break;
        case 6:
            len += sprintf(buf + len, "  %-22s", paused ? "*** PAUSED ***" : "");
            break;
        case 9:
            len += sprintf(buf + len, "  A/Left, D/Right: move  ");
            break;
        case 10:
            len += sprintf(buf + len, "  Space: pause / resume  ");
            break;
        case 11:
            len += sprintf(buf + len, "  Q / Esc: back to menu  ");
            break;
        default:
            len += sprintf(buf + len, "                         ");
            break;
        }
        len += sprintf(buf + len, "\n");
    }
    len += sprintf(buf + len, "+------------------------+\n");

    fputs(buf, stdout);
}

// one full game. Return the score, or -1 if the player pressed Esc
int playGame(void)
{
    Obstacle obs[NUM_OBS];
    int px = (ROAD_W - CAR_W) / 2;
    int score = 0;
    int lives = MAX_LIVES;
    int scroll = 0;
    int paused = 0;

    resetObstacles(obs);

    clearScreen();
    setCursorVisible(0);
    render(obs, px, score, lives, scroll, paused);

    DWORD lastStep = GetTickCount();

    while (1)
    {
        int dirty = 0;

        /* ---- input ---- */
        while (kbhit())
        {
            int key = getch();
            if (key == 0 || key == 224)
            {
                // arrow keys come as two bytes: prefix, then code
                int code = getch();
                key = (code == 75) ? 'a' : (code == 77) ? 'd' : 0;
            }
            if (key == KEY_ESC || key == 'q' || key == 'Q')
            {
                setCursorVisible(1);
                return -1;
            }
            if (key == KEY_SPACE)
            {
                paused = !paused;
                lastStep = GetTickCount();
                dirty = 1;
            }
            else if (!paused && (key == 'a' || key == 'A') && px > 0)
            {
                px -= 2;
                if (px < 0)
                {
                    px = 0;
                }
                dirty = 1;
            }
            else if (!paused && (key == 'd' || key == 'D') && px < ROAD_W - CAR_W)
            {
                px += 2;
                if (px > ROAD_W - CAR_W)
                {
                    px = ROAD_W - CAR_W;
                }
                dirty = 1;
            }
        }

        /* ---- move the road ---- */
        if (!paused && GetTickCount() - lastStep >= (DWORD)stepDelay(score))
        {
            lastStep = GetTickCount();
            scroll++;
            for (int i = 0; i < NUM_OBS; i++)
            {
                obs[i].y++;
                if (obs[i].y >= ROAD_H)
                {
                    score += 10; // the car passed this obstacle
                    placeObstacle(&obs[i], 0);
                }
            }
            dirty = 1;
        }

        /* ---- collision ---- */
        if (!paused && hitsObstacle(obs, px))
        {
            lives--;
            render(obs, px, score, lives, scroll, paused);
            setCursorVisible(1);
            if (lives == 0)
            {
                printf("\n  CRASH! No lives left.\n");
                return score;
            }
            printf("\n  CRASH! Lives left: %d. Press Enter to continue...", lives);
            waitForKey(KEY_ENTER);
            resetObstacles(obs);
            clearScreen();
            setCursorVisible(0);
            lastStep = GetTickCount();
            dirty = 1;
        }

        if (dirty)
        {
            render(obs, px, score, lives, scroll, paused);
        }

        Sleep(10);
    }
}

void playAndSave(void)
{
    selectCar();
    int score = playGame();
    if (score < 0)
    {
        return; // player left with Esc
    }

    printf("\n  GAME OVER!  Your score: %d\n", score);
    if (addScore(score))
    {
        printf("  New best score!\n");
    }
    printf("\n  Press Enter to return to the main menu...");
    waitForKey(KEY_ENTER);
}

int main(void)
{
    srand((unsigned)time(NULL));
    loadScores();

    while (1)
    {
        clearScreen();
        printf("=== DRAG 'N' DRIVE ===\n\n");
        printf("  1. Play\n");
        printf("  2. High scores\n");
        printf("  3. Instructions\n");
        printf("  4. About\n");
        printf("  0. Exit (or Q)\n\n");
        printf("Input: press a number key > ");

        int key = getch();
        switch (key)
        {
        case '1':
            playAndSave();
            break;
        case '2':
            showHighScores();
            break;
        case '3':
            showInstructions();
            break;
        case '4':
            showAbout();
            break;
        case '0':
        case 'q':
        case 'Q':
            return 0;
        }
    }
}
