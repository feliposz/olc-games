#include "rpg_dynamic.h"

namespace Rpg {

    Dynamic::Dynamic(std::string name)
    {
        px = 0;
        py = 0;
        vx = 0;
        vy = 0;
        SolidMap = true;
        SolidDynamic = true;
        Friendly = true;
        Name = name;
    }

    Dynamic::~Dynamic()
    {
    }

    DynamicCreature::DynamicCreature(std::string name, olc::Sprite *sprite) : Dynamic(name)
    {
        m_sprite = sprite;
    }

    void DynamicCreature::Update(float elapsed)
    {

    }

    void DynamicCreature::Draw(olc::PixelGameEngine *engine, float ox, float oy)
    {
        engine->DrawPartialSprite((px - ox) * 16, (py - oy) * 16, m_sprite, 0, 0, 16, 16);
    }
}