#pragma once

#include <string>
#include "olcPixelGameEngine.h"

namespace Rpg {

    class Map {

    public:
        int Width;
        int Height;
        olc::Sprite *Sprite = nullptr;

        Map();
        ~Map();

        int GetIndex(int x, int y);
        bool GetSolid(int x, int y);

        void Create(std::string filename, olc::Sprite *sprite);

    private:
        int *m_indices = nullptr;
        bool *m_solids = nullptr;
    };

    class MapVillage1 : public Map {
    public:
        MapVillage1();
    };

}