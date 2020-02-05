#include "olcConsoleGameEngine.h"
using namespace std;

class Game : public olcConsoleGameEngine
{
    float *RandomSeed = nullptr;
    float *RandomNoise = nullptr;
    int Size = 0;

    virtual bool OnUserCreate() override
    {
        Size = ScreenWidth();
        RandomSeed = new float[Size];
        RandomNoise = new float[Size];

        for (int i = 0; i < Size; i++) {
            RandomSeed[i] = (float)rand() / RAND_MAX;
        }

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

        // Test
        for (int i = 0; i < Size; i++) {
            RandomNoise[i] = RandomSeed[i];
        }

        for (int x = 0; x < ScreenWidth(); x++) {
            int y = (int)(RandomNoise[x] * ScreenHeight() / 2);
            DrawLine(x, ScreenHeight() / 2 - y, x, ScreenHeight() / 2, PIXEL_SOLID, FG_GREEN);
        }

        return true;
    }
};

int main()
{
    Game game;
    game.ConstructConsole(256, 256, 3, 3);
    game.Start();

    return 0;
}