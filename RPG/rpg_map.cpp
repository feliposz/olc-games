#include <fstream>

#include "rpg_map.h"
#include "rpg_assets.h"

namespace Rpg {

    Map::Map()
    {
    }

    Map::~Map()
    {
        delete[] m_indices;
        delete[] m_solids;
    }

    void Map::Create(std::string filename, olc::Sprite *sprite)
    {
        std::ifstream input(filename, std::ios_base::in);

        if (input.is_open()) {
            input >> Width;
            input >> Height;
            m_indices = new int[Width * Height];
            m_solids = new bool[Width * Height];
            for (int i = 0; i < Width * Height; i++) {
                input >> m_indices[i];
                input >> m_solids[i];
            }
        }
        input.close();

        Sprite = sprite;
    }

    int Map::GetIndex(int x, int y)
    {
        if (x >= 0 && x < Width && y >= 0 && y < Height) {
            return m_indices[y * Width + x];
        }
        else {
            return 0;
        }
    }

    bool Map::GetSolid(int x, int y)
    {
        if (x >= 0 && x < Width && y >= 0 && y < Height) {
            return m_solids[y * Width + x];
        }
        else {
            return 0;
        }
    }

    MapVillage1::MapVillage1() : Map()
    {
        Create("rpgdata/map/village1.lvl", Assets::GetInstance().GetSprite("village"));
    }

    MapHome1::MapHome1() : Map()
    {
        Create("rpgdata/map/home.lvl", Assets::GetInstance().GetSprite("hitech"));
    }
}