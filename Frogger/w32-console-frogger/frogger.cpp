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

        Lanes.push_back(make_pair(+0.0f, "xxx..xxx..xxx..xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
        Lanes.push_back(make_pair(+2.0f, "..xx....xxxxxx......xxx.xx...xx.......xx...xxx........xx..xx...."));
        Lanes.push_back(make_pair(-3.0f, ".xxxx.xx...xxxx..xx.....xxxxxx.....xxx..xxxxx....xx.xx...xxxx..."));
        Lanes.push_back(make_pair(+4.0f, "....xx..xxx.......xx.xxx.........xxx...xx....xxxxxx.....xxx...xx"));
        Lanes.push_back(make_pair(+0.0f, "................................................................"));
        Lanes.push_back(make_pair(+2.0f, "...xx....xx.......xx...xx.......xx......xx.xx......xx.....xx..xx"));
        Lanes.push_back(make_pair(+4.0f, ".xxxx.....xxxx....xxxx.xxxx.......xxxx.....xxxx.....xxxx...xxxx."));
        Lanes.push_back(make_pair(-3.0f, "...xx....xx....xx......xx......xx.......xx......xx.xx...xx...xx."));
        Lanes.push_back(make_pair(+2.0f, "..xx...xx......xx.....xx....xx.xx...xx.....xx......xx....xx.xx.."));
        Lanes.push_back(make_pair(+0.0f, "................................................................"));

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');
        Time += fElapsedTime;
        int nRow = 0;
        for (auto &l : Lanes) {
            int nCol = 0;
            for (int x = 0; x < 128; x++) {
                int pos = (int)(x/8.0 + Time * l.first) % 64;
                if (pos < 0) {
                    pos = 64 - abs(pos);
                }
                for (int y = 0; y < 8; y++) {
                    Draw(x, y + nRow * 8, l.second[pos] == 'x' ? 'X' : '.');
                }
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