#include "windows.h"
#include <stdio.h>
#include <chrono>
#include <thread>

using namespace std;

const int nScrWidth = 80;
const int nScrHeight = 30;

char sb[nScrWidth * nScrHeight];

void printxy(const char *text, int col, int row)
{
    // escape sequence to set cursor position
    printf("\x1B[%d;%dH%s", row, col, text);
}

void sbxy(const char chr, int col, int row)
{
    sb[col + row * nScrWidth] = chr;
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
    SetConsoleScreenBufferSize(hStdOutput, { nScrWidth, nScrHeight });
    SMALL_RECT rect = { 0, 0, nScrWidth - 1, nScrHeight - 1 };
    SetConsoleWindowInfo(hStdOutput, true, &rect);

    int nHealth = 1000;
    int nDistance = 0;
    float fPosition = nScrWidth / 2;

    for (int y = 0; y < nScrHeight; y++) {
        for (int x = 0; x < nScrWidth; x++) {
            sbxy(' ', x, y);
        }
    }

    int nMaxRow = nScrHeight - 3;
    int nCaveCol = nScrWidth / 2;
    int nGapWidth = 15;

    while (nHealth >= 0) {
        this_thread::sleep_for(10ms);

        // Handle player movement
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            fPosition -= 1;
        }

        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            fPosition += 1;
        }

        // Position to draw cave
        nCaveCol = 1 + nScrWidth / 2 + (nScrWidth / 2 - nGapWidth - 2) * sin(nDistance * 0.01) + 1.0 * sin(nDistance * 0.2);

        // Scrool screen buffer
        memmove(sb, sb + nScrWidth, nScrWidth*nMaxRow);

        // Draw cave segment (row) to screen buffer
        for (int x = 0; x < nScrWidth; x++) {
            if (x < nCaveCol || x > nCaveCol + nGapWidth) {
                sbxy('.', x, nMaxRow);
            }
            else if (x == nCaveCol || x == nCaveCol + nGapWidth) {
                sbxy('#', x, nMaxRow);
            }
            else {
                sbxy(' ', x, nMaxRow);
            }
        }

        // Draw buffer to screen
        printxy(sb, 1, 1);

        draw_player((int)fPosition);

        printxy("< Cave Diver > - ", 1, nScrHeight - 1);
        printf("Health: %4d - Distance travelled: %6d      ", nHealth, nDistance);

        // Test
        nHealth--;
        nDistance++;
    }

    printxy("Game Over! ", 1, nScrHeight);
    printf("Distance travelled: %d\n", nDistance);

    printf("Press ENTER to quit!");
    while (true) {
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
            break;
        }
    }

    return 0;
}