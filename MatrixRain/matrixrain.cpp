#include "olcConsoleGameEngine.h"

class MatrixRain : public olcConsoleGameEngine
{
    virtual bool OnUserCreate() override
    {
        return true;
    }
    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        return true;
    }
};

int main()
{
    MatrixRain game;
    game.ConstructConsole(128, 80, 12, 12);
    game.Start();
    return 0;
}
