#include "olcConsoleGameEngine.h"
using namespace std;

class Game : public olcConsoleGameEngine
{

    virtual bool OnUserCreate() override
    {
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
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