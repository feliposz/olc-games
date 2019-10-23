#include "olcConsoleGameEngine.h"

class FroggerGame : public olcConsoleGameEngine
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
    FroggerGame game;
    game.ConstructConsole(120, 80, 12, 12);
    game.Start();

    return 0;
}