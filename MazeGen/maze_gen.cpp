#define OLC_PGE_APPLICATION
#include "../olcPixelGameEngine.h"
#include <stack>
#include <utility>

using namespace std;

class MazeGen : public olc::PixelGameEngine
{
    int m_nMazeWidth;
    int m_nMazeHeight;
    int m_nTotal;
    int m_nVisited;
    int *m_maze;
    stack<pair<int, int>> m_stack;

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

        memset(m_maze, 0, m_nTotal * sizeof(int));

        m_stack.push(make_pair(0, 0));
        m_nVisited = 1;

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        chrono::milliseconds sleep_time(5);
        chrono::duration<float> elapsed(fElapsedTime);

        //this_thread::sleep_for(sleep_time - elapsed);
        int nTopX = 0;
        int nTopY = 0;

        if (!m_stack.empty()) {
            nTopX = m_stack.top().first;
            nTopY = m_stack.top().second;
            m_maze[nTopY * m_nMazeWidth + nTopX] |= MAZE_VISITED;

            // Check non-visited neighbors
            vector<int> neighbors;
            if (nTopY > 0 && !(m_maze[(nTopY - 1)* m_nMazeWidth + nTopX] & MAZE_VISITED)) {
                neighbors.push_back(MAZE_NORTH);
            }
            if (nTopY < m_nMazeHeight - 1 && !(m_maze[(nTopY + 1) * m_nMazeWidth + nTopX] & MAZE_VISITED)) {
                neighbors.push_back(MAZE_SOUTH);
            }
            if (nTopX > 0 && !(m_maze[nTopY * m_nMazeWidth + nTopX - 1] & MAZE_VISITED)) {
                neighbors.push_back(MAZE_WEST);
            }
            if (nTopX < m_nMazeWidth - 1 && !(m_maze[nTopY * m_nMazeWidth + nTopX + 1] & MAZE_VISITED)) {
                neighbors.push_back(MAZE_EAST);
            }

            if (!neighbors.empty()) {
                int nNeighborDirection = neighbors.at(rand() % neighbors.size());
                switch (nNeighborDirection) {
                case MAZE_NORTH:
                    m_stack.push(make_pair(nTopX, nTopY - 1));
                    m_maze[nTopY * m_nMazeWidth + nTopX] |= MAZE_NORTH;
                    m_maze[(nTopY - 1) * m_nMazeWidth + nTopX] |= MAZE_SOUTH;
                    break;
                case MAZE_SOUTH:
                    m_stack.push(make_pair(nTopX, nTopY + 1));
                    m_maze[nTopY * m_nMazeWidth + nTopX] |= MAZE_SOUTH;
                    m_maze[(nTopY + 1) * m_nMazeWidth + nTopX] |= MAZE_NORTH;
                    break;
                case MAZE_WEST:
                    m_stack.push(make_pair(nTopX - 1, nTopY));
                    m_maze[nTopY * m_nMazeWidth + nTopX] |= MAZE_WEST;
                    m_maze[nTopY * m_nMazeWidth + nTopX - 1] |= MAZE_EAST;
                    break;
                case MAZE_EAST:
                    m_stack.push(make_pair(nTopX + 1, nTopY));
                    m_maze[nTopY * m_nMazeWidth + nTopX] |= MAZE_EAST;
                    m_maze[nTopY * m_nMazeWidth + nTopX + 1] |= MAZE_WEST;
                    break;
                }
                m_nVisited++;
            }
            else {
                m_stack.pop();
            }
        }

        // Render maze to screen
        int nBlockSize = 4;
        for (int y = 0; y < m_nMazeHeight; y++) {
            for (int x = 0; x < m_nMazeWidth; x++) {
                int nMazePos = y * m_nMazeWidth + x;
                for (int py = 0; py < nBlockSize; py++) {
                    for (int px = 0; px < nBlockSize; px++) {
                        olc::Pixel color = olc::DARK_BLUE;
                        if (py == nBlockSize - 1 && px == nBlockSize - 1) {
                            color = olc::BLACK;
                        }
                        else if (py == nBlockSize - 1 && !(m_maze[nMazePos] & MAZE_SOUTH)) {
                            color = olc::BLACK;
                        }
                        else if (px == nBlockSize - 1 && !(m_maze[nMazePos] & MAZE_EAST)) {
                            color = olc::BLACK;
                        }
                        else if (m_maze[nMazePos] & MAZE_VISITED) {
                            color = (x == nTopX && y == nTopY) ? olc::GREEN : olc::WHITE;
                        }
                        else {
                            color = olc::DARK_BLUE;
                        }
                        Draw(x * nBlockSize + px, y * nBlockSize + py, color);
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
    if (maze.Construct(160, 100, 8, 8))
        maze.Start();

    return 0;
}