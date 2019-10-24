#include "olcConsoleGameEngine.h"
#include <vector>
#include <utility>

using namespace std;

class FroggerGame : public olcConsoleGameEngine
{
    vector<pair<float, string>> Lanes;
    float Time;
    olcSprite sprBus;
    olcSprite sprCar1;
    olcSprite sprCar2;
    olcSprite sprFrog;
    olcSprite sprHome;
    olcSprite sprLog;
    olcSprite sprPavement;
    olcSprite sprWall;
    olcSprite sprWater;

    virtual bool OnUserCreate() override
    {
        sprBus.Load(L"FroggerSprites/bus.spr");
        sprCar1.Load(L"FroggerSprites/car1.spr");
        sprCar2.Load(L"FroggerSprites/car2.spr");
        sprFrog.Load(L"FroggerSprites/frog.spr");
        sprHome.Load(L"FroggerSprites/home.spr");
        sprLog.Load(L"FroggerSprites/log.spr");
        sprPavement.Load(L"FroggerSprites/pavement.spr");
        sprWall.Load(L"FroggerSprites/wall.spr");
        sprWater.Load(L"FroggerSprites/water.spr");

        Lanes.push_back(make_pair(+0.0f, "wwwhhwwwhhwwwhhwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"));
        Lanes.push_back(make_pair(-2.0f, ",,<>,,,,<####>,,,,,,<#>,<>,,,<>,,,,,,,<>,,,<#>,,,,,,,,<>,,<>,,,,"));
        Lanes.push_back(make_pair(+3.0f, ",<##>,<>,,,<##>,,<>,,,,,<####>,,,,,<#>,,<###>,,,,<>,<>,,,<##>,,,"));
        Lanes.push_back(make_pair(-4.0f, ",,,,<>,,<#>,,,,,,,<>,<#>,,,,,,,,,<#>,,,<>,,,,<####>,,,,,<#>,,,<#"));
        Lanes.push_back(make_pair(+0.0f, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
        Lanes.push_back(make_pair(-2.0f, "...db....db.......db...db.......db......db.db......db.....db..db"));
        Lanes.push_back(make_pair(-4.0f, ".1234.....1234....1234.1234.......1234.....1234.....1234...1234."));
        Lanes.push_back(make_pair(+3.0f, "...qp....qp....qp......qp......qp.......qp......qp.qp...qp...qp."));
        Lanes.push_back(make_pair(-2.0f, "..db...db......db.....db....db.db...db.....db......db....db.db.."));
        Lanes.push_back(make_pair(+0.0f, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');
        Time += fElapsedTime;
        int nRow = 0;
        for (auto &l : Lanes) {
            int nCol = 0;
            for (int col = 0; col < 16; col++) {
                int pos = (int)(col + Time * l.first) % 64;
                if (pos < 0) {
                    pos = 64 - abs(pos);
                }                
                olcSprite *sprite = nullptr;
                int spriteTile = 0;
                switch (l.second[pos]) {
                case 'w': sprite = &sprWall; break;
                case 'x': sprite = &sprPavement; break;
                case 'h': sprite = &sprHome; break;
                case ',': sprite = &sprWater; break;
                case '<': sprite = &sprLog; spriteTile = 0; break;
                case '#': sprite = &sprLog; spriteTile = 1; break;
                case '>': sprite = &sprLog; spriteTile = 2; break;
                case '1': sprite = &sprBus; spriteTile = 0; break;
                case '2': sprite = &sprBus; spriteTile = 1; break;
                case '3': sprite = &sprBus; spriteTile = 2; break;
                case '4': sprite = &sprBus; spriteTile = 3; break;
                case 'd': sprite = &sprCar1; spriteTile = 0; break;
                case 'b': sprite = &sprCar1; spriteTile = 1; break;
                case 'q': sprite = &sprCar2; spriteTile = 0; break;
                case 'p': sprite = &sprCar2; spriteTile = 1; break;
                default: sprite = nullptr;
                }
                DrawPartialSprite(col * 8, nRow * 8, sprite, spriteTile * 8, 0, 8, 8);
            }
            nRow++;
        }
        return true;
    }
};

int main()
{
    FroggerGame game;
    game.ConstructConsole(128, 80, 12, 12);
    game.Start();

    return 0;
}