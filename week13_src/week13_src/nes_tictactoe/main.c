/*
 * main.c — NES-style Tic-Tac-Toe (กระดาน N×N, เลือกจำนวนที่ต้องเรียงเพื่อชนะ) ด้วย Raylib
 *
 * ปรับแนวคิดมาจากเกม "ttt" ของ hiimsergey (GPL-3.0, https://github.com/hiimsergey/ttt)
 * ให้เป็นแนวทางเดียวกับตัวอย่างใน 06-nes-raylib:
 *   - ไฟล์ .c ไฟล์เดียว (ต้นฉบับแยก 5 ไฟล์ .c/.h และใช้ Makefile + git submodule ของ raylib)
 *   - ใช้ static array แทน MemAlloc (ตามแนวทางของวิชา: ห้าม malloc ตอนเล่น)
 *   - หน้าจอ NES-style 320×240 (4:3) ขยาย ×2.5 → 800×600, สีจาก NES palette, FSM: TITLE → PLAYING → GAME_OVER
 *   - เล่นได้ทั้งเมาส์และปุ่ม (pad_poll: ลูกศรเลื่อน cursor, Enter/Space/Z วางหมาก)
 *   - ตรวจชนะด้วยการนับหมากต่อเนื่องใน 4 ทิศ (โค้ดนี้เขียนใหม่ ไม่ได้คัดลอกจากต้นฉบับ)
 *
 * dotnes เทียบ:
 *   pad_poll(0) / PAD_UP.. / PAD_A → IsKeyPressed(KEY_UP..) / KEY_ENTER
 *   nametable (กระดาน)              → board[row][col]
 *   ppu_wait_nmi()                  → SetTargetFPS(60)
 *
 * ปุ่ม: หน้าเลือกค่า ↑↓ = ขนาดกระดาน, ←→ = จำนวนที่ต้องเรียง, Enter = เริ่ม
 *       ตอนเล่น ลูกศร/WASD = เลื่อน cursor, Enter/Space/Z หรือคลิกเมาส์ = วางหมาก, Q = กลับหน้าเลือกค่า
 *
 * Compile:
 *   gcc -std=c99 main.c -o tictactoe -lraylib -lm -lwinmm -lgdi32
 */

#include "raylib.h"

#define NES_W     320
#define NES_H     240
#define SCALE     2.5f
#define WIN_W     ((int)(NES_W * SCALE))
#define WIN_H     ((int)(NES_H * SCALE))

#define MIN_N     3           /* กระดานเล็กสุด 3×3 */
#define MAX_N     8           /* กระดานใหญ่สุด 8×8 */
#define BOARD_PX  160         /* พื้นที่กระดานสูงสุด (px ก่อนขยาย) เหลือที่ให้ข้อความใต้กระดาน */
#define BOARD_Y   40          /* ขอบบนของกระดาน */

/* NES Palette — สีที่เลือกใช้ */
#define C_BG      CLITERAL(Color){ 0,   30,  116, 255 }  /* 0x01 dark blue */
#define C_WHITE   CLITERAL(Color){ 236, 238, 236, 255 }  /* 0x30 */
#define C_RED     CLITERAL(Color){ 228, 92,  16,  255 }  /* 0x17 */
#define C_BLUE    CLITERAL(Color){ 76,  154, 236, 255 }  /* 0x21 */
#define C_YELLOW  CLITERAL(Color){ 212, 136, 32,  255 }  /* 0x27 */
#define C_GRAY    CLITERAL(Color){ 84,  84,  84,  255 }  /* 0x00 */

/* --- FSM: GameState (pattern เดียวกับ 03_gameloop) --- */
typedef enum {
    STATE_TITLE,        /* เลือกขนาดกระดานและจำนวนที่ต้องเรียง */
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

typedef enum { EMPTY, MARK_X, MARK_O } Mark;

/* --- ข้อมูลเกม (static array, ไม่ใช้ malloc) --- */
static Mark board[MAX_N][MAX_N];
static int  n      = 3;        /* กระดาน n×n */
static int  streak = 3;        /* ต้องเรียงกี่ช่องถึงชนะ */

static int  winCells[MAX_N * MAX_N][2];   /* ช่องที่ทำให้ชนะ (row, col) */
static int  winCount = 0;

static void ClearBoard(void) {
    for (int r = 0; r < MAX_N; r++)
        for (int c = 0; c < MAX_N; c++)
            board[r][c] = EMPTY;
    winCount = 0;
}

/* ================================================================
 * ตรวจชนะ: นับหมากชนิดเดียวกันที่ต่อเนื่องกันผ่านช่อง (r, c) ใน 4 ทิศ
 * (แนวนอน, แนวตั้ง, ทแยงลง, ทแยงขึ้น) ถ้าได้ >= streak ช่อง = ชนะ
 * ================================================================ */
static int CheckWin(int r, int c) {
    static const int DIRS[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };
    Mark m = board[r][c];

    for (int d = 0; d < 4; d++) {
        int dr = DIRS[d][0], dc = DIRS[d][1];
        int cells[MAX_N * MAX_N][2];
        int total = 0;

        cells[total][0] = r;  cells[total][1] = c;  total++;

        /* เดินไปข้างหน้าตามทิศ */
        for (int rr = r + dr, cc = c + dc;
             rr >= 0 && rr < n && cc >= 0 && cc < n && board[rr][cc] == m;
             rr += dr, cc += dc) {
            cells[total][0] = rr;  cells[total][1] = cc;  total++;
        }
        /* เดินย้อนกลับทางตรงข้าม */
        for (int rr = r - dr, cc = c - dc;
             rr >= 0 && rr < n && cc >= 0 && cc < n && board[rr][cc] == m;
             rr -= dr, cc -= dc) {
            cells[total][0] = rr;  cells[total][1] = cc;  total++;
        }

        if (total >= streak) {
            winCount = total;
            for (int i = 0; i < total; i++) {
                winCells[i][0] = cells[i][0];
                winCells[i][1] = cells[i][1];
            }
            return 1;
        }
    }
    return 0;
}

/* --- Helper: วาดข้อความบน tile grid (เทียบ vram_write) --- */
static void TileText(const char *text, int col, int row, Color c) {
    DrawText(text, col * 8 * SCALE, row * 8 * SCALE, 8 * SCALE, c);
}

/* วาดข้อความตรงกลางแนวนอน (row = แถว tile) */
static void TileTextCenter(const char *text, int row, Color c) {
    int w = MeasureText(text, 8 * SCALE);
    DrawText(text, (WIN_W - w) / 2, row * 8 * SCALE, 8 * SCALE, c);
}

/* --- วาดหมาก X / O ในช่อง (พิกัด NES px) --- */
static void DrawX(int x, int y, int cell) {
    int m = cell / 5;   /* ระยะเว้นขอบ */
    float t = (float)(cell / 8 + 1) * SCALE;
    DrawLineEx((Vector2){ (float)(x + m) * SCALE,        (float)(y + m) * SCALE },
               (Vector2){ (float)(x + cell - m) * SCALE, (float)(y + cell - m) * SCALE }, t, C_RED);
    DrawLineEx((Vector2){ (float)(x + cell - m) * SCALE, (float)(y + m) * SCALE },
               (Vector2){ (float)(x + m) * SCALE,        (float)(y + cell - m) * SCALE }, t, C_RED);
}

static void DrawO(int x, int y, int cell) {
    float outer = (float)(cell / 2 - cell / 5) * SCALE;
    float inner = outer - (float)(cell / 8 + 1) * SCALE;
    DrawRing((Vector2){ (float)(x + cell / 2) * SCALE, (float)(y + cell / 2) * SCALE },
             inner, outer, 0, 360, 32, C_BLUE);
}

int main(void) {
    InitWindow(WIN_W, WIN_H, "dotnes → Raylib: Tic-Tac-Toe");
    SetTargetFPS(60);

    GameState state = STATE_TITLE;
    Mark turn = MARK_X;
    Mark winner = EMPTY;        /* EMPTY + GAME_OVER = เสมอ */
    int  moves = 0;
    int  curR = 0, curC = 0;    /* cursor */
    int  xWins = 0, oWins = 0, draws = 0;
    int  frame = 0;

    ClearBoard();

    while (!WindowShouldClose()) {
        frame++;

        int cell = BOARD_PX / n;
        int ox = (NES_W - cell * n) / 2;   /* มุมซ้ายบนของกระดาน */
        int oy = BOARD_Y;

        /* ================================================================
         * STATE UPDATE
         * ================================================================ */
        switch (state) {

        case STATE_TITLE:
            if (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) n++;
            if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) n--;
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) streak++;
            if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) streak--;
            if (n < MIN_N) n = MIN_N;
            if (n > MAX_N) n = MAX_N;
            if (streak < MIN_N) streak = MIN_N;
            if (streak > n) streak = n;

            if (IsKeyPressed(KEY_ENTER)) {
                ClearBoard();
                turn = MARK_X;
                winner = EMPTY;
                moves = 0;
                curR = curC = n / 2;
                state = STATE_PLAYING;
            }
            break;

        case STATE_PLAYING: {
            /* pad_poll: เลื่อน cursor */
            if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) curC--;
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) curC++;
            if (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) curR--;
            if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) curR++;
            if (curC < 0) curC = 0;
            if (curC > n - 1) curC = n - 1;
            if (curR < 0) curR = 0;
            if (curR > n - 1) curR = n - 1;

            int place = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z);

            /* เมาส์: เลื่อน cursor ไปที่ช่องที่ชี้ และคลิกเพื่อวางหมาก */
            Vector2 mouse = GetMousePosition();
            int mx = (int)mouse.x / SCALE - ox;
            int my = (int)mouse.y / SCALE - oy;
            if (mx >= 0 && my >= 0 && mx < cell * n && my < cell * n) {
                int hoverC = mx / cell, hoverR = my / cell;
                if (GetMouseDelta().x != 0 || GetMouseDelta().y != 0) {
                    curC = hoverC;
                    curR = hoverR;
                }
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    curC = hoverC;
                    curR = hoverR;
                    place = 1;
                }
            }

            if (place && board[curR][curC] == EMPTY) {
                board[curR][curC] = turn;
                moves++;

                if (CheckWin(curR, curC)) {
                    winner = turn;
                    if (turn == MARK_X) xWins++; else oWins++;
                    state = STATE_GAME_OVER;
                } else if (moves == n * n) {
                    winner = EMPTY;
                    draws++;
                    state = STATE_GAME_OVER;
                } else {
                    turn = (turn == MARK_X) ? MARK_O : MARK_X;
                }
            }

            if (IsKeyPressed(KEY_Q)) state = STATE_TITLE;
        } break;

        case STATE_GAME_OVER:
            if (IsKeyPressed(KEY_ENTER)) {
                ClearBoard();
                turn = MARK_X;
                winner = EMPTY;
                moves = 0;
                curR = curC = n / 2;
                state = STATE_PLAYING;
            }
            if (IsKeyPressed(KEY_Q)) {
                ClearBoard();
                state = STATE_TITLE;
            }
            break;
        }

        /* ================================================================
         * DRAW
         * ================================================================ */
        BeginDrawing();
        ClearBackground(C_BG);

        if (state == STATE_TITLE) {
            TileTextCenter("TIC-TAC-TOE", 4, C_WHITE);

            TileTextCenter(TextFormat("BOARD  %dx%d", n, n), 11, C_YELLOW);
            TileTextCenter("UP / DOWN", 12, C_GRAY);

            TileTextCenter(TextFormat("STREAK TO WIN  %d", streak), 15, C_YELLOW);
            TileTextCenter("LEFT / RIGHT", 16, C_GRAY);

            if ((frame / 30) % 2 == 0) TileTextCenter("PRESS ENTER", 21, C_WHITE);
        } else {
            /* HUD */
            TileText(TextFormat("X:%d", xWins), 1, 1, C_RED);
            TileText(TextFormat("O:%d", oWins), 9, 1, C_BLUE);
            TileText(TextFormat("DRAW:%d", draws), 17, 1, C_WHITE);
            TileText(TextFormat("%d IN A ROW", streak), 1, 3, C_GRAY);

            /* เส้นตาราง */
            for (int i = 0; i <= n; i++) {
                DrawRectangle((ox + i * cell) * SCALE - 1, oy * SCALE, 2, cell * n * SCALE, C_WHITE);
                DrawRectangle(ox * SCALE, (oy + i * cell) * SCALE - 1, cell * n * SCALE, 2, C_WHITE);
            }

            /* ช่องที่ชนะ (กระพริบ) */
            if (state == STATE_GAME_OVER && winCount > 0 && (frame / 15) % 2 == 0) {
                for (int i = 0; i < winCount; i++) {
                    DrawRectangle((ox + winCells[i][1] * cell) * SCALE + 2,
                                  (oy + winCells[i][0] * cell) * SCALE + 2,
                                  cell * SCALE - 4, cell * SCALE - 4, C_GRAY);
                }
            }

            /* cursor */
            if (state == STATE_PLAYING) {
                DrawRectangleLines((ox + curC * cell) * SCALE + 3, (oy + curR * cell) * SCALE + 3,
                                   cell * SCALE - 6, cell * SCALE - 6, C_YELLOW);
            }

            /* หมาก */
            for (int r = 0; r < n; r++) {
                for (int c = 0; c < n; c++) {
                    if (board[r][c] == MARK_X) DrawX(ox + c * cell, oy + r * cell, cell);
                    else if (board[r][c] == MARK_O) DrawO(ox + c * cell, oy + r * cell, cell);
                }
            }

            /* ข้อความสถานะใต้กระดาน (แถว 29 บรรทัดสุดท้ายของจอ = แถว 29, ข้อความช่วยเหลืออยู่แถว 28
             * จึงต้องให้สถานะอยู่เหนือแถว 28 เสมอ ไม่งั้นตัวหนังสือจะทับกัน) */
            int statusRow = (oy + cell * n) / 8 + 1;
            if (state == STATE_PLAYING) {
                TileTextCenter(turn == MARK_X ? "TURN: X" : "TURN: O", statusRow,
                               turn == MARK_X ? C_RED : C_BLUE);
                TileTextCenter("ARROWS/MOUSE + ENTER   Q: MENU", 28, C_GRAY);
            } else {
                if (winner == MARK_X)      TileTextCenter("X WINS!", statusRow, C_RED);
                else if (winner == MARK_O) TileTextCenter("O WINS!", statusRow, C_BLUE);
                else                       TileTextCenter("DRAW", statusRow, C_WHITE);
                TileTextCenter("ENTER: AGAIN   Q: MENU", 28, C_YELLOW);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
