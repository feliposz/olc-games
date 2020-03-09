#include "rpg_quest.h"
#include "rpg_assets.h"

namespace Rpg {

    ScriptProcessor *Quest::Script = nullptr;

    bool Quest_Test::OnInteraction(std::list<Dynamic*> objects, Dynamic * target, QuestNature nature)
    {
        if (target->Name == "bob" && nature == QuestNature::TALK) {
            if (m_phase == 0) {
                Script->AddCommand(new Command_Say({ "[BOB]", "You are on phase 0", "of the quest!" }));
                Script->AddCommand(new Command_Say({ "[BOB]", "You need to talk", "to me inside" }));
                m_phase = 1;
                return true;
            }
            else if (m_phase == 1) {
                Script->AddCommand(new Command_Say({ "[BOB]", "You are on phase 1", "of the quest!" }));
                return true;
            }
        }
        return false;
    }

    void Quest_Test::PopulateDynamics(std::list<Dynamic*>& objects, std::string map)
    {
        if (m_phase == 0 && map == "village") {
            Dynamic_Creature *bob = new Dynamic_Creature("bob", Assets::GetInstance().GetSprite("skelly"));
            bob->px = 6;
            bob->py = 4;
            objects.push_back(bob);
        }
        else if (m_phase == 1 && map == "home") {
            Dynamic_Creature *bob = new Dynamic_Creature("bob", Assets::GetInstance().GetSprite("skelly"));
            bob->px = 11;
            bob->py = 4;
            objects.push_back(bob);
        }
    }

}