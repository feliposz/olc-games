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
        ScriptProcessor ScriptProc;
        float CameraPosX = 0;
        float CameraPosY = 0;
        std::list<Dynamic *> ListObjects;
        Dynamic *Skelly1 = nullptr;
        Dynamic *Skelly2 = nullptr;

        bool OnUserCreate() override;
        bool OnUserUpdate(float fElapsedTime) override;
        void DrawText(std::string text, float x, float y);
        void DrawDialog(std::vector<std::string> content);
    };

}