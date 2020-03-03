
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "rpg_map.h"
#include "rpg_assets.h"
#include "rpg_dynamic.h"

namespace Rpg {
    class Game : public olc::PixelGameEngine
    {
        Map *CurrentMap;
        Dynamic *Player;
        float CameraPosX = 0;
        float CameraPosY = 0;
        bool PlayerOnGround = false;
        bool PlayerFacingLeft = false;

        virtual bool OnUserCreate() override
        {
            Assets::GetInstance().LoadSprites();

            CurrentMap = new MapVillage1();
            Player = new DynamicCreature("player", Assets::GetInstance().GetSprite("player"));
            Player->px = 5;
            Player->py = 5;

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
                    Player->vx -= 10.0f * fElapsedTime;
                }
                if (GetKey(olc::RIGHT).bHeld) {
                    Player->vx += 10.0f * fElapsedTime;
                }
                if (GetKey(olc::UP).bHeld) {
                    Player->vy -= 10.0f * fElapsedTime;
                }
                if (GetKey(olc::DOWN).bHeld) {
                    Player->vy += 10.0f * fElapsedTime;
                }
            }

            // Friction

            Dynamic *object = Player;

            object->vx += -3.0f * object->vx * fElapsedTime;
            if (fabs(object->vx) < 0.01f) {
                object->vx = 0;
            }
            object->vy += -3.0f * object->vy * fElapsedTime;
            if (fabs(object->vy) < 0.01f) {
                object->vy = 0;
            }

            if (object->vx < -10.0f) { object->vx = -10.0f; }
            if (object->vx > 10.0f) { object->vx = 10.0f; }
            if (object->vy < -100.0f) { object->vy = -100.0f; }
            if (object->vy > 100.0f) { object->vy = 100.0f; }

            float newObjectPosX = object->px + object->vx * fElapsedTime;
            float newObjectPosY = object->py + object->vy * fElapsedTime;

            // Collisions

            if (object->vx <= 0) {
                if (CurrentMap->GetSolid(newObjectPosX + 0.0f, object->py + 0.0f) || CurrentMap->GetSolid(newObjectPosX + 0.0f, object->py + 0.9f)) {
                    newObjectPosX = (int)newObjectPosX + 1;
                    object->vx = 0;
                }
            }
            else {
                if (CurrentMap->GetSolid(newObjectPosX + 1.0f, object->py + 0.0f) || CurrentMap->GetSolid(newObjectPosX + 1.0f, object->py + 0.9f)) {
                    newObjectPosX = (int)newObjectPosX;
                    object->vx = 0;
                }
            }

            PlayerOnGround = false;

            if (object->vy <= 0) {
                if (CurrentMap->GetSolid(newObjectPosX + 0.0f, newObjectPosY + 0.0f) || CurrentMap->GetSolid(newObjectPosX + 0.9f, newObjectPosY + 0.0f)) {
                    newObjectPosY = (int)newObjectPosY + 1;
                    object->vy = 0;
                }
            }
            else {
                if (CurrentMap->GetSolid(newObjectPosX + 0.0f, newObjectPosY + 1.0f) || CurrentMap->GetSolid(newObjectPosX + 0.9f, newObjectPosY + 1.0f)) {
                    newObjectPosY = (int)newObjectPosY;
                    object->vy = 0;
                    PlayerOnGround = true;
                }
            }

            object->px = newObjectPosX;
            object->py = newObjectPosY;

            object->Update(fElapsedTime);

            // Rendering

            CameraPosX = Player->px;
            CameraPosY = Player->py;

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
                    DrawPartialSprite(tileX, tileY, CurrentMap->Sprite, sx * tileWidth, sy * tileHeight, tileWidth, tileHeight);                    
                }
            }

            Player->Draw(this, levelOffsetX, levelOffsetY);

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