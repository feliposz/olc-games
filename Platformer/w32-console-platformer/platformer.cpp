#include "olcConsoleGameEngine.h"
using namespace std;

class PlatformerGame : public olcConsoleGameEngine
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
    PlatformerGame game;
    game.ConstructConsole(160, 120, 6, 6);
    game.Start();

    return 0;
}