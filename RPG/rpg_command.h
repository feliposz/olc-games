#pragma once

#include <vector>
#include "rpg_dynamic.h"

namespace Rpg {

    class GameEngine;

    class Command {
    public:
        bool Started = false;
        bool Completed = false;

        static GameEngine* Engine;

        virtual void Start() {};
        virtual void Update(float elapsed) {};
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
        Command_Say(std::vector<std::string> content);
        virtual void Start() override;
    private:
        std::vector<std::string> m_content;
    };

    class Command_ChangeMap : public Command {
    public:
        Command_ChangeMap(std::string map, float x, float y);
        virtual void Start() override;
    private:
        std::string m_map;
        float m_x;
        float m_y;
    };


}