#pragma once

#include <string>
#include "olcPixelGameEngine.h"

namespace Rpg {

    enum FacingDirection { SOUTH = 0, WEST = 1, NORTH = 2, EAST = 3 };
    enum CreatureState { STANDING, WALKING, CELEBRATING, DEAD };

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

        virtual void Update(float elapsed, Dynamic *player) {}
        virtual void Draw(olc::PixelGameEngine *engine, float ox, float oy) {}

    };

    class Dynamic_Creature : public Dynamic {

    protected:
        olc::Sprite *m_sprite;
        FacingDirection m_direction;
        CreatureState m_state;
        int m_frame;
        float m_timer;
        float m_stateTick;

    public:
        float Health;
        float MaxHealth;

        Dynamic_Creature(std::string name, olc::Sprite *sprite);
        void Update(float elapsed, Dynamic *player) override;
        void Draw(olc::PixelGameEngine *engine, float ox, float oy) override;
        virtual void Behavior(float elapsed, Dynamic *player) {}
        FacingDirection GetDirection() { return m_direction; }
    };

    class Dynamic_Creature_Skelly : public Dynamic_Creature {
    public:
        Dynamic_Creature_Skelly();
        void Behavior(float elapsed, Dynamic *player) override;
    };

    class Dynamic_Teleport : public Dynamic {

    public:
        std::string TargetMap;
        int TargetX;
        int TargetY;

        Dynamic_Teleport(float x, float y, std::string map, float tx, float ty);
        void Draw(olc::PixelGameEngine *engine, float ox, float oy) override;
    };

}