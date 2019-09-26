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
            m_state[i] = rand() % 2;
        }

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        int nSize = ScreenWidth() * ScreenHeight();
        for (int i = 0; i < nSize; i++) {
            m_output[i] = m_state[i];
        }

        for (int y = 0; y < ScreenHeight(); y++) {
            for (int x = 0; x < ScreenWidth(); x++) {
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