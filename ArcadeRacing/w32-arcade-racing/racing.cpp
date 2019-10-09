#include "olcConsoleGameEngine.h"

class RacingGame : public olcConsoleGameEngine
{
    // Inherited via olcConsoleGameEngine
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
    RacingGame game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

    return 0;
}