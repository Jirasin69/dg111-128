# Sudoku with C

This program allows the user to play the game of **Sudoku**. It provides a command-line interface where the user can input their moves and see the current state of the Sudoku board. The program creates a **random** Sudoku board every time

### Difference between Linux and Windows version
| Linux | Windows |
| ----- | ------- |
| system("clear") | system("cls") |
| Unicode character | ASCII character |

> [main.c](main.c) in this folder is the **Windows** version (`system("cls")` + ASCII board).

### Compile & Run

```bash
gcc -std=c99 -Wall main.c -o sudoku
./sudoku
```

### Developed by
[![sr-tamim's Profilator](https://profilator.deno.dev/sr-tamim?v=1.0.0.alpha.4)](https://github.com/sr-tamim)
