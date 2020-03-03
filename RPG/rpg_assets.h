#pragma once

#include <map>
#include <string>
#include "olcPixelGameEngine.h"

namespace Rpg {
    class Assets {
    public:
        static const int TileWidth = 16;
        static const int TileHeight = 16;

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
