#pragma once

#include <vector>
#include "rpg_dynamic.h"

namespace Rpg {

    class Command {
    public:
        bool Started = false;
        bool Completed = false;

        virtual void Start() {};
        virtual void Update(float elapsed) {};
        virtual void OnComplete() {}
    };

    class ScriptProcessor {

    protected:
        std::list<Command *> m_queue;

    public:
        bool UserControlEnabled = true;

        void AddCommand(Command *cmd);
        void ProcessCommands(float elapsed);
        void CompleteCommand();
    };

    class Command_WalkTo : public Command {
    public:
        Command_WalkTo(Dynamic *object, float x, float y, float duration);
        virtual void Start() override;
        virtual void Update(float elapsed) override;
    private:
        Dynamic * m_object;
        float m_startX, m_startY;
        float m_targetX, m_targetY;
        float m_time;
        float m_duration;
    };

    class Command_Wait : public Command {
    public:
        Command_Wait(float duration);
        virtual void Start() override;
        virtual void Update(float elapsed) override;
    private:
        float m_time;
        float m_duration;
    };
    
    class Command_Say : public Command {
    public:
        static bool ShowDialog;
        static std::vector<std::string> DialogContent;
        Command_Say(std::vector<std::string> content);
        virtual void Start() override;
        virtual void OnComplete() override;
    private:
        std::vector<std::string> m_content;
    };

}