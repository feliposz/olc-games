#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class GameOfLife : public olc::PixelGameEngine {

    int *m_state;
    int *m_output;
    int m_nScreenWidth;
    int m_nScreenHeight;

    // Inherited via olcConsoleGameEngine
    virtual bool OnUserCreate() override
    {
        m_nScreenWidth = ScreenWidth();
        m_nScreenHeight = ScreenHeight();
        int nSize = m_nScreenWidth * m_nScreenHeight;
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
        int nSize = m_nScreenWidth * m_nScreenHeight;
        for (int i = 0; i < nSize; i++) {
            m_state[i] = m_output[i];
            if (rand() % (nSize*10) == 0) { // Random mutation!
               // m_state[i] = !m_state[i];
            }
        }

        auto cell = [&](int x, int y) {
            if (x < 0) x = m_nScreenWidth - 1;
            if (y < 0) y = m_nScreenHeight - 1;
            if (x == m_nScreenWidth) x = 0;
            if (y == m_nScreenHeight) y = 0;
            return m_state[y * m_nScreenWidth + x];
        };

        for (int y = 0; y < m_nScreenHeight; y++) {
            for (int x = 0; x < m_nScreenWidth; x++) {
                int nNeighbors =
                    cell(x - 1, y - 1) + cell(x, y - 1) + cell(x + 1, y - 1) +
                    cell(x - 1, y) + 0 + cell(x + 1, y) +
                    cell(x - 1, y + 1) + cell(x, y + 1) + cell(x + 1, y + 1);

                if (cell(x, y)) {
                    m_output[y * m_nScreenWidth + x] = nNeighbors == 2 || nNeighbors == 3;
                }
                else {
                    m_output[y * m_nScreenWidth + x] = nNeighbors == 3;
                }

                if (m_output[y * m_nScreenWidth + x] == m_state[y * m_nScreenWidth + x]) {
                    Draw(x, y, m_output[y * m_nScreenWidth + x] ? olc::WHITE : olc::BLACK);
                }
                else {
                    Draw(x, y, m_output[y * m_nScreenWidth + x] ? olc::YELLOW : olc::DARK_GREY);
                }
            }
        }

        return true;
    }
};

int main() 
{
    GameOfLife game;
    if (game.Construct(100, 100, 8, 8))
        game.Start();

    return 0;
}