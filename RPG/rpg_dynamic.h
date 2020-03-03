#pragma once

#include <string>
#include "olcPixelGameEngine.h"

namespace Rpg {

    class Dynamic {

    public:
        float px, py;
        float vx, vy;
        bool SolidMap;
        bool SolidDynamic;
        bool Friendly;
        std::string Name;

        Dynamic(std::string name);
        ~Dynamic();

        virtual void Update(float elapsed) {}
        virtual void Draw(olc::PixelGameEngine *engine, float ox, float oy) {}

    };

    class DynamicCreature : public Dynamic {

    protected:
        olc::Sprite *m_sprite;
        enum { SOUTH = 0, WEST = 1, NORTH = 2, EAST = 3 } m_direction;
        enum { STANDING, WALKING, CELEBRATING, DEAD } m_state;
        int m_frame;
        float m_timer;

    public:
        float Health;
        float MaxHealth;

        DynamicCreature(std::string name, olc::Sprite *sprite);
        void Update(float elapsed) override;
        void Draw(olc::PixelGameEngine *engine, float ox, float oy) override;

    };

}