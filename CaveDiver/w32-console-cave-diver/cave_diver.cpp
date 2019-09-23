#include "windows.h"
#include <stdio.h>

const int nWidth = 80;
const int nHeight = 30;

void printxy(const char *text, int col, int row)
{
    // escape sequence to set cursor position
    printf("\x1B[%d;%dH%s", row, col, text);
}

int main()
{
    // Enable VT100 escape sequences
    DWORD dwMode;
    HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hStdOutput, &dwMode);
    SetConsoleMode(hStdOutput, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    GetConsoleMode(hStdInput, &dwMode);
    SetConsoleMode(hStdInput, dwMode | ENABLE_VIRTUAL_TERMINAL_INPUT);

    // Resize Console buffer/screen
    SetConsoleScreenBufferSize(hStdOutput, { nWidth, nHeight });
    SMALL_RECT rect = { 0, 0, nWidth - 1, nHeight - 1 };
    SetConsoleWindowInfo(hStdOutput, true, &rect);

    printxy("Cave Diver\n", nWidth / 2 - 5, nHeight / 2);

    system("pause");
    return 0;
}