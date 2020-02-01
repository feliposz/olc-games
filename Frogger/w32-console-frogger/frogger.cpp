#include "olcConsoleGameEngine.h"
#include <vector>
#include <utility>

using namespace std;

const int CellSize = 8;

class FroggerGame : public olcConsoleGameEngine
{
    vector<pair<float, string>> Lanes;
    wchar_t *Collision;
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
    float FrogX;
    float FrogY;

    virtual bool OnUserCreate() override
    {
        FrogX = 8;
        FrogY = 9;
        sprBus.Load(L"FroggerSprites/bus.spr");
        sprCar1.Load(L"FroggerSprites/car1.spr");
        sprCar2.Load(L"FroggerSprites/car2.spr");
        sprFrog.Load(L"FroggerSprites/frog.spr");
        sprHome.Load(L"FroggerSprites/home.spr");
        sprLog.Load(L"FroggerSprites/log.spr");
        sprPavement.Load(L"FroggerSprites/pavement.spr");
        sprWall.Load(L"FroggerSprites/wall.spr");
        sprWater.Load(L"FroggerSprites/water.spr");

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
        Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');
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

                olcSprite *sprite = nullptr;
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
        
        if (m_keys[VK_UP].bPressed && FrogY > 0) FrogY -= 1;
        if (m_keys[VK_DOWN].bPressed && FrogY < 9) FrogY += 1;
        if (m_keys[VK_LEFT].bPressed && FrogX > 0) FrogX -= 1;
        if (m_keys[VK_RIGHT].bPressed && FrogX < 15) FrogX += 1;

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

        if (m_keys[VK_SPACE].bHeld) {
            int size = ScreenWidth() * ScreenHeight();
            for (int i = 0; i < size; i++) {
                m_bufScreen[i].Char.UnicodeChar = Collision[i];
            }
        }

        DrawSprite(FrogX * CellSize, FrogY * CellSize, &sprFrog);

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