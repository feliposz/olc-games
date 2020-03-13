#pragma once

#include <string>
#include "olcPixelGameEngine.h"

namespace Rpg {

    class Dynamic;

    class Item {
    public:
        std::string Name;
        std::string Description;
        olc::Sprite *Sprite;
        bool Equipable;
        bool KeyItem;

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

}