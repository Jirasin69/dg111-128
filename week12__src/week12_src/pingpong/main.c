// Ping Pong (vertical, player vs CPU, console) - standard C + conio.h / windows.h
// The CPU paddle is at the top, yours is at the bottom. Bounce the ball back
// and let it slip past the CPU paddle. First to WIN_SCORE points wins.
// Controls: A/Left = move left, D/Right = move right,
//           Enter/Space = start / serve, Q = quit
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define FIELD_W 20    // each cell is drawn 2 characters wide
#define FIELD_H 22
#define PADDLE_W 5
#define WIN_SCORE 2
#define START_DELAY_MS 90  // ball step time; gets smaller during a rally
#define MIN_DELAY_MS 45

typedef struct
{
    int x, y;   // ball cell
    int dx, dy; // -1 or 1
} Ball;

static int playerX;  // left cell of the bottom paddle
static int cpuX;     // left cell of the top paddle
static Ball ball;
static int playerScore = 0;
static int cpuScore = 0;
static int rally = 0;
static const char *message = "ENTER/SPACE: start";
static int waiting = 1; // 1 = waiting for Enter/Space before the ball moves

/* put the ball in the middle and send it toward the given side */
void serve(int towardPlayer)
{
    ball.x = FIELD_W / 2;
    ball.y = FIELD_H / 2;
    ball.dx = (rand() % 2) ? 1 : -1;
    ball.dy = towardPlayer ? 1 : -1;
    playerX = (FIELD_W - PADDLE_W) / 2;
    cpuX = (FIELD_W - PADDLE_W) / 2;
    rally = 0;
}

int paddleCovers(int paddleX, int x)
{
    return x >= paddleX && x < paddleX + PADDLE_W;
}

/* hitting with the paddle edge changes the ball's sideways direction */
void bounceOffPaddle(int paddleX)
{
    if (ball.x < paddleX + PADDLE_W / 3)
    {
        ball.dx = -1;
    }
    else if (ball.x >= paddleX + PADDLE_W - PADDLE_W / 3)
    {
        ball.dx = 1;
    }
    rally++;
}

/* move the ball one step. Return 1 if the player scored, -1 if the CPU scored, else 0 */
int moveBall(void)
{
    // side walls
    if (ball.x + ball.dx < 0 || ball.x + ball.dx >= FIELD_W)
    {
        ball.dx = -ball.dx;
    }
    ball.x += ball.dx;

    int nextY = ball.y + ball.dy;

    if (nextY <= 0) // top edge: CPU paddle row
    {
        if (!paddleCovers(cpuX, ball.x))
        {
            return 1; // ball got past the CPU
        }
        ball.dy = 1;
        bounceOffPaddle(cpuX);
        nextY = ball.y + ball.dy;
    }
    else if (nextY >= FIELD_H - 1) // bottom edge: player paddle row
    {
        if (!paddleCovers(playerX, ball.x))
        {
            return -1; // ball got past the player
        }
        ball.dy = -1;
        bounceOffPaddle(playerX);
        nextY = ball.y + ball.dy;
    }

    ball.y = nextY;
    return 0;
}

/* CPU is a little slower than the ball: it skips every third step */
void moveCpu(int step)
{
    if (step % 3 == 2)
    {
        return;
    }
    int center = cpuX + PADDLE_W / 2;
    if (ball.x < center)
    {
        cpuX--;
    }
    else if (ball.x > center)
    {
        cpuX++;
    }

    if (cpuX < 0)
    {
        cpuX = 0;
    }
    if (cpuX > FIELD_W - PADDLE_W)
    {
        cpuX = FIELD_W - PADDLE_W;
    }
}

void movePlayer(int dir)
{
    playerX += dir * 2;
    if (playerX < 0)
    {
        playerX = 0;
    }
    if (playerX > FIELD_W - PADDLE_W)
    {
        playerX = FIELD_W - PADDLE_W;
    }
}

/* draw the whole frame with one fputs so it does not flicker */
void render(void)
{
    char buf[4096];
    int len = 0;

    // put cursor back to top-left instead of clearing the screen
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD origin = {0, 0};
    SetConsoleCursorPosition(h, origin);

    for (int y = 0; y < FIELD_H; y++)
    {
        len += sprintf(buf + len, "<!");
        for (int x = 0; x < FIELD_W; x++)
        {
            const char *cell = " .";
            if (x == ball.x && y == ball.y)
            {
                cell = "()";
            }
            else if (y == 0 && paddleCovers(cpuX, x))
            {
                cell = "[]";
            }
            else if (y == FIELD_H - 1 && paddleCovers(playerX, x))
            {
                cell = "[]";
            }
            len += sprintf(buf + len, "%s", cell);
        }
        len += sprintf(buf + len, "!>");

        // side panel
        switch (y)
        {
        case 1:
            len += sprintf(buf + len, "   CPU:    %-8d", cpuScore);
            break;
        case 2:
            len += sprintf(buf + len, "   PLAYER: %-8d", playerScore);
            break;
        case 3:
            len += sprintf(buf + len, "   FIRST TO %-3d    ", WIN_SCORE);
            break;
        case 5:
            len += sprintf(buf + len, "   RALLY:  %-8d", rally);
            break;
        case 8:
            len += sprintf(buf + len, "   %-24s", message);
            break;
        case 12:
            len += sprintf(buf + len, "   A/D or Left/Right: move");
            break;
        case 13:
            len += sprintf(buf + len, "   Enter/Space: serve      ");
            break;
        case 14:
            len += sprintf(buf + len, "   Q: quit                 ");
            break;
        default:
            len += sprintf(buf + len, "                           ");
            break;
        }
        len += sprintf(buf + len, "\n");
    }
    len += sprintf(buf + len, "<!========================================!>\n");
    len += sprintf(buf + len, "  \\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\n");

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

    serve(1);
    render();

    int quit = 0;
    int step = 0;
    DWORD lastStep = GetTickCount();

    while (!quit && playerScore < WIN_SCORE && cpuScore < WIN_SCORE)
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
            switch (key)
            {
            case 'a':
            case 'A':
                movePlayer(-1);
                dirty = 1;
                break;
            case 'd':
            case 'D':
                movePlayer(1);
                dirty = 1;
                break;
            case ' ':
            case '\r':
                if (waiting)
                {
                    waiting = 0;
                    message = "";
                    lastStep = GetTickCount();
                    dirty = 1;
                }
                break;
            case 'q':
            case 'Q':
                quit = 1;
                break;
            }
        }

        /* ---- ball and CPU ---- */
        int interval = START_DELAY_MS - rally * 3;
        if (interval < MIN_DELAY_MS)
        {
            interval = MIN_DELAY_MS;
        }
        if (!waiting && GetTickCount() - lastStep >= (DWORD)interval)
        {
            lastStep = GetTickCount();
            step++;
            moveCpu(step);
            int result = moveBall();
            dirty = 1;

            if (result != 0)
            {
                if (result == 1)
                {
                    playerScore++;
                    message = "You scored! ENTER/SPACE";
                }
                else
                {
                    cpuScore++;
                    message = "CPU scored! ENTER/SPACE";
                }
                serve(result == -1); // serve toward the side that just lost the point
                waiting = 1; // wait for Enter/Space before the next serve
            }
        }

        if (dirty)
        {
            render();
        }

        Sleep(10);
    }

    if (quit)
    {
        printf("\n  Bye! Player %d - %d CPU\n", playerScore, cpuScore);
        return 0;
    }

    printf("\n  %s  Final score: Player %d - %d CPU\n",
           playerScore > cpuScore ? "YOU WIN!" : "GAME OVER!", playerScore, cpuScore);
    printf("\n  Press any key to exit...");
    getch();
    return 0;
}
