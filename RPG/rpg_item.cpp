#include "rpg_item.h"
#include "rpg_assets.h"

namespace Rpg {

    Item::Item(std::string name, std::string description, olc::Sprite * sprite, bool equipable, bool keyItem)
    {
        Name = name;
        Description = description;
        Sprite = sprite;
        Equipable = equipable;
        KeyItem = keyItem;
    }

    Item::~Item()
    {
    }

    bool Item::OnInteract(Dynamic * object)
    {
        return false;
    }

    bool Item::OnUse(Dynamic * object)
    {
        return false;
    }

    Item_Health::Item_Health() : Item("Small Health", "Restores 10 health", Assets::GetInstance().GetSprite("health"), false, false)
    {
    }

    bool Item_Health::OnInteract(Dynamic * object)
    {
        OnUse(object);
        return false;
    }

    bool Item_Health::OnUse(Dynamic * object)
    {
        if (object != nullptr) {
            Dynamic_Creature *c = (Dynamic_Creature *)object;
            c->Health = c->Health + 10 < c->MaxHealth ? c->Health + 10 : c->MaxHealth;
        }
        return true;
    }

    Item_HealthBoost::Item_HealthBoost() : Item("Health Boost", "Increases Max Health by 10", Assets::GetInstance().GetSprite("healthboost"), false, false)
    {
    }

    bool Item_HealthBoost::OnInteract(Dynamic * object)
    {
        return true;
    }

    bool Item_HealthBoost::OnUse(Dynamic * object)
    {
        if (object != nullptr) {
            Dynamic_Creature *c = (Dynamic_Creature *)object;
            c->MaxHealth += 10;
            c->Health = c->MaxHealth;
            return true;
        }
        return false;
    }

    Weapon::Weapon(std::string name, std::string description, olc::Sprite * sprite, int damage) : Item(name, description, sprite, true, false)
    {
        m_damage = damage;
    }

    bool Weapon::OnInteract(Dynamic * object)
    {
        return false;
    }

    bool Weapon::OnUse(Dynamic * object)
    {
        return false;
    }

    Weapon_Sword::Weapon_Sword() : Weapon("Basic sword", "A wooden sword, 5 dmg", Assets::GetInstance().GetSprite("sword"), 5)
    {
    }

    bool Weapon_Sword::OnUse(Dynamic * object)
    {
        // TODO: Create projectile
        return false;
    }

}