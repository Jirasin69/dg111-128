// Typing Game (console) - standard C + conio.h / windows.h
// Words fall from the top. Type the LOWEST word letter by letter before it
// touches the ground. Press Q (or Esc) to quit.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define FIELD_W 40
#define FIELD_H 18
#define MAX_WORDS 6
#define KEY_ESC 27

static const char *WORD_LIST[] = {
    "apple", "banana", "cherry", "orange", "grape", "lemon", "mango", "peach",
    "table", "chair", "window", "door", "house", "garden", "kitchen", "bridge",
    "planet", "rocket", "star", "moon", "cloud", "river", "ocean", "island",
    "keyboard", "monitor", "program", "function", "variable", "compile", "pointer", "struct",
    "array", "string", "integer", "float", "loop", "switch", "return", "printf",
    "dragon", "knight", "castle", "sword", "shield", "wizard", "potion", "treasure"};
#define WORD_COUNT ((int)(sizeof(WORD_LIST) / sizeof(WORD_LIST[0])))

typedef struct
{
    const char *text;
    int x;
    int y;
    int typed;  // how many letters are already typed correctly
    int active; // 1 = falling, 0 = free slot
} Word;

static Word words[MAX_WORDS];

static int wordsDone = 0;
static int letters = 0;
static int errors = 0;

/* put a new random word at the top in a free slot. Return 1 on success */
int spawnWord(void)
{
    for (int i = 0; i < MAX_WORDS; i++)
    {
        if (!words[i].active)
        {
            const char *text = WORD_LIST[rand() % WORD_COUNT];
            int len = (int)strlen(text);
            words[i].text = text;
            words[i].x = rand() % (FIELD_W - len + 1);
            words[i].y = 0;
            words[i].typed = 0;
            words[i].active = 1;
            return 1;
        }
    }
    return 0;
}

/* the word closest to the ground is the one the player must type. -1 if none */
int findTarget(void)
{
    int best = -1;
    for (int i = 0; i < MAX_WORDS; i++)
    {
        if (words[i].active && (best == -1 || words[i].y > words[best].y))
        {
            best = i;
        }
    }
    return best;
}

void hideCursor(void)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(h, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(h, &info);
}

/* draw the whole frame with one fputs so it does not flicker */
void render(DWORD startTime)
{
    char grid[FIELD_H][FIELD_W + 1];
    for (int y = 0; y < FIELD_H; y++)
    {
        memset(grid[y], ' ', FIELD_W);
        grid[y][FIELD_W] = '\0';
    }

    // place every word in the grid (typed letters are shown in UPPERCASE)
    for (int i = 0; i < MAX_WORDS; i++)
    {
        if (!words[i].active || words[i].y >= FIELD_H)
        {
            continue;
        }
        for (int c = 0; words[i].text[c] != '\0'; c++)
        {
            char ch = words[i].text[c];
            grid[words[i].y][words[i].x + c] = (c < words[i].typed) ? (char)toupper(ch) : ch;
        }
    }

    double minutes = (GetTickCount() - startTime) / 60000.0;
    double rate = (minutes > 0.0) ? wordsDone / minutes : 0.0;

    int target = findTarget();
    char targetText[32] = "";
    if (target != -1)
    {
        for (int c = 0; words[target].text[c] != '\0'; c++)
        {
            char ch = words[target].text[c];
            targetText[c] = (c < words[target].typed) ? (char)toupper(ch) : ch;
            targetText[c + 1] = '\0';
        }
    }

    char buf[4096];
    int len = 0;

    // put cursor back to top-left instead of clearing the screen
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD origin = {0, 0};
    SetConsoleCursorPosition(h, origin);

    len += sprintf(buf + len, "+----------------------------------------+\n");
    for (int y = 0; y < FIELD_H; y++)
    {
        len += sprintf(buf + len, "|%s|", grid[y]);
        switch (y)
        {
        case 1:
            len += sprintf(buf + len, "  WORDS:   %-8d", wordsDone);
            break;
        case 2:
            len += sprintf(buf + len, "  LETTERS: %-8d", letters);
            break;
        case 3:
            len += sprintf(buf + len, "  ERRORS:  %-8d", errors);
            break;
        case 4:
            len += sprintf(buf + len, "  SPEED:   %-5.1f w/m", rate);
            break;
        case 6:
            len += sprintf(buf + len, "  TARGET:  %-14s", targetText);
            break;
        case 9:
            len += sprintf(buf + len, "  Type the lowest word.  ");
            break;
        case 10:
            len += sprintf(buf + len, "  Typed letters turn     ");
            break;
        case 11:
            len += sprintf(buf + len, "  UPPERCASE.             ");
            break;
        case 13:
            len += sprintf(buf + len, "  Q / Esc: quit          ");
            break;
        default:
            len += sprintf(buf + len, "                         ");
            break;
        }
        len += sprintf(buf + len, "\n");
    }
    len += sprintf(buf + len, "+----------------------------------------+\n");
    len += sprintf(buf + len, "############# GROUND ####################\n");

    fputs(buf, stdout);
}

/* ask for difficulty. Return the falling interval in milliseconds */
int chooseDifficulty(void)
{
    int n = 2;

    system("cls");
    printf("Welcome to the Typing Game!\n\n");
    printf("Words fall from the top of the screen.\n");
    printf("Type the LOWEST word letter by letter before it touches the ground.\n\n");
    printf("Select difficulty:\n");
    printf("  1 - Difficult (fast)\n");
    printf("  2 - Normal\n");
    printf("  3 - Easy (slow)\n");
    printf("Input: type 1, 2 or 3 (Q to quit), then press Enter > ");

    if (scanf("%d", &n) != 1)
    {
        char c = 0;
        scanf(" %c", &c);
        if (c == 'q' || c == 'Q')
        {
            exit(0);
        }
        scanf("%*s"); // discard the rest of the non-numeric input
        n = 0;
    }

    switch (n)
    {
    case 1:
        return 500;
    case 3:
        return 2000;
    case 2:
        return 1000;
    default:
        printf("Invalid input! Using Normal difficulty.\n");
        Sleep(1500);
        return 1000;
    }
}

int main(void)
{
    srand((unsigned)time(NULL));

    int interval = chooseDifficulty();

    system("cls");
    hideCursor();

    spawnWord();
    int tick = 0;
    int fail = 0;
    int quit = 0;
    DWORD startTime = GetTickCount();
    DWORD lastTick = startTime;

    render(startTime);

    while (!fail && !quit)
    {
        int dirty = 0;

        /* ---- input ---- */
        while (kbhit())
        {
            int key = getch();
            // no word in WORD_LIST contains the letter q, so Q is free to quit
            if (key == KEY_ESC || key == 'q' || key == 'Q')
            {
                quit = 1;
                break;
            }
            if (key == 0 || key == 224)
            {
                getch(); // skip the second byte of special keys
                continue;
            }
            if (!isalpha(key))
            {
                continue;
            }

            int target = findTarget();
            if (target != -1)
            {
                Word *w = &words[target];
                if (w->text[w->typed] == tolower(key))
                {
                    w->typed++;
                    letters++;
                    if (w->text[w->typed] == '\0')
                    {
                        wordsDone++;
                        w->active = 0;
                    }
                }
                else
                {
                    errors++;
                }
                dirty = 1;
            }
        }

        /* ---- gravity ---- */
        DWORD now = GetTickCount();
        if (now - lastTick >= (DWORD)interval)
        {
            lastTick = now;
            for (int i = 0; i < MAX_WORDS; i++)
            {
                if (words[i].active)
                {
                    words[i].y++;
                    if (words[i].y >= FIELD_H)
                    {
                        fail = 1;
                    }
                }
            }
            tick++;
            if (tick % 2 == 0)
            {
                spawnWord();
            }
            dirty = 1;
        }

        // never leave the field empty
        if (findTarget() == -1)
        {
            spawnWord();
            dirty = 1;
        }

        if (dirty)
        {
            render(startTime);
        }

        Sleep(10);
    }

    double minutes = (GetTickCount() - startTime) / 60000.0;
    double rate = (minutes > 0.0) ? wordsDone / minutes : 0.0;

    printf("\n%s\n", fail ? "GAME OVER! A word touched the ground." : "You quit the game.");
    printf("Words typed:   %d\n", wordsDone);
    printf("Letters typed: %d\n", letters);
    printf("Wrong keys:    %d\n", errors);
    printf("Speed:         %.1f words/min\n", rate);
    printf("\nPress any key to exit...");
    getch();

    return 0;
}
