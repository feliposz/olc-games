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

    const int nMapWidth = 32;
    const int nMapHeight = 16;

    string map;
    map += "################################";
    map += "#..............#...............#";
    map += "#..............##..............#";
    map += "#..............#...............#";
    map += "#.......###....##......##......#";
    map += "#.........#....#...............#";
    map += "#.........#....##..............#";
    map += "#..............................#";
    map += "#..............................#";
    map += "#..............##..............#";
    map += "#..............#...............#";
    map += "#..#...........##..............#";
    map += "#..#...........#.......##......#";
    map += "#..#..#........##..............#";
    map += "#..#..#..#.....#...............#";
    map += "################################";

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

        float fTurnSpeed = 2.0f;
        float fPlayerSpeed = 4.0f;

        // Player rotation
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            fPlayerA -= fTurnSpeed * delta;
        }

        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            fPlayerA += fTurnSpeed * delta;
        }

        float fDeltaX = 0.0f;
        float fDeltaY = 0.0f;

        // Player movement
        if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
            fDeltaX += fPlayerSpeed * sinf(fPlayerA) * delta;
            fDeltaY += fPlayerSpeed * cosf(fPlayerA) * delta;
        }

        if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
            fDeltaX -= fPlayerSpeed * sinf(fPlayerA) * delta;
            fDeltaY -= fPlayerSpeed * cosf(fPlayerA) * delta;
        }

        if (GetAsyncKeyState('A') & 0x8000) {
            fDeltaX += fPlayerSpeed * sinf(fPlayerA - PI_2) * delta;
            fDeltaY += fPlayerSpeed * cosf(fPlayerA - PI_2) * delta;
        }

        if (GetAsyncKeyState('D') & 0x8000) {
            fDeltaX += fPlayerSpeed * sinf(fPlayerA + PI_2) * delta;
            fDeltaY += fPlayerSpeed * cosf(fPlayerA + PI_2) * delta;
        }

        // Test for wall collision
        int nTestPlayerX = (int)(fPlayerX + fDeltaX);
        int nTestPlayerY = (int)(fPlayerY + fDeltaY);

        if (map[nTestPlayerY * nMapWidth + nTestPlayerX] != '#') {
            fPlayerX += fDeltaX;
            fPlayerY += fDeltaY;
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
                float fFloorDist = 1.0f - ((float)y - nScreenHeight / 2.0f) / (nScreenHeight / 2.0f);
                char nFloorShade = ' ';
                if (fFloorDist < 0.2f) {
                    nFloorShade = '#';
                }
                else if (fFloorDist < 0.4f) {
                    nFloorShade = 'x';
                }
                else if (fFloorDist < 0.6f) {
                    nFloorShade = ':';
                }
                else if (fFloorDist < 0.8f) {
                    nFloorShade = '-';
                }
                else {
                    nFloorShade = ' ';
                }

                if (y < nCeiling) {
                    screen[pos] = ' ';
                }
                else if (y <= nFloor) {
                    screen[pos] = nShade;
                }
                else {
                    screen[pos] = nFloorShade;
                }
            }
        }

        for (int x = 0; x < nMapWidth; x++) {
            for (int y = 0; y < nMapHeight; y++) {
                // Y is reversed when displayed
                int pos = (nMapHeight - y - 1) * nScreenWidth + x;
                if (x == (int)fPlayerX && y == (int)fPlayerY) {
                    float fDirX = sinf(fPlayerA);
                    float fDirY = cosf(fPlayerA);
                    if (fabs(fDirX) > fabs(fDirY)) {
                        if (fDirX < 0) {
                            screen[pos] = '<';
                        }
                        else {
                            screen[pos] = '>';
                        }
                    }
                    else {
                        if (fDirY < 0) {
                            screen[pos] = 'v';
                        }
                        else {
                            screen[pos] = '^';
                        }
                    }
                }
                else {
                    screen[pos] = map[y * nMapWidth + x];
                }
            }
        }

        DWORD dwWritten = 0;
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth*nScreenHeight, { 0, 0 }, &dwWritten);
    }

    return 0;
}