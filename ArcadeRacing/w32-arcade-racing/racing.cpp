#include "olcConsoleGameEngine.h"
#include <vector>
#include <utility>
using namespace std;

class RacingGame : public olcConsoleGameEngine
{
    float CarPos;
    float Distance;
    float Speed;
    float Curvature;
    vector<pair<float, float>> Track; // Curvature/Distance segments
    float CurrentSegmentDistance;
    int CurrentSegment;

    // Inherited via olcConsoleGameEngine
    virtual bool OnUserCreate() override
    {
        CarPos = 0;
        Distance = 0;
        Speed = 0;
        Curvature = 0;
        CurrentSegment = 0;
        
        Track.emplace_back(make_pair(0, 200.0f));
        Track.emplace_back(make_pair(0.5f, 100.0f));
        Track.emplace_back(make_pair(0, 200.0f));
        Track.emplace_back(make_pair(-0.5f, 100.0f));

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        if (m_keys[VK_UP].bHeld) {
            Speed += 10.0f * fElapsedTime;
        }
        else {
            Speed -= 2.0f * fElapsedTime;
        }

        if (Speed < 0) Speed = 0;
        if (Speed > 1) Speed = 1;

        float CalcDist = 50.0f * Speed * fElapsedTime;
        Distance += CalcDist;
        CurrentSegmentDistance += CalcDist;

        if (CurrentSegmentDistance >= Track[CurrentSegment].second) {
            CurrentSegmentDistance -= Track[CurrentSegment].second;
            CurrentSegment++;
            if (CurrentSegment >= Track.size()) {
                CurrentSegment = 0;
            }
        }

        // Test
        Curvature = Track[CurrentSegment].first;

        for (int y = 0; y < ScreenHeight() / 2; y++) {
            int row = y + ScreenHeight() / 2;
            float Perspective = 0.1f + 0.8f * ((float)y / ScreenHeight() * 2.0f);
            for (int x = 0; x < ScreenWidth(); x++) {
                short color = 0;

                float RoadCenter = 0.5f + Curvature * powf(1.0f - Perspective, 3);
                float RoadWidth = 0.6f * Perspective;
                float ClipWidth = 0.15f * Perspective;

                int LeftGrass = (int)((RoadCenter - RoadWidth / 2.0f - ClipWidth) * ScreenWidth());
                int LeftClip = (int)((RoadCenter - RoadWidth / 2.0f) * ScreenWidth());
                int RightClip = (int)((RoadCenter + RoadWidth / 2.0f) * ScreenWidth());
                int RightGrass = (int)((RoadCenter + RoadWidth / 2.0f + ClipWidth) * ScreenWidth());

                float GrassColor = sinf(20.0f * powf(1.0f - Perspective, 3) + Distance * 0.1f) > 0 ? BG_GREEN : BG_DARK_GREEN;
                float StripeColor = sinf(80.0f * powf(1.0f - Perspective, 2) + Distance) > 0 ? BG_RED : BG_WHITE;

                if (x < LeftGrass) {
                    color = GrassColor;
                }
                else if (x < LeftClip) {
                    color = StripeColor;
                }
                else if (x >= RightGrass) {
                    color = GrassColor;
                }
                else if (x >= RightClip) {
                    color = StripeColor;
                }
                else {
                    color = BG_DARK_GREY;
                }

                Draw(x, row, ' ', color);

            }
        }

        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 80, L"   ||####||   ", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 81, L"      ##      ", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 82, L"     ####     ", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 83, L"     ####     ", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 84, L"|||  ####  |||", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 85, L"|||########|||", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 86, L"|||  ####  |||", FG_WHITE | BG_BLACK);

        return true;
    }
};

int main()
{
    RacingGame game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

    return 0;
}