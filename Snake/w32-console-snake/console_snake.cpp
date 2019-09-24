#include "windows.h"
#include <stdio.h>
#include <list>
#include <thread>

using namespace std;

const int nScreenWidth = 80;
const int nScreenHeight = 40;

struct snake_segment {
    int x, y;
};

enum {
    SNAKE_UP,
    SNAKE_RIGHT,
    SNAKE_DOWN,
    SNAKE_LEFT
};

int main()
{
    char screen[nScreenWidth * nScreenHeight];

    // Set console window size
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT windowSize = { 0, 0, nScreenWidth - 1, nScreenHeight - 1 };
    SetConsoleScreenBufferSize(hStdOut, { nScreenWidth, nScreenHeight });
    SetConsoleWindowInfo(hStdOut, TRUE, &windowSize);

    DWORD dwWritten;
    bool bGameOver = false;

    list<snake_segment> snake;

    int nSnakeX = nScreenWidth / 2;
    int nSnakeY = nScreenHeight;
    int nFoodX = nSnakeX;
    int nFoodY = nScreenHeight / 2;
    int nSnakeDir = 0;

    for (int i = 0; i < 5; i++) {
        snake.push_front({ nSnakeX, nSnakeY});
    }

    while (!bGameOver) {

        // Timing
        this_thread::sleep_for(100ms);

        // Input
        if (nSnakeDir != SNAKE_DOWN && GetAsyncKeyState(VK_UP) & 0x8000) {
            nSnakeDir = SNAKE_UP;
        }
        if (nSnakeDir != SNAKE_UP && GetAsyncKeyState(VK_DOWN) & 0x8000) {
            nSnakeDir = SNAKE_DOWN;
        }
        if (nSnakeDir != SNAKE_LEFT && GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            nSnakeDir = SNAKE_RIGHT;
        }
        if (nSnakeDir != SNAKE_RIGHT && GetAsyncKeyState(VK_LEFT) & 0x8000) {
            nSnakeDir = SNAKE_LEFT;
        }

        // Game logic
        switch (nSnakeDir) {
        case SNAKE_UP:
            nSnakeY--;
            break;
        case SNAKE_DOWN:
            nSnakeY++;
            break;
        case SNAKE_LEFT:
            nSnakeX--;
            break;
        case SNAKE_RIGHT:
            nSnakeX++;
            break;
        }

        // TEMP: Wrap coords for now
        if (nSnakeY < 0) nSnakeY = nScreenHeight * 2 - 1;
        if (nSnakeY >= nScreenHeight * 2) nSnakeY = 0;
        if (nSnakeX < 0) nSnakeX = nScreenWidth - 1;
        if (nSnakeX >= nScreenWidth) nSnakeX = 0;

        // Check food collision
        if (nSnakeX == nFoodX && nSnakeY == nFoodY) {
            int nTailX = snake.back().x;
            int nTailY = snake.back().y;
            // grow snake tail
            for (int i = 0; i < 5; i++) {
                snake.push_back({ nTailX, nTailY });
            }
            while (true) {
                // place new food
                nFoodX = rand() % nScreenWidth;
                nFoodY = rand() % (nScreenHeight * 2);
                // check if place is empty
                if (screen[nFoodY/2 * nScreenWidth + nFoodX] == ' ') {
                    break;
                }
            }
        }

        // Rotate snake segments
        snake.push_front({ nSnakeX, nSnakeY });
        snake.pop_back();

        // Clear buffer
        memset(screen, ' ', sizeof(screen));

        // Draw food
        for (auto s : snake) {
            int p = nFoodY / 2 * nScreenWidth + nFoodX;
            screen[p] = (nFoodY % 2 == 0) ? '\xdf' : '\xdc';
        }

        // Draw snake
        for (auto s : snake) {
            int p = s.y / 2 * nScreenWidth + s.x;
            if (screen[p] == ' ' && s.y % 2 == 0) {
                screen[p] = '\xdf';
            }
            else if (screen[p] == ' ' && s.y % 2 == 1) {
                screen[p] = '\xdc';
            }
            else if (screen[p] == '\xdf' && s.y % 2 == 1) {
                screen[p] = '\xdb';
            }
            else if (screen[p] == '\xdc' && s.y % 2 == 0) {
                screen[p] = '\xdb';
            }
        }

        WriteConsoleOutputCharacter(hStdOut, screen, nScreenWidth*nScreenHeight, { 0, 0 }, &dwWritten);
    }

    return 0;
}
