#include "rpg_assets.h"
#include "rpg_map.h"

namespace Rpg {

    Assets::Assets()
    {
    }

    Assets::~Assets()
    {
    }

    Assets & Assets::GetInstance()
    {
        static Assets assets;
        return assets;
    }

    void Assets::LoadSprites()
    {
        auto loadSpriteData = [&](std::string name, std::string filename) {
            olc::Sprite *s = new olc::Sprite();
            s->LoadFromPGESprFile(filename);
            m_sprites[name] = s;
        };

        loadSpriteData("village", "rpgdata/gfx/toml_spritesheetdark.spr");
        loadSpriteData("player", "rpgdata/gfx/toml_CharacterSprites.spr");
        loadSpriteData("font", "rpgdata/gfx/javidx9_nesfont8x8.spr");
        loadSpriteData("skelly", "rpgdata/gfx/toml_char001.png.spr");
        loadSpriteData("hitech", "rpgdata/gfx/toml_modernish.spr");
    }

    void Assets::LoadMaps()
    {
        m_maps["village"] = new MapVillage1();
        m_maps["home"] = new MapHome1();
    }

    olc::Sprite * Assets::GetSprite(std::string name)
    {
        return m_sprites[name];
    }

    Map * Assets::GetMap(std::string name)
    {
        return m_maps[name];
    }

}