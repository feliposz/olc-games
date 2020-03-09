#include "rpg_engine.h"

int main()
{
    Rpg::GameEngine game;
    game.Construct(256, 240, 4, 4);
    game.SetPixelMode(olc::Pixel::MASK);
    game.Start();

    return 0;
}