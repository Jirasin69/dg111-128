///
/// Snake - console game (Windows, C99)
///
/// Controls: W/A/S/D or Arrow keys = move, P = pause, Q = quit
///

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

#define BOARD_WIDTH 30
#define BOARD_HEIGHT 20
#define MAX_LENGTH (BOARD_WIDTH * BOARD_HEIGHT)
#define START_LENGTH 4
#define START_DELAY_MS 150
#define MIN_DELAY_MS 60
#define SPEEDUP_MS 5 // faster by this much per food eaten
#define POINTS_PER_FOOD 10

typedef struct
{
    int x;
    int y;
} Point;

typedef enum
{
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_QUIT
} GameState;

typedef struct
{
    Point body[MAX_LENGTH]; // body[0] is the head
    int length;
    Point dir;
    Point next_dir; // buffered so two quick keys cannot reverse the snake
    Point food;
    int score;
    int high_score;
    int delay_ms;
    GameState state;
} Game;

static Point point_make(int x, int y)
{
    Point p = {x, y};
    return p;
}

static int point_equals(Point a, Point b)
{
    return a.x == b.x && a.y == b.y;
}

// ---------- Console helpers ----------

static void console_set_cursor(int x, int y)
{
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

static void console_show_cursor(int visible)
{
    CONSOLE_CURSOR_INFO info = {25, visible};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

// ---------- Game logic ----------

static int snake_occupies(const Game *g, Point p)
{
    for (int i = 0; i < g->length; i++)
    {
        if (point_equals(g->body[i], p))
        {
            return 1;
        }
    }
    return 0;
}

static void place_food(Game *g)
{
    // The board never gets completely full before the win check, so this terminates
    do
    {
        g->food = point_make(rand() % BOARD_WIDTH, rand() % BOARD_HEIGHT);
    } while (snake_occupies(g, g->food));
}

static void game_reset(Game *g)
{
    g->length = START_LENGTH;
    for (int i = 0; i < g->length; i++)
    {
        g->body[i] = point_make(BOARD_WIDTH / 2 - i, BOARD_HEIGHT / 2);
    }
    g->dir = point_make(1, 0);
    g->next_dir = g->dir;
    g->score = 0;
    g->delay_ms = START_DELAY_MS;
    g->state = STATE_PLAYING;
    place_food(g);
}

static void set_direction(Game *g, Point d)
{
    // Ignore a 180-degree turn relative to the direction actually moved last
    if (d.x == -g->dir.x && d.y == -g->dir.y)
    {
        return;
    }
    g->next_dir = d;
}

static void handle_input(Game *g)
{
    while (_kbhit())
    {
        int key = _getch();

        if (key == 0 || key == 224)
        {
            // Extended key (arrows): second code follows
            switch (_getch())
            {
            case 72:
                set_direction(g, point_make(0, -1));
                break; // up
            case 80:
                set_direction(g, point_make(0, 1));
                break; // down
            case 75:
                set_direction(g, point_make(-1, 0));
                break; // left
            case 77:
                set_direction(g, point_make(1, 0));
                break; // right
            default:
                break;
            }
            continue;
        }

        switch (key)
        {
        case 'w':
        case 'W':
            set_direction(g, point_make(0, -1));
            break;
        case 's':
        case 'S':
            set_direction(g, point_make(0, 1));
            break;
        case 'a':
        case 'A':
            set_direction(g, point_make(-1, 0));
            break;
        case 'd':
        case 'D':
            set_direction(g, point_make(1, 0));
            break;
        case 'p':
        case 'P':
            if (g->state == STATE_PLAYING)
                g->state = STATE_PAUSED;
            else if (g->state == STATE_PAUSED)
                g->state = STATE_PLAYING;
            break;
        case 'q':
        case 'Q':
            g->state = STATE_QUIT;
            break;
        default:
            break;
        }
    }
}

static void update(Game *g)
{
    g->dir = g->next_dir;
    Point head = point_make(g->body[0].x + g->dir.x, g->body[0].y + g->dir.y);

    // Wall collision
    if (head.x < 0 || head.x >= BOARD_WIDTH || head.y < 0 || head.y >= BOARD_HEIGHT)
    {
        g->state = STATE_GAME_OVER;
        return;
    }

    int ate = point_equals(head, g->food);

    // Self collision: the tail cell is free this tick unless we are growing
    int check_len = ate ? g->length : g->length - 1;
    for (int i = 0; i < check_len; i++)
    {
        if (point_equals(g->body[i], head))
        {
            g->state = STATE_GAME_OVER;
            return;
        }
    }

    if (ate)
    {
        g->length++;
    }

    // Shift body toward the tail, then place the new head
    for (int i = g->length - 1; i > 0; i--)
    {
        g->body[i] = g->body[i - 1];
    }
    g->body[0] = head;

    if (ate)
    {
        g->score += POINTS_PER_FOOD;
        if (g->score > g->high_score)
        {
            g->high_score = g->score;
        }
        if (g->delay_ms > MIN_DELAY_MS)
        {
            g->delay_ms -= SPEEDUP_MS;
        }
        if (g->length >= MAX_LENGTH)
        {
            g->state = STATE_GAME_OVER; // board full: the player wins
            return;
        }
        place_food(g);
    }
}

// ---------- Rendering ----------

static void render(const Game *g)
{
    char board[BOARD_HEIGHT][BOARD_WIDTH];
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            board[y][x] = ' ';
        }
    }
    board[g->food.y][g->food.x] = '*';
    for (int i = g->length - 1; i >= 0; i--)
    {
        board[g->body[i].y][g->body[i].x] = (i == 0) ? '@' : 'o';
    }

    // Build the whole frame in one buffer and write it once to reduce flicker
    char frame[(BOARD_WIDTH + 3) * (BOARD_HEIGHT + 2) + 256];
    int n = 0;

    n += sprintf(frame + n, "Score: %-5d High: %-5d Speed: %dms\n",
                 g->score, g->high_score, g->delay_ms);

    frame[n++] = '+';
    for (int x = 0; x < BOARD_WIDTH; x++)
        frame[n++] = '-';
    frame[n++] = '+';
    frame[n++] = '\n';

    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        frame[n++] = '|';
        for (int x = 0; x < BOARD_WIDTH; x++)
            frame[n++] = board[y][x];
        frame[n++] = '|';
        frame[n++] = '\n';
    }

    frame[n++] = '+';
    for (int x = 0; x < BOARD_WIDTH; x++)
        frame[n++] = '-';
    frame[n++] = '+';
    frame[n++] = '\n';

    const char *status;
    if (g->state == STATE_PAUSED)
        status = "** PAUSED - press P to resume **          ";
    else if (g->state == STATE_GAME_OVER)
        status = "** GAME OVER - R: restart, Q: quit **     ";
    else
        status = "WASD/Arrows: move  P: pause  Q: quit     ";
    n += sprintf(frame + n, "%s\n", status);

    frame[n] = '\0';
    system("cls");
    fputs(frame, stdout);
    fflush(stdout);
}

// ---------- Main ----------

int main(void)
{
    srand((unsigned)time(NULL));
    system("cls");
    console_show_cursor(0);

    Game game = {0};
    game_reset(&game);

    DWORD last_tick = GetTickCount();
    GameState prev_state = STATE_QUIT; // differs from the first state, so frame 1 is drawn

    while (game.state != STATE_QUIT)
    {
        handle_input(&game);

        if (game.state == STATE_GAME_OVER)
        {
            // Wait for restart or quit
            render(&game);
            int key = _getch();
            if (key == 'r' || key == 'R')
            {
                int high = game.high_score;
                game_reset(&game);
                game.high_score = high;
                prev_state = STATE_QUIT; // force a redraw of the fresh game
                last_tick = GetTickCount();
            }
            else if (key == 'q' || key == 'Q')
            {
                game.state = STATE_QUIT;
            }
            continue;
        }

        DWORD now = GetTickCount();
        int redraw = (game.state != prev_state); // e.g. pause toggled
        if (game.state == STATE_PLAYING && now - last_tick >= (DWORD)game.delay_ms)
        {
            last_tick = now;
            update(&game);
            redraw = 1;
        }
        else if (game.state == STATE_PAUSED)
        {
            last_tick = now; // do not accumulate time while paused
        }
        prev_state = game.state;

        // cls is slow, so only redraw when something actually changed
        if (redraw)
        {
            render(&game);
        }
        Sleep(10);
    }

    console_show_cursor(1);
    console_set_cursor(0, BOARD_HEIGHT + 5);
    printf("Thanks for playing! Final high score: %d\n", game.high_score);
    return 0;
}
