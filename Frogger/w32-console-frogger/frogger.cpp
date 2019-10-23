#include "olcConsoleGameEngine.h"
#include <vector>
#include <utility>

using namespace std;

class FroggerGame : public olcConsoleGameEngine
{
    vector<pair<float, string>> Lanes;

    virtual bool OnUserCreate() override
    {
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