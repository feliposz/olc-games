#pragma once

#include <string>
#include "olcPixelGameEngine.h"
#include "rpg_command.h"
#include "rpg_dynamic.h"

namespace Rpg {

    enum InteractNature { WALK, TALK };

    class Map {

    public:
        int Width;
        int Height;
        olc::Sprite *Sprite = nullptr;

        static ScriptProcessor *Script;

        Map();
        ~Map();

        int GetIndex(int x, int y);
        bool GetSolid(int x, int y);

        void Create(std::string filename, olc::Sprite *sprite);

        virtual bool PopulateDynamics(std::list<Dynamic *> &ListDynamics)
        {
            return false;
        }

        virtual void OnInteraction(std::list<Dynamic *> &ListDynamics, Dynamic *object, InteractNature nature) {}

    private:
        int *m_indices = nullptr;
        bool *m_solids = nullptr;
    };

    class MapVillage1 : public Map {
    public:
        MapVillage1();
        bool PopulateDynamics(std::list<Dynamic *> &ListDynamics) override;
        void OnInteraction(std::list<Dynamic *> &ListDynamics, Dynamic *object, InteractNature nature) override;
    };

    class MapHome1 : public Map {
    public:
        MapHome1();
        bool PopulateDynamics(std::list<Dynamic *> &ListDynamics) override;
        void OnInteraction(std::list<Dynamic *> &ListDynamics, Dynamic *object, InteractNature nature) override;
    };

}