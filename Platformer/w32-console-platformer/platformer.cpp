#include <string>
#include "olcConsoleGameEngine.h"
using namespace std;

class PlatformerGame : public olcConsoleGameEngine
{
    wstring Level;
    int LevelWidth;
    int LevelHeight;
    float PlayerPosX = 0;
    float PlayerPosY = 0;
    float PlayerVelX = 0;
    float PlayerVelY = 0;
    float CameraPosX = 0;
    float CameraPosY = 0;

    virtual bool OnUserCreate() override
    {
        LevelWidth = 64;
        LevelHeight = 16;
        Level += L"................................................................";
        Level += L"................................................................";
        Level += L"................................................................";
        Level += L"................................................................";
        Level += L"..................#.............................................";
        Level += L"....#............##.........#.#...............................#.";
        Level += L".......#........###.........#.#.............................#.#.";
        Level += L"...............####.......................................#.#.#.";
        Level += L"##################################.##########....###############";
        Level += L".................................#.#...........##...............";
        Level += L".....................#############.#.........##.................";
        Level += L".....................#.............#.......##...................";
        Level += L".....................#.#############.....##.....................";
        Level += L".....................#.................##.......................";
        Level += L".....................##################.........................";
        Level += L"................................................................";

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        int tileWidth = 16;
        int tileHeight = 16;

        int visibleTilesX = ScreenWidth() / tileWidth;
        int visibleTilesY = ScreenHeight() / tileHeight;

        PlayerVelX = 0;
        PlayerVelY = 0;

        if (IsFocused()) {
            if (GetKey(VK_LEFT).bHeld) {
                PlayerVelX -= 6.0f;
            }
            if (GetKey(VK_RIGHT).bHeld) {
                PlayerVelX += 6.0f;
            }
            if (GetKey(VK_UP).bHeld) {
                PlayerVelY -= 6.0f;
            }
            if (GetKey(VK_DOWN).bHeld) {
                PlayerVelY += 6.0f;
            }
        }

        PlayerPosX = PlayerPosX + PlayerVelX * fElapsedTime;
        PlayerPosY = PlayerPosY + PlayerVelY * fElapsedTime;

        CameraPosX = PlayerPosX;
        CameraPosY = PlayerPosY;

        float levelOffsetX = CameraPosX - visibleTilesX / 2.0f;
        float levelOffsetY = CameraPosY - visibleTilesY / 2.0f;

        if (levelOffsetX < 0) { levelOffsetX = 0; }
        if (levelOffsetY < 0) { levelOffsetY = 0; }
        if (levelOffsetX > (LevelWidth - visibleTilesX)) { levelOffsetX = LevelWidth - visibleTilesX; }
        if (levelOffsetY > (LevelHeight - visibleTilesY)) { levelOffsetY = LevelHeight - visibleTilesY; }

        float tileOffsetX = (levelOffsetX - (int)levelOffsetX) * tileWidth;
        float tileOffsetY = (levelOffsetY - (int)levelOffsetY) * tileHeight;

        for (int y = -1; y <= visibleTilesY + 1; y++) {
            for (int x = -1; x <= visibleTilesX + 1; x++) {
                wchar_t tile = GetTile(x + levelOffsetX, y + levelOffsetY);
                int tileX = x * tileWidth - tileOffsetX;
                int tileY = y * tileHeight - tileOffsetY;
                switch (tile) {
                case L'.':
                    Fill(tileX, tileY, tileX + tileWidth, tileY + tileHeight, PIXEL_SOLID, FG_CYAN);
                    break;
                case L'#':
                    Fill(tileX, tileY, tileX + tileWidth, tileY + tileHeight, PIXEL_SOLID, FG_RED);
                    break;
                default:
                    Fill(tileX, tileY, tileX + tileWidth, tileY + tileHeight, PIXEL_SOLID, FG_BLACK);
                    break;
                }
            }
        }

        int playerX = (int)((PlayerPosX - levelOffsetX) * tileWidth);
        int playerY = (int)((PlayerPosY - levelOffsetY) * tileHeight);

        Fill(playerX, playerY, playerX + tileWidth, playerY + tileHeight, PIXEL_SOLID, FG_YELLOW);

        return true;
    }

    wchar_t GetTile(int x, int y)
    {
        if (x >= 0 && x < LevelWidth && y >= 0 && y < LevelHeight) {
            return Level[y * LevelWidth + x];
        }
        else {
            return L' ';
        }
    }

    void SetTile(int x, int y, wchar_t value)
    {
        if (x >= 0 && x < LevelWidth && y >= 0 && y < LevelHeight) {
            Level[y * LevelWidth + x] = value;
        }
    }
};

int main()
{
    PlatformerGame game;
    game.ConstructConsole(160, 120, 6, 6);
    game.Start();

    return 0;
}