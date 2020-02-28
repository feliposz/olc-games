#include "olcConsoleGameEngine.h"
#include <string>
#include <list>
using namespace std;

const float PI = 3.14159f;
const float PI_2 = PI / 2.0f;

struct Object {
    float x;
    float y;
    float vx;
    float vy;
    bool bRemove;
    olcSprite *sprite;
};

class FPSGame : public olcConsoleGameEngine {
private:
    string map;
    const int nMapWidth = 32;
    const int nMapHeight = 32;
    float fPlayerX = 8.0f;
    float fPlayerY = 8.0f;
    float fPlayerA = 0.0f;
    float fFOV = PI / 4.0f;
    float *fDepthBuffer;
    olcSprite sprWall;
    olcSprite sprLamp;
    olcSprite sprFireball;
    list<Object> lstObjects;

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

        sprWall.Load(L"Sprites/fps_wall1.spr");
        sprLamp.Load(L"Sprites/fps_lamp1.spr");
        sprFireball.Load(L"Sprites/fps_fireball1.spr");

        lstObjects.push_back({ 1.5f, 2.5f, 0, 0, false, &sprLamp });
        lstObjects.push_back({ 2.5f, 3.5f, 0, 0, false, &sprLamp });
        lstObjects.push_back({ 4.5f, 5.5f, 0, 0, false, &sprLamp });
        lstObjects.push_back({ 5.5f, 5.5f, 0, 0, false, &sprLamp });
        lstObjects.push_back({ 10.5f, 13.5f, 0, 0, false, &sprLamp });

        fDepthBuffer = new float[ScreenWidth()];

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

        while (fPlayerA < -PI) {
            fPlayerA += PI * 2.0f;
        }
        while (fPlayerA > PI) {
            fPlayerA -= PI * 2.0f;
        }

        float fDeltaX = 0.0f;
        float fDeltaY = 0.0f;

        // Player movement
        if (GetKey(VK_UP).bHeld || GetKey('W').bHeld) {
            fDeltaX += fPlayerSpeed * sinf(fPlayerA) * fElapsedTime;
            fDeltaY += fPlayerSpeed * cosf(fPlayerA) * fElapsedTime;
        }

        if (GetKey(VK_DOWN).bHeld || GetKey('S').bHeld) {
            fDeltaX -= fPlayerSpeed * sinf(fPlayerA) * fElapsedTime;
            fDeltaY -= fPlayerSpeed * cosf(fPlayerA) * fElapsedTime;
        }

        if (GetKey('A').bHeld) {
            fDeltaX += fPlayerSpeed * sinf(fPlayerA - PI_2) * fElapsedTime;
            fDeltaY += fPlayerSpeed * cosf(fPlayerA - PI_2) * fElapsedTime;
        }

        if (GetKey('D').bHeld) {
            fDeltaX += fPlayerSpeed * sinf(fPlayerA + PI_2) * fElapsedTime;
            fDeltaY += fPlayerSpeed * cosf(fPlayerA + PI_2) * fElapsedTime;
        }

        if (GetKey(VK_SPACE).bPressed) {
            float fNoise = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
            Object oFireball = {
                fPlayerX,
                fPlayerY,
                8.0f * sinf(fPlayerA + fNoise),
                8.0f * cosf(fPlayerA + fNoise),
                false,
                &sprFireball
            };
            lstObjects.push_back(oFireball);
        }


        // Test for wall collision
        int nTestPlayerX = (int)(fPlayerX + fDeltaX);
        int nTestPlayerY = (int)(fPlayerY + fDeltaY);

        if (map[nTestPlayerY * nMapWidth + nTestPlayerX] != '#') {
            fPlayerX += fDeltaX;
            fPlayerY += fDeltaY;
        }

        float fDepth = 16.0f;

        for (int x = 0; x < ScreenWidth(); x++) {

            // Raytracing

            float fRayAngle = (fPlayerA - fFOV / 2.0f) + (float)x / (float)ScreenWidth() * fFOV;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);
            float fDistanceToWall = 0.0f;
            bool bHitWall = false;

            float fSampleX = 0;
            float fSampleY = 0;

            while (!bHitWall && fDistanceToWall <= fDepth) {
                fDistanceToWall += 0.01f;
                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);
                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else if (map[nTestY * nMapWidth + nTestX] == '#') {
                    bHitWall = true;
                    // Use the angle between the mid point of the block and the point the ray hit the wall, to find out which wall was hit (N, S, E, W)
                    float fBlockMidX = nTestX + 0.5f;
                    float fBlockMidY = nTestY + 0.5f;
                    float fTestPointX = fPlayerX + fEyeX * fDistanceToWall;
                    float fTestPointY = fPlayerY + fEyeY * fDistanceToWall;
                    float fTestAngle = atan2((fTestPointX - fBlockMidX), (fTestPointY - fBlockMidY));
                    if (fTestAngle >= -PI * 0.25f && fTestAngle < PI * 0.25f) {
                        fSampleX = fTestPointX - (int)fTestPointX;
                    }
                    if (fTestAngle >= PI * 0.25f && fTestAngle < PI * 0.75f) {
                        fSampleX = fTestPointY - (int)fTestPointY;
                    }
                    if (fTestAngle >= -PI * 0.75f && fTestAngle < -PI * 0.25f) {
                        fSampleX = fTestPointY - (int)fTestPointY;
                    }
                    if (fTestAngle >= PI * 0.75f || fTestAngle < -PI * 0.75f) {
                        fSampleX = fTestPointX - (int)fTestPointX;
                    }
                }
            }

            fDepthBuffer[x] = fDistanceToWall;

            // Calculate wall height

            int nWallHeight = ScreenHeight() / fDistanceToWall;
            int nCeiling = (ScreenHeight() - nWallHeight) / 2;
            int nFloor = ScreenHeight() - nCeiling;

            wchar_t cShade = ' ';
            short nColor = FG_BLACK;

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
                    cFloorShade = L',';
                }
                else {
                    cFloorShade = L'.';
                }

                if (y < nCeiling) {
                    Draw(x, y, PIXEL_SOLID, FG_CYAN);
                }
                else if (y < nFloor) {
                    if (bHitWall) {
                        fSampleY = (float)(y - nCeiling) / (float)(nFloor - nCeiling);
                        nColor = sprWall.GetColour(sprWall.nWidth * fSampleX, sprWall.nHeight * fSampleY);
                    }
                    Draw(x, y, cShade, nColor);
                }
                else {
                    Draw(x, y, cFloorShade, FG_DARK_GREEN);
                }
            }
        }

        // Map display

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

        for (auto &o : lstObjects) {
            // Update object position
            o.x += o.vx * fElapsedTime;
            o.y += o.vy * fElapsedTime;
            if (o.x < 0 || o.x >= nMapWidth || o.y < 0 || o.y >= nMapHeight || map[(int)o.y * nMapWidth + (int)o.x] == '#') {
                o.bRemove = true;
            }
            if (o.bRemove) {
                continue; // Skip removed objects
            }
            // Place object on mini-map
            Draw(o.x, (nMapHeight - o.y - 1), L'o');
            // Check angle and distance from object
            float fVecX = o.x - fPlayerX;
            float fVecY = o.y - fPlayerY;
            float fObjectDist = sqrt(fVecX*fVecX + fVecY * fVecY);
            float fObjectAngle = atan2f(cosf(fPlayerA), sinf(fPlayerA)) - atan2f(fVecY, fVecX);
            if (fObjectAngle < -PI) { fObjectAngle += PI * 2.0f; }
            if (fObjectAngle > PI) { fObjectAngle -= PI * 2.0f; }
            bool bObjectVisible = (fabs(fObjectAngle) < fFOV / 2.0f) && fObjectDist >= 0.5f && fObjectDist < fDepth;
            if (bObjectVisible) {
                // Object dimensions
                int nObjectCeiling = (ScreenHeight() / 2.0f) - ScreenHeight() / fObjectDist;
                int nObjectFloor = ScreenHeight() - nObjectCeiling;
                int nObjectHeight = nObjectFloor - nObjectCeiling;
                float fAspectRatio = (float)o.sprite->nWidth / o.sprite->nHeight;
                int nObjectWidth = nObjectHeight * fAspectRatio;
                int nObjectMidX = (0.5f * (fObjectAngle / (fFOV / 2.0f)) + 0.5f) * (float)ScreenWidth();
                for (int x = 0; x < nObjectWidth; x++) {
                    int nDrawX = nObjectMidX + x - nObjectWidth / 2;
                    // Use buffer to check if object is behind a wall
                    if (fObjectDist < fDepthBuffer[nDrawX]) {
                        for (int y = 0; y < nObjectHeight; y++) {
                            short nGlyph = o.sprite->SampleGlyph((float)x / nObjectWidth, (float)y / nObjectHeight);
                            short nColor = o.sprite->SampleColour((float)x / nObjectWidth, (float)y / nObjectHeight);
                            if (nGlyph != L' ') { // Transparent
                                Draw(nDrawX, nObjectCeiling + y, nGlyph, nColor);
                            }
                            fDepthBuffer[nDrawX] = fObjectDist;
                        }
                    }
                }
            }
        }

        lstObjects.remove_if([&](Object o) { return o.bRemove; });

        return true;
    }
};

int main()
{
    FPSGame game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

    return 0;
}