#pragma once

#include <string>
#include "olcPixelGameEngine.h"

namespace Rpg {

    class Dynamic;
    class GameEngine;

    class Item {
    public:
        std::string Name;
        std::string Description;
        olc::Sprite *Sprite;
        bool Equipable;
        bool KeyItem;

        static GameEngine *Engine;

        Item(std::string name, std::string description, olc::Sprite *sprite, bool equipable, bool keyItem);
        ~Item();

        virtual bool OnInteract(Dynamic *object);
        virtual bool OnUse(Dynamic *object);
    
    };

    class Item_Health : public Item {
    public:
        Item_Health();
        bool OnInteract(Dynamic *object) override;
        bool OnUse(Dynamic *object) override;
    };

    class Item_HealthBoost : public Item {
    public:
        Item_HealthBoost();
        bool OnInteract(Dynamic *object) override;
        bool OnUse(Dynamic *object) override;
    };

    class Weapon : public Item {
    public:
        Weapon(std::string name, std::string description, olc::Sprite *sprite, int damage);
        bool OnInteract(Dynamic *object) override;
        bool OnUse(Dynamic *object) override;
    protected:
        int m_damage = 0;
    };

    class Weapon_Sword : public Weapon {
    public:
        Weapon_Sword();
        bool OnUse(Dynamic *object) override;
    };

}