#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <string>
#include <vector>
#include <utility>
using namespace std;

struct Point {
    float x, y, length;
};

class CatmullRomSplines : public olc::PixelGameEngine
{
    const int MaxPoints = 10;
    const float ControlPointSpeed = 20.0f;

    vector<Point> Points;
    int Selected = 0;
    float CurrentOffset = 0;
    float TotalLength = 0;

    Point Interpolate(float t)
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
        float x = 0.5f * (q0 * Points[p0].x + q1 * Points[p1].x + q2 * Points[p2].x + q3 * Points[p3].x);
        float y = 0.5f * (q0 * Points[p0].y + q1 * Points[p1].y + q2 * Points[p2].y + q3 * Points[p3].y);
        return { x , y };
    }

    Point Gradient(float t)
    {
        int p1 = ((int)t) % MaxPoints;
        int p2 = (p1 + 1) % MaxPoints;
        int p3 = (p2 + 1) % MaxPoints;
        int p0 = (p1 - 1) >= 0 ? p1 - 1 : MaxPoints - 1;
        t = t - (int)t; // Decimal part
        float t2 = t * t;
        float t3 = t2 * t;
        // Based on the original Catmull-Rom spline but modified by javidx9
        float q0 = -3.0f * t2 + 4.0f * t - 1.0f;
        float q1 = 9.0f * t2 - 10.0f * t;
        float q2 = -9.0f * t2 + 8.0f * t + 1.0f;
        float q3 = 3.0f * t2 - 2.0f * t;
        float x = 0.5f * (q0 * Points[p0].x + q1 * Points[p1].x + q2 * Points[p2].x + q3 * Points[p3].x);
        float y = 0.5f * (q0 * Points[p0].y + q1 * Points[p1].y + q2 * Points[p2].y + q3 * Points[p3].y);
        return { x , y };
    }

    float CalculateSegmentLength(float t)
    {
        float length = 0;
        float step = 0.05f;
        Point oldP = Interpolate(t), newP;
        for (float u = step; u < 1; u += step) {
            newP = Interpolate(t + u);
            length += sqrtf((newP.x - oldP.x) * (newP.x - oldP.x) + (newP.y - oldP.y) * (newP.y - oldP.y));
            oldP = newP;
        }
        return length;
    }

    void UpdateLength()
    {
        float total = 0;
        for (int i = 0; i < Points.size(); i++) {
            total += Points[i].length = CalculateSegmentLength(i);
        }
        TotalLength = total;
    }

    float GetNormalizedOffset(float offset)
    {
        float total = 0;
        int i = 0;
        while (offset > Points[i].length) {
            offset -= Points[i].length;
            i++;
            if (i >= Points.size()) {
                return i;
            }
        }
        return (float)i + offset / Points[i].length;
    }

    virtual bool OnUserCreate() override
    {
        for (int i = 0; i < MaxPoints; i++) {
            float angle = (float)i / MaxPoints * 3.14159f * 2.0f;
            Points.push_back({ ScreenWidth() / 2 + sinf(angle) * 30.0f, ScreenHeight() / 2 + cosf(angle) * 30.0f, 0.0f });
        }

        UpdateLength();

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        CurrentOffset += 0.35f;

        if (CurrentOffset > TotalLength) {
            CurrentOffset = 0;
        }

        // Change selected control point
        if (GetKey(olc::Z).bReleased) {
            Selected--;
            if (Selected < 0) {
                Selected = MaxPoints - 1;
            }
        }
        if (GetKey(olc::X).bReleased) {
            Selected++;
            if (Selected >= MaxPoints) {
                Selected = 0;
            }
        }

        // Move selected control point
        if (GetKey(olc::LEFT).bHeld) {
            Points[Selected].x -= ControlPointSpeed * fElapsedTime;
        }
        if (GetKey(olc::RIGHT).bHeld) {
            Points[Selected].x += ControlPointSpeed * fElapsedTime;
        }
        if (GetKey(olc::UP).bHeld) {
            Points[Selected].y -= ControlPointSpeed * fElapsedTime;
        }
        if (GetKey(olc::DOWN).bHeld) {
            Points[Selected].y += ControlPointSpeed * fElapsedTime;
        }

        UpdateLength();

        // Clear Screen
        Clear(olc::BLACK);

        // Draw curve
        for (float t = 0; t < Points.size(); t += 0.001) {
            Point p = Interpolate(t);
            Draw(p.x, p.y, olc::WHITE);
        }

        // Draw gradient
        float CurrentGradient = GetNormalizedOffset(CurrentOffset);
        Point p = Interpolate(CurrentGradient);
        Point g = Gradient(CurrentGradient);
        DrawLine(p.x, p.y, p.x + 0.3f * g.x, p.y + 0.3f * g.y, olc::BLUE);
        Draw(p.x, p.y, olc::GREEN);

        // Draw control points
        int n = 0;        
        for (auto p : Points) {
            FillRect(p.x-1, p.y-1, 2, 2, n == Selected ? olc::YELLOW : olc::RED);
            DrawString(p.x + 3, p.y + 3, to_string(n), n == Selected ? olc::YELLOW : olc::RED);
            DrawString(p.x + 18, p.y + 3, to_string(p.length), olc::MAGENTA);
            n++;
        }

        DrawString(0, 0, "Length: " + to_string(TotalLength), olc::WHITE);
        DrawString(0, 8, "Offset: " + to_string(CurrentOffset), olc::MAGENTA);
        DrawString(0, 16, "Point: " + to_string(CurrentGradient), olc::RED);
        DrawString(0, ScreenHeight() - 32, "X Next point     -> Right ^ Up  ", olc::WHITE);
        DrawString(0, ScreenHeight() - 16, "Z Previous point <- Left  v Down", olc::WHITE);

        return true;
    }
};

int main()
{
    CatmullRomSplines game;
    if (game.Construct(320, 240, 4, 4))
        game.Start();

    return 0;
}