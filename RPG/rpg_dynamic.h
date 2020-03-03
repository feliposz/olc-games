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

    public:
        DynamicCreature(std::string name, olc::Sprite *sprite);
        void Update(float elapsed) override;
        void Draw(olc::PixelGameEngine *engine, float ox, float oy) override;

    };

}