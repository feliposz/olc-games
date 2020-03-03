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
        Health = 10;
        MaxHealth = 10;
        m_sprite = sprite;
        m_frame = 0;
        m_timer = 0;
    }

    void DynamicCreature::Update(float elapsed)
    {
        m_timer += elapsed;
        if (m_timer > 0.2f) {
            m_timer -= 0.2f;
            m_frame = (m_frame + 1) % 2;
        }

        if (vx < -0.1f) {
            m_direction = WEST;
        }
        if (vx > 0.1f) {
            m_direction = EAST;
        }
        if (vy < -0.1f) {
            m_direction = NORTH;
        }
        if (vy > 0.1f) {
            m_direction = SOUTH;
        }

        if (Health <= 0) {
            Health = 0;
            m_state = DEAD;
        }
        else if (fabs(vx) > 0.1f || fabs(vy) > 0.1f) {
            m_state = WALKING;
        }
        else {
            m_state = STANDING;
        }
    }

    void DynamicCreature::Draw(olc::PixelGameEngine *engine, float ox, float oy)
    {
        int spriteSheetX = 0;
        int spriteSheetY = 0;
        switch (m_state) {
        case STANDING:
            spriteSheetX = m_direction * 16;
            spriteSheetY = 0 * 16;
            break;
        case WALKING:
            spriteSheetX = m_direction * 16;
            spriteSheetY = m_frame * 16;
            break;
        case CELEBRATING:
            spriteSheetX = 4 * 16;
            spriteSheetY = 0 * 16;
            break;
        case DEAD:
            spriteSheetX = 4 * 16;
            spriteSheetY = 1 * 16;
            break;
        }
        engine->DrawPartialSprite((px - ox) * 16, (py - oy) * 16, m_sprite, spriteSheetX, spriteSheetY, 16, 16);
    }
}