#pragma once
#include "rpg_dynamic.h"
#include "rpg_command.h"

namespace Rpg {

    class Quest {
    public:
        Quest() {}
        ~Quest() {}

        enum QuestNature {
            TALK,
            WALK,
            ATTACK,
            KILL
        };

        bool Completed;

        static ScriptProcessor *Script;

        virtual bool OnInteraction(std::list<Dynamic *> objects, Dynamic *target, QuestNature nature) { return false; }
        virtual void PopulateDynamics(std::list<Dynamic *> &objects, std::string map) {}
    };

    class Quest_Test : public Quest {
    public:
        virtual bool OnInteraction(std::list<Dynamic *> objects, Dynamic *target, QuestNature nature);
        virtual void PopulateDynamics(std::list<Dynamic *> &objects, std::string map);
    private:
        int m_phase = 0;
    };

}
