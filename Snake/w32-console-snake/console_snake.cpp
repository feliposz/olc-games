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

    bool bContinue = true;

    while (bContinue) {

        list<snake_segment> snake;

        bool bDead = false;
        int nSnakeX = nScreenWidth / 2;
        int nSnakeY = nScreenHeight;
        int nFoodX = nSnakeX;
        int nFoodY = nScreenHeight / 2;
        int nSnakeDir = 0;
        int nScore = 0;

        for (int i = 0; i < 5; i++) {
            snake.push_front({ nSnakeX, nSnakeY });
        }

        while (!bDead) {

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
                nScore++;
                // grow snake tail
                for (int i = 0; i < 5; i++) {
                    snake.push_back({ nTailX, nTailY });
                }
                while (true) {
                    // place new food
                    nFoodX = rand() % nScreenWidth;
                    nFoodY = rand() % (nScreenHeight * 2);
                    // check if place is empty
                    if (screen[nFoodY / 2 * nScreenWidth + nFoodX] == ' ') {
                        break;
                    }
                }
            }

            // Check tail collision
            for (auto it = snake.begin(); it != snake.end(); it++) {
                if (it != snake.begin() && it->x == nSnakeX && it->y == nSnakeY) {
                    bDead = true;
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
            if (!bDead) {
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
            }
            else {
                for (auto s : snake) {
                    int p = s.y / 2 * nScreenWidth + s.x;
                    screen[p] = '+';
                }
                int p = nSnakeY / 2 * nScreenWidth + nSnakeX;
                screen[p] = 'X';
            }

            if (bDead) {
                sprintf_s(screen + 20 * nScreenWidth + 20, 41, "Game Over! <SPACE> continues <ESC> quits");
            }
            sprintf_s(screen, 11, "Score:%4d", nScore);

            DWORD dwWritten;
            WriteConsoleOutputCharacter(hStdOut, screen, nScreenWidth*nScreenHeight, { 0, 0 }, &dwWritten);
        }
        
        while (true) {
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                bContinue = true;
                break;
            }
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                bContinue = false;
                break;
            }
        }

    }
    
    return 0;
}
