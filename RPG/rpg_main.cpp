
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "rpg_map.h"

namespace Rpg {
    class Game : public olc::PixelGameEngine
    {
        Map *CurrentMap;
        float PlayerPosX = 10;
        float PlayerPosY = 10;
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
            CurrentMap = new MapVillage1();

            sprLevel.LoadFromPGESprFile("rpgdata/gfx/toml_spritesheetdark.spr");
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
                    PlayerVelX -= 10.0f * fElapsedTime;
                    PlayerFacingLeft = true;
                }
                if (GetKey(olc::RIGHT).bHeld) {
                    PlayerVelX += 10.0f * fElapsedTime;
                    PlayerFacingLeft = false;
                }
                if (GetKey(olc::UP).bHeld) {
                    PlayerVelY -= 10.0f * fElapsedTime;
                }
                if (GetKey(olc::DOWN).bHeld) {
                    PlayerVelY += 10.0f * fElapsedTime;
                }
            }

            // Friction

            PlayerVelX += -3.0f * PlayerVelX * fElapsedTime;
            if (fabs(PlayerVelX) < 0.01f) {
                PlayerVelX = 0;
            }
            PlayerVelY += -3.0f * PlayerVelY * fElapsedTime;
            if (fabs(PlayerVelY) < 0.01f) {
                PlayerVelY = 0;
            }

            if (PlayerVelX < -10.0f) { PlayerVelX = -10.0f; }
            if (PlayerVelX > 10.0f) { PlayerVelX = 10.0f; }
            if (PlayerVelY < -100.0f) { PlayerVelY = -100.0f; }
            if (PlayerVelY > 100.0f) { PlayerVelY = 100.0f; }

            float newPlayerPosX = PlayerPosX + PlayerVelX * fElapsedTime;
            float newPlayerPosY = PlayerPosY + PlayerVelY * fElapsedTime;

            // Collisions

            if (PlayerVelX <= 0) {
                if (CurrentMap->GetSolid(newPlayerPosX + 0.0f, PlayerPosY + 0.0f) || CurrentMap->GetSolid(newPlayerPosX + 0.0f, PlayerPosY + 0.9f)) {
                    newPlayerPosX = (int)newPlayerPosX + 1;
                    PlayerVelX = 0;
                }
            }
            else {
                if (CurrentMap->GetSolid(newPlayerPosX + 1.0f, PlayerPosY + 0.0f) || CurrentMap->GetSolid(newPlayerPosX + 1.0f, PlayerPosY + 0.9f)) {
                    newPlayerPosX = (int)newPlayerPosX;
                    PlayerVelX = 0;
                }
            }

            PlayerOnGround = false;

            if (PlayerVelY <= 0) {
                if (CurrentMap->GetSolid(newPlayerPosX + 0.0f, newPlayerPosY + 0.0f) || CurrentMap->GetSolid(newPlayerPosX + 0.9f, newPlayerPosY + 0.0f)) {
                    newPlayerPosY = (int)newPlayerPosY + 1;
                    PlayerVelY = 0;
                }
            }
            else {
                if (CurrentMap->GetSolid(newPlayerPosX + 0.0f, newPlayerPosY + 1.0f) || CurrentMap->GetSolid(newPlayerPosX + 0.9f, newPlayerPosY + 1.0f)) {
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
            if (levelOffsetX > (CurrentMap->Width - visibleTilesX)) { levelOffsetX = CurrentMap->Width - visibleTilesX; }
            if (levelOffsetY > (CurrentMap->Height - visibleTilesY)) { levelOffsetY = CurrentMap->Height - visibleTilesY; }

            float tileOffsetX = (levelOffsetX - (int)levelOffsetX) * tileWidth;
            float tileOffsetY = (levelOffsetY - (int)levelOffsetY) * tileHeight;

            for (int y = -1; y <= visibleTilesY + 1; y++) {
                for (int x = -1; x <= visibleTilesX + 1; x++) {
                    int index = CurrentMap->GetIndex(x + levelOffsetX, y + levelOffsetY);
                    int tileX = x * tileWidth - tileOffsetX;
                    int tileY = y * tileHeight - tileOffsetY;
                    int sx = index % 10;
                    int sy = index / 10;
                    DrawPartialSprite(tileX, tileY, &sprLevel, sx * tileWidth, sy * tileHeight, tileWidth, tileHeight);                    
                }
            }

            int playerX = (int)((PlayerPosX - levelOffsetX) * tileWidth);
            int playerY = (int)((PlayerPosY - levelOffsetY) * tileHeight);
            int spriteX = 0;
            int spriteY = PlayerFacingLeft ? 1 : 0;
            DrawPartialSprite(playerX, playerY, &sprJario, spriteX * tileWidth, spriteY * tileHeight, tileWidth, tileHeight);

            return true;
        }
    };

};

int main()
{
    Rpg::Game game;
    game.Construct(256, 240, 4, 4);
    game.SetPixelMode(olc::Pixel::MASK);
    game.Start();

    return 0;
}