#include "rpg_quest.h"
#include "rpg_assets.h"
#include "rpg_engine.h"

namespace Rpg {

    ScriptProcessor *Quest::Script = nullptr;
    GameEngine *Quest::Engine = nullptr;

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

    void Quest_MainQuest::PopulateDynamics(std::list<Dynamic*>& objects, std::string map)
    {
        if (map == "village") {
            Dynamic_Creature *carl = new Dynamic_Creature("carl", Assets::GetInstance().GetSprite("purple"));
            carl->px = 6;
            carl->py = 4;
            carl->Friendly = true;
            objects.push_back(carl);
        }
        else if (map == "home") {
            Dynamic_Creature *bob = new Dynamic_Creature("bob", Assets::GetInstance().GetSprite("skelly"));
            bob->px = 11;
            bob->py = 4;
            bob->Friendly = true;
            objects.push_back(bob);
        }
    }


    bool Quest_MainQuest::OnInteraction(std::list<Dynamic*> objects, Dynamic * target, QuestNature nature)
    {
        if (target->Name == "carl") {
            Script->AddCommand(new Command_Say({ "[Carl]", "You have no", "additional quests!" }));
            if (Engine->HasItem(Assets::GetInstance().GetItem("healthboost"))) {
                Script->AddCommand(new Command_Say({ "[Carl]", "Yay! You do have", "a health boost." }));
            }
            else {
                Script->AddCommand(new Command_Say({ "[Carl]", "Boo! You don't have", "a health boost." }));
            }
            return true;
        }
        if (target->Name == "bob") {
            Script->AddCommand(new Command_Say({ "[Bob]", "I need you to", "do something for me!" }));
            Script->AddCommand(new Command_Say({ "[Bob]", "Predictably, there are", "rats on my basement!" }));
            Engine->AddQuest(new Quest_BobsQuest());
            return true;
        }
        return false;
    }


    bool Quest_BobsQuest::OnInteraction(std::list<Dynamic*> objects, Dynamic * target, QuestNature nature)
    {
        if (target->Name == "carl") {
            Script->AddCommand(new Command_Say({ "[Carl]", "You are doing", "Bob's quest!" }));
            return true;
        }
        return false;
    }

}