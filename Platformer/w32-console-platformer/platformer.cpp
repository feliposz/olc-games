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
    bool PlayerOnGround = false;

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
        Level += L"##################################.########......###############";
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

        // Player movement

        if (IsFocused()) {
            if (GetKey(VK_LEFT).bHeld) {
                PlayerVelX -= (PlayerOnGround ? 8.0f : 4.0f) * fElapsedTime;
            }
            if (GetKey(VK_RIGHT).bHeld) {
                PlayerVelX += (PlayerOnGround ? 8.0f : 4.0f) * fElapsedTime;
            }
            if (GetKey(VK_UP).bHeld) {
                PlayerVelY -= 6.0f * fElapsedTime;
            }
            if (GetKey(VK_DOWN).bHeld) {
                PlayerVelY += 6.0f * fElapsedTime;
            }
            if (GetKey(VK_SPACE).bPressed && PlayerOnGround) { // Jump
                PlayerVelY -= 12.0f;
            }
        }

        PlayerVelY += 20.0f * fElapsedTime;

        // Collisions

        if (PlayerOnGround) {
            PlayerVelX += -2.0f * PlayerVelX * fElapsedTime;
            if (fabs(PlayerVelX) < 0.01f) {
                PlayerVelX = 0;
            }
        }

        if (PlayerVelX < -10.0f) { PlayerVelX = -10.0f; }
        if (PlayerVelX > 10.0f) { PlayerVelX = 10.0f; }
        if (PlayerVelY < -100.0f) { PlayerVelY = -100.0f; }
        if (PlayerVelY > 100.0f) { PlayerVelY = 100.0f; }

        float newPlayerPosX = PlayerPosX + PlayerVelX * fElapsedTime;
        float newPlayerPosY = PlayerPosY + PlayerVelY * fElapsedTime;

        if (PlayerVelX <= 0) {
            if (GetTile(newPlayerPosX + 0.0f, PlayerPosY + 0.0f) != L'.' || GetTile(newPlayerPosX + 0.0f, PlayerPosY + 0.9f) != L'.') {
                newPlayerPosX = (int)newPlayerPosX + 1;
                PlayerVelX = 0;
            }
        }
        else {
            if (GetTile(newPlayerPosX + 1.0f, PlayerPosY + 0.0f) != L'.' || GetTile(newPlayerPosX + 1.0f, PlayerPosY + 0.9f) != L'.') {
                newPlayerPosX = (int)newPlayerPosX;
                PlayerVelX = 0;
            }
        }

        PlayerOnGround = false;

        if (PlayerVelY <= 0) {
            if (GetTile(newPlayerPosX + 0.0f, newPlayerPosY + 0.0f) != L'.' || GetTile(newPlayerPosX + 0.9f, newPlayerPosY + 0.0f) != L'.') {
                newPlayerPosY = (int)newPlayerPosY + 1;
                PlayerVelY = 0;
            }
        }
        else {
            if (GetTile(newPlayerPosX + 0.0f, newPlayerPosY + 1.0f) != L'.' || GetTile(newPlayerPosX + 0.9f, newPlayerPosY + 1.0f) != L'.') {
                newPlayerPosY = (int)newPlayerPosY;
                PlayerVelY = 0;
                PlayerOnGround = true;
            }
        }

        PlayerPosX = newPlayerPosX;
        PlayerPosY = newPlayerPosY;

        // Rendering

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