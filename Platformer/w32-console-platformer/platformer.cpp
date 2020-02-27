#include <string>
#include "olcConsoleGameEngine.h"
using namespace std;

class PlatformerGame : public olcConsoleGameEngine
{
    wstring Level;
    int LevelWidth;
    int LevelHeight;
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
        Level += L"....#............##.........#.#.................................";
        Level += L"................###.........#.#.................................";
        Level += L"...............####.............................................";
        Level += L"##################################.##########    ###############";
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
        int tileWidth = 8;
        int tileHeight = 8;

        int visibleTilesX = ScreenWidth() / tileWidth;
        int visibleTilesY = ScreenHeight() / tileHeight;

        for (int y = 0; y < visibleTilesY; y++) {
            for (int x = 0; x < visibleTilesX; x++) {
                wchar_t tile = GetTile(x, y);
                int tileX = x * tileWidth;
                int tileY = y * tileHeight;
                switch (tile) {
                case L'.':
                    Fill(tileX, tileY, tileX + tileWidth, tileY + tileHeight, PIXEL_SOLID, FG_CYAN);
                    break;
                case L'#':
                    Fill(tileX, tileY, tileX + tileWidth, tileY + tileHeight, PIXEL_SOLID, FG_RED);
                    break;
                }
            }
        }

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