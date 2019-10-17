#include "olcConsoleGameEngine.h"
#include <string>
#include <vector>
#include <utility>
using namespace std;

class CatmullRomSplines : public olcConsoleGameEngine
{
    const int MaxPoints = 5;
    const int Padding = 10;
    const int ControlSize = 3;
    const int ControlPointSpeed = 20.0f;

    vector<pair<float, float>> Points;
    int Selected = 0;

    virtual bool OnUserCreate() override
    {
        for (int i = 0; i < MaxPoints; i++) {
            Points.emplace_back(make_pair(Padding + i * ((ScreenWidth() - Padding)/MaxPoints), ScreenHeight() / 2));
        }

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        if (m_keys['Z'].bReleased) {
            Selected--;
            if (Selected < 0) {
                Selected = MaxPoints - 1;
            }
        }
        if (m_keys['X'].bReleased) {
            Selected++;
            if (Selected >= MaxPoints) {
                Selected = 0;
            }
        }

        if (m_keys[VK_LEFT].bHeld) {
            Points[Selected].first -= ControlPointSpeed * fElapsedTime;
        }
        if (m_keys[VK_RIGHT].bHeld) {
            Points[Selected].first += ControlPointSpeed * fElapsedTime;
        }
        if (m_keys[VK_UP].bHeld) {
            Points[Selected].second -= ControlPointSpeed * fElapsedTime;
        }
        if (m_keys[VK_DOWN].bHeld) {
            Points[Selected].second += ControlPointSpeed * fElapsedTime;
        }

        Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');
        int n = 0;        
        for (auto p : Points) {
            Fill(p.first, p.second, p.first + ControlSize, p.second + ControlSize, PIXEL_SOLID, n == Selected ? FG_YELLOW : FG_RED);
            DrawString(p.first + ControlSize / 2, p.second + ControlSize / 2, to_wstring(n), FG_WHITE);
            n++;
        }
        return true;
    }
};

int main()
{
    CatmullRomSplines game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

    return 0;
}