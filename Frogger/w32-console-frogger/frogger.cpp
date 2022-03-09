#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <utility>

using namespace std;

const int CellSize = 8;

class FroggerGame : public olc::PixelGameEngine
{
    vector<pair<float, string>> Lanes;
    wchar_t *Collision;
    float Time;
    olc::Sprite sprBus;
    olc::Sprite sprCar1;
    olc::Sprite sprCar2;
    olc::Sprite sprFrog;
    olc::Sprite sprHome;
    olc::Sprite sprLog;
    olc::Sprite sprPavement;
    olc::Sprite sprWall;
    olc::Sprite sprWater;
    float FrogX;
    float FrogY;

    virtual bool OnUserCreate() override
    {
        FrogX = 8;
        FrogY = 9;
        sprBus.LoadFromFile("assets/bus.png");
        sprCar1.LoadFromFile("assets/car1.png");
        sprCar2.LoadFromFile("assets/car2.png");
        sprFrog.LoadFromFile("assets/frog.png");
        sprHome.LoadFromFile("assets/home.png");
        sprLog.LoadFromFile("assets/log.png");
        sprPavement.LoadFromFile("assets/pavement.png");
        sprWall.LoadFromFile("assets/wall.png");
        sprWater.LoadFromFile("assets/water.png");

        Lanes.push_back(make_pair(+0.0f, "wwhhwwwhhwwwhhwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"));
        Lanes.push_back(make_pair(-2.0f, ",,<>,,,,<####>,,,,,,<#>,<>,,,<>,,,,,,,<>,,,<#>,,,,,,,,<>,,<>,,,,"));
        Lanes.push_back(make_pair(+3.0f, ",<##>,<>,,,<##>,,<>,,,,,<####>,,,,,<#>,,<###>,,,,<>,<>,,,<##>,,,"));
        Lanes.push_back(make_pair(-4.0f, ",,,,<>,,<#>,,,,,,,<>,<#>,,,,,,,,,<#>,,,<>,,,,<####>,,,,,<#>,,,<#"));
        Lanes.push_back(make_pair(+0.0f, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
        Lanes.push_back(make_pair(-2.0f, "...db....db.......db...db.......db......db.db......db.....db..db"));
        Lanes.push_back(make_pair(-4.0f, ".1234.....1234....1234.1234.......1234.....1234.....1234...1234."));
        Lanes.push_back(make_pair(+3.0f, "...qp....qp....qp......qp......qp.......qp......qp.qp...qp...qp."));
        Lanes.push_back(make_pair(-2.0f, "..db...db......db.....db....db.db...db.....db......db....db.db.."));
        Lanes.push_back(make_pair(+0.0f, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));

        int size = ScreenWidth() * ScreenHeight();
        Collision = new wchar_t[size];        

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);
        Time += fElapsedTime;
        int nRow = 0;
        for (auto &l : Lanes) {
            int nCol = 0;
            for (int col = -1; col < 18; col++) {
                float realPos = Time * l.first;

                int pos = (int)(col + realPos) % 64;
                if (pos < 0) {
                    pos = 64 - abs(pos);
                }
                int partial = (int)(realPos * CellSize) % CellSize;

                olc::Sprite *sprite = nullptr;
                int spriteTile = 0;
                bool solid = true;
                switch (l.second[pos]) {
                case 'w': sprite = &sprWall; break;
                case 'x': sprite = &sprPavement; solid = false; break;
                case 'h': sprite = &sprHome; solid = false; break;
                case ',': sprite = &sprWater; break;
                case '<': sprite = &sprLog; spriteTile = 0; solid = false; break;
                case '#': sprite = &sprLog; spriteTile = 1; solid = false; break;
                case '>': sprite = &sprLog; spriteTile = 2; solid = false; break;
                case '1': sprite = &sprBus; spriteTile = 0; break;
                case '2': sprite = &sprBus; spriteTile = 1; break;
                case '3': sprite = &sprBus; spriteTile = 2; break;
                case '4': sprite = &sprBus; spriteTile = 3; break;
                case 'd': sprite = &sprCar1; spriteTile = 0; break;
                case 'b': sprite = &sprCar1; spriteTile = 1; break;
                case 'q': sprite = &sprCar2; spriteTile = 0; break;
                case 'p': sprite = &sprCar2; spriteTile = 1; break;
                default: sprite = nullptr; solid = false; break;
                }

                DrawPartialSprite(col * CellSize - partial, nRow * CellSize, sprite, spriteTile * CellSize, 0, CellSize, CellSize);

                // Fill collision grid
                for (int y = nRow * CellSize; y < (1 + nRow) * CellSize; y++) {
                    for (int x = col * CellSize - partial; x < (1 + col) * CellSize - partial; x++) {
                        if (x >= 0 && x < ScreenWidth()) {
                            Collision[ScreenWidth() * y + x] = solid ? L'#' : L'.';
                        }
                    }
                }
            }
            nRow++;
        }
        
        if (GetKey(olc::UP).bPressed && FrogY > 0) FrogY -= 1;
        if (GetKey(olc::DOWN).bPressed && FrogY < 9) FrogY += 1;
        if (GetKey(olc::LEFT).bPressed && FrogX > 0) FrogX -= 1;
        if (GetKey(olc::RIGHT).bPressed && FrogX < 15) FrogX += 1;

        if (FrogY < 4) {
             FrogX -= fElapsedTime * Lanes[(int)FrogY].first;
        }

        int left = ScreenWidth() * (int)(FrogY * CellSize) + (int)(FrogX * CellSize) + 1;
        int right = ScreenWidth() * (int)(FrogY * CellSize) + (int)(FrogX * CellSize) + CellSize - 2;

        if (Collision[left] == L'#' || Collision[right] == L'#') {
            // Frog collided
            FrogX = 8;
            FrogY = 9;
        }

        /*
        if (GetKey(olc::SPACE].bHeld) {
            int size = ScreenWidth() * ScreenHeight();
            for (int i = 0; i < size; i++) {
                m_bufScreen[i].Char.UnicodeChar = Collision[i];
            }
        }
        */

        SetPixelMode(olc::Pixel::MASK);
        DrawSprite(FrogX * CellSize, FrogY * CellSize, &sprFrog);
        SetPixelMode(olc::Pixel::NORMAL);

        return true;
    }
};

int main()
{
    FroggerGame game;
    if (game.Construct(128, 80, 12, 12))
       game.Start();

    return 0;
}