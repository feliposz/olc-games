#include "olcConsoleGameEngine.h"

class MazeGen : public olcConsoleGameEngine
{
    int m_nMazeWidth;
    int m_nMazeHeight;
    int m_nTotal;
    int m_nVisited;
    int *m_maze;

public:

    enum {
        MAZE_NORTH = 0x01,
        MAZE_SOUTH = 0x02,
        MAZE_EAST = 0x04,
        MAZE_WEST = 0x08,
        MAZE_VISITED = 0x10
    };

    // Inherited via olcConsoleGameEngine
    virtual bool OnUserCreate() override
    {
        m_nMazeWidth = 40;
        m_nMazeHeight = 25;
        m_nTotal = m_nMazeWidth * m_nMazeHeight;
        m_maze = new int[m_nTotal];
        m_nVisited = 0;

        memset(m_maze, 0, m_nTotal * sizeof(int));

        m_maze[0] = MAZE_VISITED;

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {

        // Render maze to screen
        int nBlockSize = 4;
        for (int y = 0; y < m_nMazeHeight; y++) {
            for (int x = 0; x < m_nMazeWidth; x++) {
                int nMazePos = y * m_nMazeWidth + x;
                for (int py = 0; py < nBlockSize; py++) {
                    for (int px = 0; px < nBlockSize; px++) {
                        short color = FG_DARK_BLUE;
                        if (py == nBlockSize - 1 && !(m_maze[nMazePos] & MAZE_SOUTH)) {
                            color = FG_BLACK;
                        }
                        else if (px == nBlockSize - 1 && !(m_maze[nMazePos] & MAZE_EAST)) {
                            color = FG_BLACK;
                        }
                        else if (m_maze[nMazePos] & m_nVisited) {
                            color = FG_WHITE;
                        }
                        else {
                            color = FG_DARK_BLUE;
                        }
                        Draw(x * nBlockSize + px, y * nBlockSize + py, PIXEL_SOLID, color);
                    }
                }
            }
        }

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