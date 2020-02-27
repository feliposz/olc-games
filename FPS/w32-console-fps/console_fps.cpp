#include "olcConsoleGameEngine.h"
#include <string>
using namespace std;

const float PI = 3.14159f;
const float PI_2 = PI / 2.0f;

class FPSGame : public olcConsoleGameEngine {
private:
    string map;
    const int nMapWidth = 32;
    const int nMapHeight = 32;
    float fPlayerX = 8.0f;
    float fPlayerY = 8.0f;
    float fPlayerA = 0.0f;
    float fFOV = PI / 4.0f;

public:

    virtual bool OnUserCreate() override
    {
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
        map += "#..####..############......#####";
        map += "#..............#...............#";
        map += "#..............#...............#";
        map += "#..............##..............#";
        map += "#..............................#";
        map += "#.......###....##......##......#";
        map += "#.........#....#...............#";
        map += "#.........#....##..............#";
        map += "#..............................#";
        map += "#..............................#";
        map += "#..............#####...........#";
        map += "#..............#...............#";
        map += "#..#...........#..............##";
        map += "#..#...........#.............###";
        map += "#..#..#........#............####";
        map += "#..#..#..#.....#...........#####";
        map += "################################";

        return true;
    }
    virtual bool OnUserUpdate(float fElapsedTime) override
    {

        float fTurnSpeed = 2.0f;
        float fPlayerSpeed = 4.0f;

        // Player rotation
        if (GetKey(VK_LEFT).bHeld) {
            fPlayerA -= fTurnSpeed * fElapsedTime;
        }

        if (GetKey(VK_RIGHT).bHeld) {
            fPlayerA += fTurnSpeed * fElapsedTime;
        }

        float ffElapsedTimeX = 0.0f;
        float ffElapsedTimeY = 0.0f;

        // Player movement
        if (GetKey(VK_UP).bHeld || GetKey('W').bHeld) {
            ffElapsedTimeX += fPlayerSpeed * sinf(fPlayerA) * fElapsedTime;
            ffElapsedTimeY += fPlayerSpeed * cosf(fPlayerA) * fElapsedTime;
        }

        if (GetKey(VK_DOWN).bHeld || GetKey('S').bHeld) {
            ffElapsedTimeX -= fPlayerSpeed * sinf(fPlayerA) * fElapsedTime;
            ffElapsedTimeY -= fPlayerSpeed * cosf(fPlayerA) * fElapsedTime;
        }

        if (GetKey('A').bHeld) {
            ffElapsedTimeX += fPlayerSpeed * sinf(fPlayerA - PI_2) * fElapsedTime;
            ffElapsedTimeY += fPlayerSpeed * cosf(fPlayerA - PI_2) * fElapsedTime;
        }

        if (GetKey('D').bHeld) {
            ffElapsedTimeX += fPlayerSpeed * sinf(fPlayerA + PI_2) * fElapsedTime;
            ffElapsedTimeY += fPlayerSpeed * cosf(fPlayerA + PI_2) * fElapsedTime;
        }

        // Test for wall collision
        int nTestPlayerX = (int)(fPlayerX + ffElapsedTimeX);
        int nTestPlayerY = (int)(fPlayerY + ffElapsedTimeY);

        if (map[nTestPlayerY * nMapWidth + nTestPlayerX] != '#') {
            fPlayerX += ffElapsedTimeX;
            fPlayerY += ffElapsedTimeY;
        }

        for (int x = 0; x < ScreenWidth(); x++) {

            // Raytracing

            float fRayAngle = (fPlayerA - fFOV / 2.0f) + (float)x / (float)ScreenWidth() * fFOV;

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

            int nWallHeight = ScreenHeight() / fDistanceToWall;
            int nCeiling = ScreenHeight() / 2 - nWallHeight;
            int nFloor = ScreenHeight() - nCeiling;

            wchar_t cShade = ' ';

            // Shade wall according to distance

            if (fDistanceToWall < 0.2f * fDepth) {
                cShade = PIXEL_SOLID;
            }
            else if (fDistanceToWall < 0.4f * fDepth) {
                cShade = PIXEL_THREEQUARTERS;
            }
            else if (fDistanceToWall < 0.8f * fDepth) {
                cShade = PIXEL_HALF;
            }
            else if (fDistanceToWall < 1.00f * fDepth) {
                cShade = PIXEL_QUARTER;
            }
            else {
                cShade = L' ';
            }

            // Draw wall column

            for (int y = 0; y < ScreenHeight(); y++) {
                float fFloorDist = 1.0f - ((float)y - ScreenHeight() / 2.0f) / (ScreenHeight() / 2.0f);
                wchar_t cFloorShade = L' ';
                if (fFloorDist < 0.2f) {
                    cFloorShade = L'#';
                }
                else if (fFloorDist < 0.4f) {
                    cFloorShade = L'x';
                }
                else if (fFloorDist < 0.6f) {
                    cFloorShade = L':';
                }
                else if (fFloorDist < 0.8f) {
                    cFloorShade = L'-';
                }
                else {
                    cFloorShade = L'.';
                }

                if (y < nCeiling) {
                    Draw(x, y, L' ');
                }
                else if (y <= nFloor) {
                    Draw(x, y, cShade);
                }
                else {
                    Draw(x, y, cFloorShade);
                }
            }
        }

        for (int x = 0; x < nMapWidth; x++) {
            for (int y = 0; y < nMapHeight; y++) {
                wchar_t c = L' ';
                // Y is reversed when displayed
                if (x == (int)fPlayerX && y == (int)fPlayerY) {
                    float fDirX = sinf(fPlayerA);
                    float fDirY = cosf(fPlayerA);
                    if (fabs(fDirX) > fabs(fDirY)) {
                        if (fDirX < 0) {
                            c = L'<';
                        }
                        else {
                            c = L'>';
                        }
                    }
                    else {
                        if (fDirY < 0) {
                            c = L'v';
                        }
                        else {
                            c = L'^';
                        }
                    }
                }
                else {
                    c = map[y * nMapWidth + x];
                }
                Draw(x, (nMapHeight - y - 1), c);
            }
        }

        wstring display;
        display += L"X=" + to_wstring(fPlayerX);
        display += L", Y = " + to_wstring(fPlayerY);
        display += L", A = " + to_wstring(fPlayerA);
        
        DrawString(50, 0, display);

        return true;
    }
};

int main()
{
    FPSGame game;
    game.ConstructConsole(160, 90, 8, 8);
    game.Start();

    return 0;
}