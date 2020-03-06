#pragma once

#include <map>
#include <string>
#include "olcPixelGameEngine.h"
#include "rpg_map.h"

namespace Rpg {
    class Assets {
    public:
        static const int TileWidth = 16;
        static const int TileHeight = 16;

        Assets(Assets const&) = delete;
        void operator=(Assets const&) = delete;

        static Assets& GetInstance();
        void LoadSprites();
        void LoadMaps();
        olc::Sprite *GetSprite(std::string name);
        Map *GetMap(std::string name);
        
    private:
        Assets();
        ~Assets();
        std::map<std::string, olc::Sprite *> m_sprites;
        std::map<std::string, Map *> m_maps;
    };
}
