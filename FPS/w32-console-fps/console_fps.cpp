#include "windows.h"
#include <string>
#include <chrono> 
using namespace std;

int main()
{
    const int nScreenWidth = 120;
    const int nScreenHeight = 40;
    const float PI = 3.14159f;
    const float PI_2 = PI / 2.0f;

    char *screen = new char[nScreenWidth * nScreenHeight];

    const int nMapWidth = 16;
    const int nMapHeight = 16;

    string map;
    map += "################";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#.......###....#";
    map += "#.........#....#";
    map += "#.........#....#";
    map += "#...............";
    map += "#...............";
    map += "#..............#";
    map += "#..............#";
    map += "#..#...........#";
    map += "#..#...........#";
    map += "#..#..#........#";
    map += "#..#..#..#.....#";
    map += "################";

    float fPlayerX = 8.0f;
    float fPlayerY = 8.0f;
    float fPlayerA = 0.0f;
    float fFOV = PI / 4.0f;

    // Set console window size
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT windowSize = { 0, 0, nScreenWidth - 1, nScreenHeight - 1 };
    SetConsoleWindowInfo(hStdOut, TRUE, &windowSize);

    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    // Hide cursor
    CONSOLE_CURSOR_INFO CursorInfo;
    CursorInfo.bVisible = false;
    CursorInfo.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &CursorInfo);

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    while (1) {

        // Handle timing
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsed = tp2 - tp1;
        tp1 = tp2;

        float delta = elapsed.count();

        float fTurnSpeed = 0.7f;
        float fPlayerSpeed = 2.0f;

        // Player rotation
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            fPlayerA -= fTurnSpeed * delta;
        }

        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            fPlayerA += fTurnSpeed * delta;
        }

        // Player movement
        if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
            fPlayerX += fPlayerSpeed * sinf(fPlayerA) * delta;
            fPlayerY += fPlayerSpeed * cosf(fPlayerA) * delta;
        }

        if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
            fPlayerX -= fPlayerSpeed * sinf(fPlayerA) * delta;
            fPlayerY -= fPlayerSpeed * cosf(fPlayerA) * delta;
        }

        if (GetAsyncKeyState('A') & 0x8000) {
            fPlayerX += fPlayerSpeed * sinf(fPlayerA - PI_2) * delta;
            fPlayerY += fPlayerSpeed * cosf(fPlayerA - PI_2) * delta;
        }

        if (GetAsyncKeyState('D') & 0x8000) {
            fPlayerX += fPlayerSpeed * sinf(fPlayerA + PI_2) * delta;
            fPlayerY += fPlayerSpeed * cosf(fPlayerA + PI_2) * delta;
        }

        for (int x = 0; x < nScreenWidth; x++) {

            // Raytracing

            float fRayAngle = (fPlayerA - fFOV / 2.0f) + (float)x / (float)nScreenWidth * fFOV;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);
            float fDistanceToWall = 0.0f;
            float fDepth = 16.0f;
            bool bHitWall = false;

            while (!bHitWall && fDistanceToWall <= fDepth) {
                fDistanceToWall += 0.1f;
                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);
                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else if (map[nTestY * nMapWidth + nTestX] == '#') {
                    bHitWall = true;
                }
            }

            // Calculate wall height

            int nWallHeight = nScreenHeight / fDistanceToWall;
            int nCeiling = nScreenHeight / 2 - nWallHeight;
            int nFloor = nScreenHeight - nCeiling;

            char nShade = ' ';

            // Shade wall according to distance

            if (fDistanceToWall < 0.2f * fDepth) {
                nShade = 219;
            }
            else if (fDistanceToWall < 0.4f * fDepth) {
                nShade = 178;
            }
            else if (fDistanceToWall < 0.8f * fDepth) {
                nShade = 177;
            }
            else if (fDistanceToWall < 1.00f * fDepth) {
                nShade = 176;
            }
            else {
                nShade = ' ';
            }

            // Draw wall column

            for (int y = 0; y < nScreenHeight; y++) {
                int pos = y * nScreenWidth + x;
                if (y < nCeiling) {
                    screen[pos] = ' ';
                }
                else if (y <= nFloor) {
                    screen[pos] = nShade;
                }
                else {
                    screen[pos] = '.';
                }
            }
        }

        DWORD dwWritten = 0;
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth*nScreenHeight, { 0, 0 }, &dwWritten);
    }

    return 0;
}