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
            s->LoadFromFile(filename);
            m_sprites[name] = s;
        };

        loadSpriteData("village", "assets/gfx/toml_village.png");
        loadSpriteData("player", "assets/gfx/toml_CharacterSprites.png");
        loadSpriteData("font", "assets/gfx/javidx9_nesfont8x8.png");
        loadSpriteData("skelly", "assets/gfx/toml_char001.png");
        loadSpriteData("hitech", "assets/gfx/toml_modernish.png");
        loadSpriteData("purple", "assets/gfx/toml_purple.png");
        loadSpriteData("health", "assets/gfx/item_health.png");
        loadSpriteData("healthboost", "assets/gfx/item_healthboost.png");
        loadSpriteData("sword", "assets/gfx/weapon_basic_sword.png");
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