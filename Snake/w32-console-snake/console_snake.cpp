#include "windows.h"
#include <stdio.h>

const int nScreenWidth = 80;
const int nScreenHeight = 40;

int main()
{
    // Set console window size
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT windowSize = { 0, 0, nScreenWidth - 1, nScreenHeight - 1 };
    SetConsoleScreenBufferSize(hStdOut, { nScreenWidth, nScreenHeight });
    SetConsoleWindowInfo(hStdOut, TRUE, &windowSize);

    printf("Snake\n");

    return 0;
}
