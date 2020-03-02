#pragma once

#include <map>
#include <string>
#include "olcPixelGameEngine.h"

namespace Rpg {
    class Assets {
    public:
        Assets(Assets const&) = delete;
        void operator=(Assets const&) = delete;

        static Assets& GetInstance();
        void LoadSprites();
        olc::Sprite *GetSprite(std::string name);
        
    private:
        Assets();
        ~Assets();
        std::map<std::string, olc::Sprite *> m_sprites;
    };
}
