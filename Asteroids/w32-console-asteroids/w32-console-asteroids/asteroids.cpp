#include "olcConsoleGameEngine.h"

class AsteroidsGame : public olcConsoleGameEngine
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
    AsteroidsGame game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

    return 0;
}