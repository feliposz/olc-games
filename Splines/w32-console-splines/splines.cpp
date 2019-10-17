#include "olcConsoleGameEngine.h"

class CatmullRomSplines : public olcConsoleGameEngine
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
    CatmullRomSplines game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

    return 0;
}