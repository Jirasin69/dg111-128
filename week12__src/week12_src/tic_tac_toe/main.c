///
/// https://github.com/R3DHULK/C-For-Gamers/blob/main/tic-tac-toe.c
///

#include <stdio.h>
#include <conio.h>

#include <stdlib.h>

#define BOARD_SIZE 3

// ตำแหน่งทั้ง 8 เส้นที่ชนะได้ (แถว 3 + คอลัมน์ 3 + แนวทแยง 2) เก็บเป็น {row, col} ทีละช่อง
static const int LINES[8][3][2] = {
    {{0, 0}, {0, 1}, {0, 2}}, {{1, 0}, {1, 1}, {1, 2}}, {{2, 0}, {2, 1}, {2, 2}}, // rows
    {{0, 0}, {1, 0}, {2, 0}}, {{0, 1}, {1, 1}, {2, 1}}, {{0, 2}, {1, 2}, {2, 2}}, // cols
    {{0, 0}, {1, 1}, {2, 2}}, {{0, 2}, {1, 1}, {2, 0}},                          // diagonals
};

void init_board(char board[BOARD_SIZE][BOARD_SIZE])
{
    char *cell = &board[0][0]; // ชื่อ array 2 มิติ = pointer ไปยังช่องแรก
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
        cell[i] = ' ';
}

void print_board(const char board[BOARD_SIZE][BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            printf(" %c ", board[i][j]);
            if (j != BOARD_SIZE - 1)
                printf("|");
        }
        printf("\n");
        if (i != BOARD_SIZE - 1)
            printf("---+---+---\n");
    }
}

// ตรวจ 1 เส้น: รับ pointer ไปยัง 3 ช่องบน board แล้วเทียบกับ symbol
int line_wins(const char *a, const char *b, const char *c, char symbol)
{
    return *a == symbol && *b == symbol && *c == symbol;
}

int check_win(const char board[BOARD_SIZE][BOARD_SIZE], char symbol)
{
    for (int i = 0; i < 8; i++)
    {
        const int(*line)[2] = LINES[i];
        if (line_wins(&board[line[0][0]][line[0][1]],
                       &board[line[1][0]][line[1][1]],
                       &board[line[2][0]][line[2][1]],
                       symbol))
            return 1;
    }
    return 0;
}

// รับตำแหน่ง (1-9) จากผู้เล่น แล้วส่ง row/col ออกทาง pointer จนกว่าจะได้ช่องที่ว่างจริง
void get_move(char board[BOARD_SIZE][BOARD_SIZE], int *row, int *col)
{
    int pos = 0;
    char line[32];
    do
    {
        printf("Enter a position (1-9), or Q to quit: ");
        // อ่านทั้งบรรทัด — พิมพ์ Q เพื่อออกจากเกมทันที
        if (fgets(line, sizeof(line), stdin) == NULL || line[0] == 'q' || line[0] == 'Q')
        {
            printf("Thanks for playing!\n");
            exit(0);
        }
        pos = 0;
        sscanf(line, "%d", &pos);
        // เลขช่องเรียงแบบ numpad: แถวบนสุดคือ 7 8 9 และแถวล่างสุดคือ 1 2 3
        *row = BOARD_SIZE - 1 - (pos - 1) / BOARD_SIZE;
        *col = (pos - 1) % BOARD_SIZE;
    } while (pos < 1 || pos > BOARD_SIZE * BOARD_SIZE || board[*row][*col] != ' ');
}

// เคลียหน้าจอ แล้ววาดหัวข้อ + คู่มือเลขช่อง + กระดานปัจจุบัน
void show_screen(const char board[BOARD_SIZE][BOARD_SIZE])
{
    system("cls");

    printf("Welcome to Tic-Tac-Toe!\n");
    printf("Player 1 uses 'X' and Player 2 uses 'O'.\n");
    printf("The board is numbered like a numpad:\n");
    printf(" 7 | 8 | 9 \n");
    printf("---+---+---\n");
    printf(" 4 | 5 | 6 \n");
    printf("---+---+---\n");
    printf(" 1 | 2 | 3 \n\n");

    printf("Current board:\n");
    print_board(board);
    printf("\n");
}

int main()
{
    char board[BOARD_SIZE][BOARD_SIZE];
    init_board(board);

    int player = 1;
    int moves = 0;
    while (1)
    {
        char symbol = (player == 1) ? 'X' : 'O';
        show_screen(board);
        printf("Player %d's turn:\n", player);

        int row, col;
        get_move(board, &row, &col);
        board[row][col] = symbol;
        moves++;

        if (check_win(board, symbol))
        {
            show_screen(board);
            printf("Player %d wins!\n", player);
            break;
        }

        if (moves == BOARD_SIZE * BOARD_SIZE)
        {
            show_screen(board);
            printf("Tie game!\n");
            break;
        }

        player = (player == 1) ? 2 : 1;
    }

	// Wait for user input before closing the console
	printf("\nPress any key to exit...");
	getch();

	return 0;
}
