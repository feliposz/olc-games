#include "olcConsoleGameEngine.h"

class GameOfLife : public olcConsoleGameEngine {

    int *m_state;
    int *m_output;

    // Inherited via olcConsoleGameEngine
    virtual bool OnUserCreate() override
    {
        int nSize = ScreenWidth() * ScreenHeight();
        m_state = new int[nSize];
        m_output = new int[nSize];
        memset(m_state, 0, sizeof(int) * nSize);
        memset(m_output, 0, sizeof(int) * nSize);

        for (int i = 0; i < nSize; i++) {
            m_output[i] = rand() % 2;
        }

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        int nSize = ScreenWidth() * ScreenHeight();
        for (int i = 0; i < nSize; i++) {
            m_state[i] = m_output[i];
        }

        auto cell = [&](int x, int y) {
            return m_state[y * ScreenWidth() + x];
        };

        for (int y = 1; y < ScreenHeight()-1; y++) {
            for (int x = 1; x < ScreenWidth()-1; x++) {
                int nNeighbors =
                    cell(x - 1, y - 1) + cell(x, y - 1) + cell(x + 1, y - 1) +
                    cell(x - 1, y) + 0 + cell(x + 1, y) +
                    cell(x - 1, y + 1) + cell(x, y + 1) + cell(x + 1, y + 1);

                if (cell(x, y)) {
                    m_output[y * ScreenWidth() + x] = nNeighbors == 2 || nNeighbors == 3;
                }
                else {
                    m_output[y * ScreenWidth() + x] = nNeighbors == 3;
                }

                Draw(x, y, PIXEL_SOLID, m_output[y * ScreenWidth() + x] ? FG_WHITE : FG_BLACK);
            }
        }

        return true;
    }
};

int main() 
{
    GameOfLife game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

    return 0;
}