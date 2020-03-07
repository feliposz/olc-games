#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "rpg_engine.h"
#include "rpg_map.h"
#include "rpg_assets.h"
#include "rpg_dynamic.h"
#include "rpg_command.h"
#include "game_util.h"

namespace Rpg {


    bool GameEngine::OnUserCreate()
    {
        Command::Engine = this;
        Map::Script = &Script;

        Assets::GetInstance().LoadSprites();
        Assets::GetInstance().LoadMaps();

        Font = Assets::GetInstance().GetSprite("font");

        Player = new Dynamic_Creature("player", Assets::GetInstance().GetSprite("player"));

        ChangeMap("village", 5, 5);

        return true;
    }

    bool GameEngine::OnUserUpdate(float fElapsedTime)
    {

        int visibleTilesX = ScreenWidth() / Assets::TileWidth;
        int visibleTilesY = ScreenHeight() / Assets::TileHeight;

        // Player movement

        Player->vx = 0;
        Player->vy = 0;

        if (IsFocused()) {
            if (Script.UserControlEnabled) {
                if (GetKey(olc::LEFT).bHeld) {
                    Player->vx = -4.0f;
                }
                if (GetKey(olc::RIGHT).bHeld) {
                    Player->vx = 4.0f;
                }
                if (GetKey(olc::UP).bHeld) {
                    Player->vy = -4.0f;
                }
                if (GetKey(olc::DOWN).bHeld) {
                    Player->vy = 4.0f;
                }
            }
            else {
                if (DialogDisplay) {
                    if (GetKey(olc::SPACE).bReleased) {
                        DialogDisplay = false;
                        Script.CompleteCommand();
                    }
                }
            }
        }

        Script.ProcessCommands(fElapsedTime);

        for (auto &object : ListObjects) {

            float newObjectPosX = object->px + object->vx * fElapsedTime;
            float newObjectPosY = object->py + object->vy * fElapsedTime;

            // Collisions with map wall

            float border = 0.1f;

            // Test horizontal first
            if (object->vx <= 0) {
                if (CurrentMap->GetSolid(newObjectPosX + 0.0f, object->py + (0.0f + border)) || CurrentMap->GetSolid(newObjectPosX + 0.0f, object->py + (1.0f - border))) {
                    newObjectPosX = (int)newObjectPosX + 1;
                    object->vx = 0;
                }
            }
            else {
                if (CurrentMap->GetSolid(newObjectPosX + 1.0f, object->py + (0.0f + border)) || CurrentMap->GetSolid(newObjectPosX + 1.0f, object->py + (1.0f - border))) {
                    newObjectPosX = (int)newObjectPosX;
                    object->vx = 0;
                }
            }

            // Test vertical second
            if (object->vy <= 0) {
                if (CurrentMap->GetSolid(newObjectPosX + (0.0f + border), newObjectPosY + 0.0f) || CurrentMap->GetSolid(newObjectPosX + (1.0f - border), newObjectPosY + 0.0f)) {
                    newObjectPosY = (int)newObjectPosY + 1;
                    object->vy = 0;
                }
            }
            else {
                if (CurrentMap->GetSolid(newObjectPosX + (0.0f + border), newObjectPosY + 1.0f) || CurrentMap->GetSolid(newObjectPosX + (1.0f - border), newObjectPosY + 1.0f)) {
                    newObjectPosY = (int)newObjectPosY;
                    object->vy = 0;
                }
            }

            // Handle collision with other dynamic objects

            float newDynamicPosX = newObjectPosX;
            float newDynamicPosY = newObjectPosY;

            for (auto &other : ListObjects) {
                if (object != other) {
                    if (object->SolidDynamic && other->SolidDynamic) {
                        // Test horizontal first
                        if (GameUtil::RectOverlap(newDynamicPosX, object->py, 1.0f, 1.0f, other->px, other->py, 1.0f, 1.0f)) {
                            if (object->vx <= 0) {
                                newDynamicPosX = other->px + 1.0f;
                            }
                            else {
                                newDynamicPosX = other->px - 1.0f;
                            }
                        }
                        // Test vertical second
                        if (GameUtil::RectOverlap(newDynamicPosX, newDynamicPosY, 1.0f, 1.0f, other->px, other->py, 1.0f, 1.0f)) {
                            if (object->vy <= 0) {
                                newDynamicPosY = other->py + 1.0f;
                            }
                            else {
                                newDynamicPosY = other->py - 1.0f;
                            }
                        }
                    }
                    else {
                        if (GameUtil::RectOverlap(newDynamicPosX, newDynamicPosY, 1.0f, 1.0f, other->px, other->py, 1.0f, 1.0f)) {
                            CurrentMap->OnInteraction(ListObjects, other);
                        }
                    }
                }
            }

            object->px = newDynamicPosX;
            object->py = newDynamicPosY;

            object->Update(fElapsedTime, Player);
        }

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

        for (auto &object : ListObjects) {
            object->Draw(this, levelOffsetX, levelOffsetY);
        }

        if (DialogDisplay) {
            DrawDialog(DialogContent);
        }

        return true;
    }

    void GameEngine::ShowDialog(std::vector<std::string> content)
    {
        DialogDisplay = true;
        DialogContent = content;
    }

    void GameEngine::DrawText(std::string text, float x, float y)
    {
        for (int i = 0; i < text.size(); i++) {
            int sx = (text[i] - 32) % 16;
            int sy = (text[i] - 32) / 16;
            DrawPartialSprite(x + i * 8, y, Font, sx * 8, sy * 8, 8, 8);
        }
    }

    void GameEngine::DrawDialog(std::vector<std::string> content)
    {
        int dialogHeight = content.size() * 9;
        int dialogWidth = 0;
        int padding = 4;
        for (auto &line : content) {
            int lineWidth = line.size() * 8;
            if (dialogWidth < lineWidth) {
                dialogWidth = lineWidth;
            }
        }
        FillRect(10, 10, dialogWidth + padding, dialogHeight + padding, olc::BLUE);
        DrawRect(10, 10, dialogWidth + padding, dialogHeight + padding, olc::WHITE);
        int y = 12;
        for (auto &line : content) {
            DrawText(line, 12, y);
            y += 9;
        }
    }

    void GameEngine::ChangeMap(std::string map, float x, float y)
    {
        CurrentMap = Assets::GetInstance().GetMap(map);
        for (auto &object : ListObjects) {
            if (object != Player) {
                delete object;
            }
        }
        ListObjects.clear();
        ListObjects.push_back(Player);
        Player->px = x;
        Player->py = y;
        CurrentMap->PopulateDynamics(ListObjects);
    }

}