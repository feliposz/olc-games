#pragma once

#include "olcPixelGameEngine.h"
#include "rpg_map.h"
#include "rpg_assets.h"
#include "rpg_dynamic.h"
#include "rpg_command.h"
#include "rpg_quest.h"
#include "rpg_item.h"

namespace Rpg {

    class GameEngine : public olc::PixelGameEngine
    {
    private:
        Map * CurrentMap = nullptr;
        Dynamic_Creature *Player = nullptr;
        olc::Sprite *Font = nullptr;
        ScriptProcessor Script;
        float CameraPosX = 0;
        float CameraPosY = 0;
        std::list<Dynamic *> ListObjects;
        bool DialogDisplay = false;
        std::vector<std::string> DialogContent;
        std::list<Quest *> ListQuests;
        std::list<Item *> ListItems;

    public:
        bool OnUserCreate() override;
        bool OnUserUpdate(float fElapsedTime) override;
        void ShowDialog(std::vector<std::string> content);
        void DrawText(std::string text, float x, float y);
        void DrawDialog(std::vector<std::string> content);
        void ChangeMap(std::string map, float x, float y);
        void AddQuest(Quest *quest);
        bool GiveItem(Item *item);
        bool TakeItem(Item *item);
        bool HasItem(Item *item);
    };

}