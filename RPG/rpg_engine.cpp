#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "rpg_engine.h"
#include "game_util.h"

namespace Rpg {


    bool GameEngine::OnUserCreate()
    {
        Map::Script = &Script;
        Quest::Script = &Script;
        Command::Engine = this;
        Quest::Engine = this;
        Dynamic::Engine = this;
        Item::Engine = this;

        Assets::GetInstance().LoadSprites();
        Assets::GetInstance().LoadItems();
        Assets::GetInstance().LoadMaps();

        Font = Assets::GetInstance().GetSprite("font");

        Player = new Dynamic_Creature_Player();

        ListQuests.push_back(new Quest_MainQuest());

        ChangeMap("village", 5, 5);

        return true;
    }

    bool GameEngine::OnUserUpdate(float fElapsedTime)
    {
        switch (GameMode) {
        case GM_LocalMap:
            return UpdateLocalMap(fElapsedTime);
        case GM_Inventory:
            return UpdateInventory(fElapsedTime);
        }
        return true;
    }

    bool GameEngine::UpdateLocalMap(float fElapsedTime)
    {
        int visibleTilesX = ScreenWidth() / Assets::TileWidth;
        int visibleTilesY = ScreenHeight() / Assets::TileHeight;

        // Clear completed quests

        auto removeQuestIdx = std::remove_if(ListQuests.begin(), ListQuests.end(), [&](Quest *quest) { return quest->Completed; });
        if (removeQuestIdx != ListQuests.end()) {
            ListQuests.erase(removeQuestIdx);
        }

        // Clear projectiles

        auto removeProjIdx = std::remove_if(ListProjectiles.begin(), ListProjectiles.end(), [&](Dynamic *projectile) { return ((Dynamic_Projectile*)projectile)->Redundant; });
        for (auto &projectile : ListProjectiles) {
            if (projectile && ((Dynamic_Projectile *)projectile)->Redundant) {
                delete projectile;
                projectile = nullptr;
            }
        }
        ListProjectiles.erase(removeProjIdx, ListProjectiles.end());

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
                if (GetKey(olc::Z).bReleased) {
                    GameMode = GM_Inventory;
                }
                if (GetKey(olc::SPACE).bReleased) {
                    float probeX = Player->px + 0.5f;
                    float probeY = Player->py + 0.5f;
                    switch (Player->GetDirection()) {
                    case FacingDirection::EAST:
                        probeX += 1.0f;
                        break;
                    case FacingDirection::WEST:
                        probeX -= 1.0f;
                        break;
                    case FacingDirection::NORTH:
                        probeY -= 1.0f;
                        break;
                    case FacingDirection::SOUTH:
                        probeY += 1.0f;
                        break;
                    }
                    bool hitSomething = false;
                    for (auto &target : ListObjects) {
                        if (target != Player) {
                            if (probeX > target->px && probeX < (target->px + 1.0f) && probeY > target->py && probeY < (target->py + 1.0f)) {
                                hitSomething = true;
                                if (target->Friendly) {
                                    for (auto &quest : ListQuests) {
                                        if (quest->OnInteraction(ListObjects, target, Quest::QuestNature::TALK)) {
                                            break;
                                        }
                                    }
                                    CurrentMap->OnInteraction(ListObjects, target, Map::InteractNature::TALK);
                                }
                                else {
                                    Player->PerformAttack();
                                }
                            }
                        }
                    }
                    if (!hitSomething) {
                        Player->PerformAttack();
                    }
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

        bool handlingProjectiles = false;
        for (auto &list : { ListObjects, ListProjectiles }) {
            for (auto &object : list) {

                float newObjectPosX = object->px + object->vx * fElapsedTime;
                float newObjectPosY = object->py + object->vy * fElapsedTime;

                // Collisions with map wall

                if (object->SolidMap) {
                    float border = 0.1f;
                    bool collidedWithMap = false;

                    // Test horizontal first
                    if (object->vx <= 0) {
                        if (CurrentMap->GetSolid(newObjectPosX + 0.0f, object->py + (0.0f + border)) || CurrentMap->GetSolid(newObjectPosX + 0.0f, object->py + (1.0f - border))) {
                            newObjectPosX = (int)newObjectPosX + 1;
                            object->vx = 0;
                            collidedWithMap = true;
                        }
                    }
                    else {
                        if (CurrentMap->GetSolid(newObjectPosX + 1.0f, object->py + (0.0f + border)) || CurrentMap->GetSolid(newObjectPosX + 1.0f, object->py + (1.0f - border))) {
                            newObjectPosX = (int)newObjectPosX;
                            object->vx = 0;
                            collidedWithMap = true;
                        }
                    }

                    // Test vertical second
                    if (object->vy <= 0) {
                        if (CurrentMap->GetSolid(newObjectPosX + (0.0f + border), newObjectPosY + 0.0f) || CurrentMap->GetSolid(newObjectPosX + (1.0f - border), newObjectPosY + 0.0f)) {
                            newObjectPosY = (int)newObjectPosY + 1;
                            object->vy = 0;
                            collidedWithMap = true;
                        }
                    }
                    else {
                        if (CurrentMap->GetSolid(newObjectPosX + (0.0f + border), newObjectPosY + 1.0f) || CurrentMap->GetSolid(newObjectPosX + (1.0f - border), newObjectPosY + 1.0f)) {
                            newObjectPosY = (int)newObjectPosY;
                            object->vy = 0;
                            collidedWithMap = true;
                        }
                    }

                    if (handlingProjectiles && collidedWithMap) {
                        ((Dynamic_Projectile *)object)->Redundant = true;
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
                            if (object == Player) { // Only player interacts with objects
                                if (GameUtil::RectOverlap(newDynamicPosX, newDynamicPosY, 1.0f, 1.0f, other->px, other->py, 1.0f, 1.0f)) {
                                    for (auto &quest : ListQuests) {
                                        if (quest->OnInteraction(ListObjects, other, Quest::QuestNature::WALK)) {
                                            break;
                                        }
                                    }
                                    CurrentMap->OnInteraction(ListObjects, other, Map::InteractNature::WALK);
                                    other->OnInteract(object);
                                }
                            }
                            else {
                                if (handlingProjectiles) {
                                    if (other->IsAttackable && other->Friendly != object->Friendly) {
                                        if (GameUtil::RectOverlap(newDynamicPosX, newDynamicPosY, 1.0f, 1.0f, other->px, other->py, 1.0f, 1.0f)) {
                                            Dynamic_Projectile *projectile = (Dynamic_Projectile *)object;
                                            Dynamic_Creature *victim = (Dynamic_Creature *)other;
                                            DoDamage(victim, projectile);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                object->px = newDynamicPosX;
                object->py = newDynamicPosY;

                object->Update(fElapsedTime, Player);
            }
            handlingProjectiles = true;
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

        for (auto &list : { ListObjects, ListProjectiles }) {
            for (auto &object : list) {
                object->Draw(this, levelOffsetX, levelOffsetY);
            }
        }

        if (DialogDisplay) {
            DrawDialog(DialogContent);
        }

        DrawText("HP:" + std::to_string(Player->Health) + "/" + std::to_string(Player->MaxHealth), 180, 16);

        return true;
    }

    bool GameEngine::UpdateInventory(float fElapsedTime)
    {
        FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

        DrawText("INVENTORY", 4, 4);

        Item *selected = nullptr;

        int i = 0;
        for (auto &item : ListItems) {
            int x = i % 4;
            int y = i / 4;
            i++;
            if (SelectX == x && SelectY == y) {
                selected = item;
            }
            DrawPartialSprite(8 + x * (Assets::TileWidth + 4), 20 + y * (Assets::TileHeight + 4), item->Sprite, 0, 0, Assets::TileWidth, Assets::TileHeight);
        }

        DrawRect(8 + SelectX * (Assets::TileWidth + 4), 20 + SelectY * (Assets::TileHeight + 4), Assets::TileWidth, Assets::TileHeight);

        if (GetKey(olc::LEFT).bReleased) SelectX--;
        if (GetKey(olc::RIGHT).bReleased) SelectX++;
        if (GetKey(olc::UP).bReleased) SelectY--;
        if (GetKey(olc::DOWN).bReleased) SelectY++;
        if (SelectX < 0) SelectX = 3;
        if (SelectX > 3) SelectX = 0;
        if (SelectY < 0) SelectY = 3;
        if (SelectY > 3) SelectY = 0;

        if (selected) {
            DrawText("SELECTED:", 8, 160);
            DrawText(selected->Name, 8, 170);
            DrawText("DESCRIPTION:", 8, 190);
            DrawText(selected->Description, 8, 200);
            if (!selected->KeyItem) {
                DrawText("(Press SPACE to use)", 80, 160);
                if (GetKey(olc::SPACE).bReleased) {
                    if (selected->OnUse(Player)) {
                        TakeItem(selected);
                    }
                }
            }
        }

        DrawText("LOCATION:", 128, 8);
        DrawText(CurrentMap->Name, 128, 16);

        DrawText("HEALTH: " + std::to_string(Player->Health), 128, 32);
        DrawText("MAX HEALTH: " + std::to_string(Player->MaxHealth), 128, 40);

        if (IsFocused()) {
            if (GetKey(olc::Z).bReleased) {
                GameMode = GM_LocalMap;
            }
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
        for (auto &quest : ListQuests) {
            quest->PopulateDynamics(ListObjects, map);
        }
    }

    void GameEngine::AddQuest(Quest *quest)
    {
        ListQuests.push_front(quest);
    }

    bool GameEngine::GiveItem(Item *item)
    {
        ListItems.push_back(item);
        return true;
    }

    bool GameEngine::TakeItem(Item *item)
    {
        if (item != nullptr) {
            ListItems.erase(find(ListItems.begin(), ListItems.end(), item));
            return true;
        }
        return false;
    }

    bool GameEngine::HasItem(Item *item)
    {
        return find(ListItems.begin(), ListItems.end(), item) != ListItems.end();
    }

    void GameEngine::AddProjectile(Dynamic_Projectile *projectile)
    {
        ListProjectiles.push_back(projectile);
    }

    void GameEngine::DoDamage(Dynamic_Creature * victim, Dynamic_Projectile * projectile)
    {
        if (victim != nullptr && projectile != nullptr) {
            victim->Health -= projectile->Damage;
            if (projectile->OneHit) {
                projectile->Redundant = true;
            }
        }
    }

}