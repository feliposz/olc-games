#include <string>
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class RpgGame : public olc::PixelGameEngine
{
    std::wstring Level;
    int LevelWidth = 0;
    int LevelHeight = 0;
    float PlayerPosX = 0;
    float PlayerPosY = 0;
    float PlayerVelX = 0;
    float PlayerVelY = 0;
    float CameraPosX = 0;
    float CameraPosY = 0;
    bool PlayerOnGround = false;
    bool PlayerFacingLeft = false;
    olc::Sprite sprJario;
    olc::Sprite sprLevel;

    virtual bool OnUserCreate() override
    {
        LevelWidth = 64;
        LevelHeight = 16;
        Level += L"................................................................";
        Level += L"..................o.............................................";
        Level += L"......oooo........o.............................................";
        Level += L"......oooo...................o................................o.";
        Level += L"..................#.........................................o...";
        Level += L"....BB?BB?BB.....##.........#.#...........................o...#.";
        Level += L"................###.........#.#.............................#.#.";
        Level += L"...............####.......................................#.#.#.";
        Level += L"GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG.GGGGGGGG......GGGGGGGGGGGGGGG";
        Level += L".................................#.#...........##...............";
        Level += L".....................#############.#.o.......##.................";
        Level += L".....................#.............#.......##...................";
        Level += L".....................#.GGGGGGGGGGGGG.....##.....................";
        Level += L".....................#.................##.......................";
        Level += L".....................GGGGGGGGGGGGGGGGGG.........................";
        Level += L"................................................................";

        sprLevel.LoadFromPGESprFile("Sprites/leveljario.spr");
        sprJario.LoadFromPGESprFile("Sprites/minijario.spr");

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        int tileWidth = 16;
        int tileHeight = 16;

        int visibleTilesX = ScreenWidth() / tileWidth;
        int visibleTilesY = ScreenHeight() / tileHeight;

        // Player movement

        if (IsFocused()) {
            if (GetKey(olc::LEFT).bHeld) {
                PlayerVelX -= (PlayerOnGround ? 25.0f : 15.0f) * fElapsedTime;
                PlayerFacingLeft = true;
            }
            if (GetKey(olc::RIGHT).bHeld) {
                PlayerVelX += (PlayerOnGround ? 25.0f : 15.0f) * fElapsedTime;
                PlayerFacingLeft = false;
            }
            if (GetKey(olc::UP).bHeld) {
                PlayerVelY -= 6.0f * fElapsedTime;
            }
            if (GetKey(olc::DOWN).bHeld) {
                PlayerVelY += 6.0f * fElapsedTime;
            }
            if (GetKey(olc::SPACE).bPressed && PlayerOnGround) { // Jump
                PlayerVelY -= 12.0f;
            }
        }

        PlayerVelY += 20.0f * fElapsedTime;

        // Collisions

        if (PlayerOnGround) {
            PlayerVelX += -3.0f * PlayerVelX * fElapsedTime;
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

        // Pick up coins

        if (GetTile(newPlayerPosX + 0, newPlayerPosY + 0) == L'o') {
            SetTile(newPlayerPosX + 0, newPlayerPosY + 0, L'.');
        }
        if (GetTile(newPlayerPosX + 0, newPlayerPosY + 1) == L'o') {
            SetTile(newPlayerPosX + 0, newPlayerPosY + 1, L'.');
        }
        if (GetTile(newPlayerPosX + 1, newPlayerPosY + 0) == L'o') {
            SetTile(newPlayerPosX + 1, newPlayerPosY + 0, L'.');
        }
        if (GetTile(newPlayerPosX + 1, newPlayerPosY + 1) == L'o') {
            SetTile(newPlayerPosX + 1, newPlayerPosY + 1, L'.');
        }

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
                    FillRect(tileX, tileY, tileX + tileWidth, tileY + tileHeight, olc::CYAN);
                    break;
                case L'#':
                    DrawPartialSprite(tileX, tileY, &sprLevel, 2 * tileWidth, 0 * tileHeight, tileWidth, tileHeight);
                    break;
                case L'o':
                    FillRect(tileX, tileY, tileX + tileWidth, tileY + tileHeight, olc::CYAN);
                    DrawPartialSprite(tileX, tileY, &sprLevel, 3 * tileWidth, 0 * tileHeight, tileWidth, tileHeight);
                    break;
                case L'B':
                    DrawPartialSprite(tileX, tileY, &sprLevel, 0 * tileWidth, 1 * tileHeight, tileWidth, tileHeight);
                    break;
                case L'?':
                    DrawPartialSprite(tileX, tileY, &sprLevel, 1 * tileWidth, 1 * tileHeight, tileWidth, tileHeight);
                    break;
                case L'G':                    
                    DrawPartialSprite(tileX, tileY, &sprLevel, 0  * tileWidth, 0 * tileHeight, tileWidth, tileHeight);
                    break;
                default:
                    DrawRect(tileX, tileY, tileX + tileWidth, tileY + tileHeight, olc::BLACK);
                    break;
                }
            }
        }

        int playerX = (int)((PlayerPosX - levelOffsetX) * tileWidth);
        int playerY = (int)((PlayerPosY - levelOffsetY) * tileHeight);
        int spriteX = PlayerOnGround ? 0 : 1;
        int spriteY = PlayerFacingLeft ? 1 : 0;
        DrawPartialSprite(playerX, playerY, &sprJario, spriteX * tileWidth, spriteY * tileHeight, tileWidth, tileHeight);

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
    RpgGame game;
    game.Construct(256, 240, 4, 4);
    game.SetPixelMode(olc::Pixel::MASK);
    game.Start();

    return 0;
}