#include "rpg_assets.h"

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
        loadSpriteData("purple", "rpgdata/gfx/toml_purple.spr");
        loadSpriteData("health", "rpgdata/gfx/item_health.spr");
        loadSpriteData("healthboost", "rpgdata/gfx/item_healthboost.spr");
        loadSpriteData("sword", "rpgdata/gfx/weapon_basic_sword.spr");
    }

    void Assets::LoadMaps()
    {
        m_maps["village"] = new MapVillage1();
        m_maps["home"] = new MapHome1();
    }

    void Assets::LoadItems()
    {
        m_items["health"] = new Item_Health();
        m_items["healthboost"] = new Item_HealthBoost();
        m_items["sword"] = new Weapon_Sword();
    }

    olc::Sprite * Assets::GetSprite(std::string name)
    {
        return m_sprites[name];
    }

    Map * Assets::GetMap(std::string name)
    {
        return m_maps[name];
    }

    Item * Assets::GetItem(std::string name)
    {
        return m_items[name];
    }

}