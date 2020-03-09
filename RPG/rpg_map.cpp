#include <fstream>

#include "rpg_map.h"
#include "rpg_assets.h"
#include "rpg_dynamic.h"

namespace Rpg {

    ScriptProcessor *Map::Script = nullptr;

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

    bool MapVillage1::PopulateDynamics(std::list<Dynamic*>& ListDynamics)
    {
        ListDynamics.push_back(new Dynamic_Teleport(12, 6, "home", 5, 12));
        
        for (int i = 0; i < 3; i++) {
            Dynamic_Creature_Skelly *skelly = new Dynamic_Creature_Skelly();
            skelly->px = 5 + i * 6;
            skelly->py = 15 - i * 2;
            ListDynamics.push_back(skelly);
        }

        return true;
    }

    void MapVillage1::OnInteraction(std::list<Dynamic*>& ListDynamics, Dynamic * object, InteractNature nature)
    {
        if (object->Name == "teleport" && nature == InteractNature::WALK) {
            Dynamic_Teleport *t = (Dynamic_Teleport*) object;
            Script->AddCommand(new Command_ChangeMap(t->TargetMap, t->TargetX, t->TargetY));
        }
    }

    MapHome1::MapHome1() : Map()
    {
        Create("rpgdata/map/home.lvl", Assets::GetInstance().GetSprite("hitech"));
    }

    bool MapHome1::PopulateDynamics(std::list<Dynamic*>& ListDynamics)
    {
        ListDynamics.push_back(new Dynamic_Teleport(5, 13, "village", 12, 7));
        ListDynamics.push_back(new Dynamic_Teleport(4, 13, "village", 12, 7));
        Dynamic_Creature *bob = new Dynamic_Creature("bob", Assets::GetInstance().GetSprite("skelly"));
        bob->px = 11;
        bob->py = 4;
        ListDynamics.push_back(bob);
        return true;
    }

    void MapHome1::OnInteraction(std::list<Dynamic*>& ListDynamics, Dynamic * object, InteractNature nature)
    {
        if (object->Name == "teleport" && nature == InteractNature::WALK) {
            Dynamic_Teleport *t = (Dynamic_Teleport*)object;
            Script->AddCommand(new Command_ChangeMap(t->TargetMap, t->TargetX, t->TargetY));
        }
        if (object->Name == "bob" && nature == InteractNature::TALK) {
            Script->AddCommand(new Command_Say({ "[BOB]", "Hello.", "I'm bob!" }));
        }
    }
}