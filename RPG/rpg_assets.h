#pragma once

#include <map>
#include <string>
#include "olcPixelGameEngine.h"
#include "rpg_map.h"
#include "rpg_item.h"

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
        void LoadItems();
        olc::Sprite *GetSprite(std::string name);
        Map *GetMap(std::string name);
        Item *GetItem(std::string name);
        
    private:
        Assets();
        ~Assets();
        std::map<std::string, olc::Sprite *> m_sprites;
        std::map<std::string, Map *> m_maps;
        std::map<std::string, Item *> m_items;
    };

}
