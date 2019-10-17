#include "olcConsoleGameEngine.h"
#include <string>
#include <vector>
#include <utility>
using namespace std;

class CatmullRomSplines : public olcConsoleGameEngine
{
    const int MaxPoints = 8;
    const int Padding = 10;
    const float ControlPointSpeed = 20.0f;

    vector<pair<float, float>> Points;
    int Selected = 0;

    pair<float, float> Interpolate(float t)
    {        
        int p1 = ((int)t) % MaxPoints;
        int p2 = (p1 + 1) % MaxPoints;
        int p3 = (p2 + 1) % MaxPoints;
        int p0 = (p1 - 1) >= 0 ? p1 - 1 : MaxPoints - 1;
        t = t - (int)t; // Decimal part
        float t2 = t * t;
        float t3 = t2 * t;
        // Based on the original Catmull-Rom spline but modified by javidx9
        float q0 = -t3 + 2.0f * t2 - t; 
        float q1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
        float q2 = -3.0f * t3 + 4.0f * t2 + t;
        float q3 = t3 - t2;
        float x = 0.5f * (q0 * Points[p0].first + q1 * Points[p1].first + q2 * Points[p2].first + q3 * Points[p3].first);
        float y = 0.5f * (q0 * Points[p0].second + q1 * Points[p1].second + q2 * Points[p2].second + q3 * Points[p3].second);
        return { x , y };
    }

    virtual bool OnUserCreate() override
    {
        for (int i = 0; i < MaxPoints; i++) {
            Points.emplace_back(make_pair(Padding + i * ((ScreenWidth() - Padding)/MaxPoints), 20 * (i%2) + ScreenHeight() / 2));
        }

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        // Change selected control point
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

        // Move selected control point
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

        // Clear Screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');

        // Draw curve
        for (float t = 0; t < Points.size(); t += 0.001) {
            pair<float, float> p = Interpolate(t);
            Draw(p.first, p.second, PIXEL_SOLID, FG_WHITE);
        }

        // Draw control points
        int n = 0;        
        for (auto p : Points) {
            Fill(p.first-1, p.second-1, p.first + 2, p.second + 2, PIXEL_SOLID, n == Selected ? FG_YELLOW : FG_RED);
            DrawString(p.first, p.second, to_wstring(n), FG_WHITE);
            n++;
        }
        return true;
    }
};

int main()
{
    CatmullRomSplines game;
    game.ConstructConsole(120, 80, 10, 10);
    game.Start();

    return 0;
}