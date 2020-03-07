#pragma once

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
        ScriptProcessor Script;
        float CameraPosX = 0;
        float CameraPosY = 0;
        std::list<Dynamic *> ListObjects;
        bool DialogDisplay = false;
        std::vector<std::string> DialogContent;

        bool OnUserCreate() override;
        bool OnUserUpdate(float fElapsedTime) override;
        void ShowDialog(std::vector<std::string> content);
        void DrawText(std::string text, float x, float y);
        void DrawDialog(std::vector<std::string> content);
        void ChangeMap(std::string map, float x, float y);
    };

}