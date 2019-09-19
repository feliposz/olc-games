#include "windows.h"
#include <string>
#include <stdio.h> 
using namespace std;


int main()
{
    const int nScreenWidth = 120;
    const int nScreenHeight = 40;

    char *screen = new char[nScreenWidth * nScreenHeight];

    const int nMapWidth = 16;
    const int nMapHeight = 16;

    string map;
    map += "################";
    map += "#.....#........#";
    map += "#..............#";
    map += "#.........#....#";
    map += "#..............#";
    map += "#..#...........#";
    map += "#..###......#..#";
    map += "#..#........#..#";
    map += "#..#...........#";
    map += "#..............#";
    map += "#.........##...#";
    map += "#.........##...#";
    map += "#.........##...#";
    map += "#...#..........#";
    map += "#...#..........#";
    map += "################";

    float fPlayerX = 8.0f;
    float fPlayerY = 8.0f;
    float fPlayerA = 0.0f;
    float fFOV = 3.14159f / 4.0f;

    for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
        screen[i] = L'.';
    }

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

    while (1) {

        fPlayerA += 0.001f;

        for (int x = 0; x < nScreenWidth; x++) {

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

            int nWallHeight = nScreenHeight / fDistanceToWall;
            int nCeiling = nScreenHeight / 2 - nWallHeight;
            int nFloor = nScreenHeight - nCeiling;

            char nShade = ' ';

            if (fDistanceToWall < 0.25f * fDepth) {
                nShade = 219;
            }
            else if (fDistanceToWall < 0.5f * fDepth) {
                nShade = 178;
            }
            else if (fDistanceToWall < 0.75f * fDepth) {
                nShade = 177;
            }
            else {
                nShade = 176;
            }

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