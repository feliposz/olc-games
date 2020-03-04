#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "rpg_map.h"
#include "rpg_assets.h"
#include "rpg_dynamic.h"
#include "rpg_command.h"

namespace Rpg {

    class GameEngine : public olc::PixelGameEngine
    {
    public:
        Map * CurrentMap = nullptr;
        Dynamic *Player = nullptr;
        olc::Sprite *Font = nullptr;
        ScriptProcessor ScriptProc;
        float CameraPosX = 0;
        float CameraPosY = 0;

        bool OnUserCreate()
        {
            Assets::GetInstance().LoadSprites();

            Font = Assets::GetInstance().GetSprite("font");
            CurrentMap = new MapVillage1();
            Player = new DynamicCreature("player", Assets::GetInstance().GetSprite("player"));
            Player->px = 5;
            Player->py = 5;

            return true;
        }

        bool OnUserUpdate(float fElapsedTime)
        {

            int visibleTilesX = ScreenWidth() / Assets::TileWidth;
            int visibleTilesY = ScreenHeight() / Assets::TileHeight;

            // Player movement

            if (IsFocused()) {
                if (ScriptProc.UserControlEnabled) {
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
                    if (GetKey(olc::Z).bReleased) {
                        ScriptProc.AddCommand(new Command_WalkTo(Player, 10, 10, 1.5f));
                        ScriptProc.AddCommand(new Command_Wait(0.5f));
                        ScriptProc.AddCommand(new Command_WalkTo(Player, 10, 15, 1.5f));
                        ScriptProc.AddCommand(new Command_Wait(0.5f));
                        ScriptProc.AddCommand(new Command_WalkTo(Player, 15, 10, 1.5f));
                        ScriptProc.AddCommand(new Command_Say({ "Hello", "world!" }));
                    }
                }
                else {
                    if (GetKey(olc::SPACE).bReleased) {
                        ScriptProc.CompleteCommand();
                    }
                }
            }

            ScriptProc.ProcessCommands(fElapsedTime);

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

            float tileOffsetX = (levelOffsetX - (int)levelOffsetX) * Assets::TileWidth;
            float tileOffsetY = (levelOffsetY - (int)levelOffsetY) * Assets::TileHeight;

            for (int y = -1; y <= visibleTilesY + 1; y++) {
                for (int x = -1; x <= visibleTilesX + 1; x++) {
                    int index = CurrentMap->GetIndex(x + levelOffsetX, y + levelOffsetY);
                    int tileX = x * Assets::TileWidth - tileOffsetX;
                    int tileY = y * Assets::TileHeight - tileOffsetY;
                    int sx = index % 10;
                    int sy = index / 10;
                    DrawPartialSprite(tileX, tileY, CurrentMap->Sprite, sx * Assets::TileWidth, sy * Assets::TileHeight, Assets::TileWidth, Assets::TileHeight);
                }
            }

            Player->Draw(this, levelOffsetX, levelOffsetY);

            return true;
        }

        void DrawText(std::string text, float x, float y)
        {
            for (int i = 0; i < text.size(); i++) {
                int sx = (text[i] - 32) % 16;
                int sy = (text[i] - 32) / 16;
                DrawPartialSprite(x + i * 8, y, Font, sx * 8, sy * 8, 8, 8);
            }
        }

    };

}

int main()
{
    Rpg::GameEngine game;
    game.Construct(256, 240, 4, 4);
    game.SetPixelMode(olc::Pixel::MASK);
    game.Start();

    return 0;
}