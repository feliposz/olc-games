#include "olcConsoleGameEngine.h"

class MazeGen : public olcConsoleGameEngine
{
    int m_nMazeWidth;
    int m_nMazeHeight;
    int m_nTotal;
    int m_nVisited;
    int *m_maze;

public:

    // Inherited via olcConsoleGameEngine
    virtual bool OnUserCreate() override
    {
        m_nMazeWidth = 40;
        m_nMazeHeight = 25;
        m_nTotal = m_nMazeWidth * m_nMazeHeight;
        m_maze = new int[m_nTotal];
        m_nVisited = 0;

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        return true;
    }
};

int main()
{
    MazeGen maze;
    maze.ConstructConsole(160, 100, 8, 8);
    maze.Start();

    return 0;
}