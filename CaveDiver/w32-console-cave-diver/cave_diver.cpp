#include "windows.h"
#include <chrono>
#include <stdio.h>
#include <thread>

using namespace std;

const int nWidth = 80;
const int nHeight = 30;

char sb[nWidth * nHeight];

void printxy(const char *text, int col, int row)
{
    // escape sequence to set cursor position
    printf("\x1B[%d;%dH%s", row, col, text);
}

void sbxy(const char chr, int col, int row)
{
    sb[col + row * nWidth] = chr;
}

void draw_player(int x)
{
    printxy("O-V-O", x, 3);
    printxy(" O-O ", x, 4);
    printxy("  V  ", x, 5);    
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

    int nHealth = 1000;
    int nDistance = 0;
    float fPosition = nWidth / 2;

    for (int y = 0; y < nHeight; y++) {
        for (int x = 0; x < nWidth; x++) {
            sbxy(' ', x, y);
        }
    }    

    while (nHealth >= 0) {
        this_thread::sleep_for(10ms);

        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            fPosition -= 1;
        }

        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            fPosition += 1;
        }

        printxy(sb, 1, 1);

        draw_player((int)fPosition);

        printxy("< Cave Diver > - ", 1, nHeight - 2);
        printf("Health: %4d - Distance travelled: %6d      ", nHealth, nDistance);
       
        // Test
        nHealth--;
        nDistance++;
    }

    printxy("Game Over! ", 1, nHeight);
    printf("Distance travelled: %d\n", nDistance);
    
    printf("Press ENTER to quit!");
    while (true) {
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
            break;
        }
    }

    return 0;
}