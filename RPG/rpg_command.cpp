#include "rpg_command.h"
#include "rpg_engine.h"

namespace Rpg {

    GameEngine *Command::Engine = nullptr;

    void ScriptProcessor::AddCommand(Command *cmd)
    {
        m_queue.emplace_back(cmd);
    }

    void ScriptProcessor::ProcessCommands(float elapsed)
    {
        if (!m_queue.empty()) {
            UserControlEnabled = false;
            Command *cmd = m_queue.front();
            if (!cmd->Started) {
                cmd->Started = true;
                cmd->Start();
            }
            cmd->Update(elapsed);
            if (cmd->Completed) {
                delete cmd;
                m_queue.pop_front();
            }
        }
        else {
            UserControlEnabled = true;
        }
    }

    void ScriptProcessor::CompleteCommand()
    {
        if (!m_queue.empty()) {
            m_queue.front()->Completed = true;
        }
    }

    Command_WalkTo::Command_WalkTo(Dynamic *object, float x, float y, float duration)
    {
        m_object = object;
        m_targetX = x;
        m_targetY = y;
        m_duration = duration;
    }

    void Command_WalkTo::Start()
    {
        Started = true;
        m_time = 0;
        m_startX = m_object->px;
        m_startY = m_object->py;
    }

    void Command_WalkTo::Update(float elapsed)
    {
        m_time += elapsed;
        if (m_time >= m_duration) {
            Completed = true;
            m_time = m_duration;
        }
        m_object->px = (m_targetX - m_startX) * (m_time / m_duration) + m_startX;
        m_object->py = (m_targetY - m_startY) * (m_time / m_duration) + m_startY;
    }

    Command_Wait::Command_Wait(float duration)
    {
        m_duration = duration;
    }

    void Command_Wait::Start()
    {
        m_time = 0;
    }

    void Command_Wait::Update(float elapsed)
    {
        m_time += elapsed;
        if (m_time >= m_duration) {
            Completed = true;
            m_time = m_duration;
        }
    }

    Command_Say::Command_Say(std::vector<std::string> content)
    {
        m_content = content;
    }

    void Command_Say::Start()
    {
        Engine->ShowDialog(m_content);
    }

    Command_ChangeMap::Command_ChangeMap(std::string map, float x, float y)
    {
        m_map = map;
        m_x = x;
        m_y = y;
    }

    void Command_ChangeMap::Start()
    {
        Engine->ChangeMap(m_map, m_x, m_y);
        Completed = true;
    }

}