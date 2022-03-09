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

void draw_player(int x, bool colliding)
{
    if (colliding)
    {
        printf("\x1B[1;31m");
    }
    printxy("O-V-O", x, 3);
    printxy(" O-O ", x, 4);
    printxy("  V  ", x, 5);
    if (colliding)
    {
        printf("\x1B[0m");
    }
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

    // HACK: Set buffer size a second time to fix aligment issues
    SetConsoleScreenBufferSize(hStdOutput, { nScrWidth, nScrHeight });


    int nHealth = 1000;
    float fSpeed = 10.0f;
    float fAccel = 1.0f;
    float fDistance = 0.0f;
    float fPosition = nScrWidth / 2;

    for (int y = 0; y < nScrHeight; y++) {
        for (int x = 0; x < nScrWidth; x++) {
            sbxy(' ', x, y);
        }
    }

    int nMaxRow = nScrHeight - 3;
    int nCaveCol = nScrWidth / 2;
    int nGapWidth = 15;

    auto tp1 = chrono::system_clock::now();
    auto tp2 = tp1;

    while (nHealth >= 0) {
        // Timing
        this_thread::sleep_for(10ms);
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsed = tp2 - tp1;
        tp1 = tp2;

        // Handle player movement
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            fPosition -= 40.0f * elapsed.count();
        }

        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            fPosition += 40.0f * elapsed.count();
        }

        bool colliding = false;

        // Collision left
        if (sb[(int)fPosition] == '#') {
            nHealth -= 3;
            fPosition += 1;
            colliding = true;
        }
        else if (sb[(int)fPosition] == '.') {
            nHealth -= 10;
            fPosition += 2;
            colliding = true;
        }

        // Collision right
        if (sb[(int)fPosition + 4] == '#') {
            nHealth -= 3;
            fPosition -= 1;
            colliding = true;
        }
        else if (sb[(int)fPosition + 4] == '.') {
            nHealth -= 10;
            fPosition -= 2;
            colliding = true;
        }

        fSpeed += elapsed.count() * fAccel;
        fDistance += elapsed.count() * fSpeed;

        // Position to draw cave
        nCaveCol = 1 + nScrWidth / 2 + (nScrWidth / 2 - nGapWidth - 2) * sin(fDistance * 0.01) + 1.0 * sin(fDistance * 0.2);

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

        draw_player((int)fPosition, colliding);

        printxy("\x1B[1;36m< Cave Diver >\x1B[0m - ", 1, nScrHeight - 1);
        printf("Health: \x1B[1;31m%4d\x1B[0m - Distance travelled: \x1B[1;33m%.2f\x1B[0m      ", nHealth, fDistance);
    }

    printxy("\x1B[1;31mGame Over!\x1B[0m ", 1, nScrHeight);
    printf("Distance travelled: \x1B[1;32m%f.2\x1B[0m\n", fDistance);

    printf("Press ENTER to quit!");
    while (true) {
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
            break;
        }
    }

    return 0;
}