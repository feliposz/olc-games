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
    float PlayerCurvature;
    float TrackCurvature;
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
        PlayerCurvature = 0;
        TrackCurvature = 0;
        CurrentSegment = 0;

        Track.emplace_back(make_pair(0, 10.0f));
        Track.emplace_back(make_pair(0, 100.0f));
        Track.emplace_back(make_pair(0.25f, 100.0f));
        Track.emplace_back(make_pair(0, 50.0f));
        Track.emplace_back(make_pair(-0.5, 100.0f));
        Track.emplace_back(make_pair(0, 150.0f));
        Track.emplace_back(make_pair(-0.75f, 100.0f));
        Track.emplace_back(make_pair(0.5f, 100.0f));
        Track.emplace_back(make_pair(0.1f, 100.0f));

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        if (m_keys[VK_UP].bHeld) {
            Speed += 2.0f * fElapsedTime;
        }
        else {
            Speed -= 1.0f * fElapsedTime;
        }

        if (fabs(PlayerCurvature - TrackCurvature) > 0.8f) {
            Speed -= 6.0f * fElapsedTime;
        }

        int CarDirection = 0;

        if (m_keys[VK_LEFT].bHeld) {
            PlayerCurvature -= 0.7f * (1.0f - Speed / 2.0f) * fElapsedTime;
            CarDirection = -1;
        }

        if (m_keys[VK_RIGHT].bHeld) {
            PlayerCurvature += 0.7f * (1.0f - Speed / 2.0f) * fElapsedTime;
            CarDirection = 1;
        }

        if (Speed < 0) Speed = 0;
        if (Speed > 1) Speed = 1;

        float TargetCurvature = Track[CurrentSegment].first;
        float TargetDistance = Track[CurrentSegment].second;

        float CalcDist = 50.0f * Speed * fElapsedTime;
        Distance += CalcDist;
        CurrentSegmentDistance += CalcDist;

        Curvature += (TargetCurvature - Curvature) * Speed * fElapsedTime;
        TrackCurvature += Curvature * Speed * fElapsedTime;
        CarPos = PlayerCurvature - TrackCurvature;

        if (CurrentSegmentDistance >= TargetDistance) {
            CurrentSegmentDistance -= TargetDistance;
            CurrentSegment++;
            if (CurrentSegment >= Track.size()) {
                CurrentSegment = 0;
            }
        }

        // Render Sky
        for (int y = 0; y < ScreenHeight() / 2; y++) {
            short SkyShades[5] = { PIXEL_SOLID, PIXEL_THREEQUARTERS, PIXEL_HALF, PIXEL_QUARTER, ' ' };
            int i = (int)y / (ScreenHeight() / 2 / 5);
            Fill(0, y, ScreenWidth(), y + 1, SkyShades[i], FG_BLUE | BG_DARK_BLUE);
        }

        // Render Mountains
        for (int x = 0; x < ScreenWidth(); x++) {
            int MountainHeight = 10 + ScreenHeight() * 0.1f * (
                0.5f * sinf(0.04f * (x + 50 * PlayerCurvature))
                + 0.2f * sinf(0.12f * (x + 50 * PlayerCurvature))
                + 0.3f * sinf(0.21f * (x + 50 * PlayerCurvature))
                + 0.1f * sinf(0.07f * (x + 50 * PlayerCurvature))
                );
            if (MountainHeight > 0) {
                Fill(x, ScreenHeight() / 2 - MountainHeight, x + 1, ScreenHeight() / 2, PIXEL_HALF, FG_DARK_RED | BG_DARK_YELLOW);
            }
        }

        // Render Track
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
                else if (CurrentSegment == 0) {
                    color = ((x & 1) != (y & 1)) ? BG_WHITE : BG_DARK_GREY;
                }
                else {
                    color = BG_DARK_GREY;
                }

                Draw(x, row, ' ', color);

            }
        }

        // Render Car
        if (CarDirection == -1) {
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 80, L"\\\\####\\\\     ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 81, L"   ##          ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 82, L"   ####        ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 83, L"    ####       ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 84, L"\\\\\\ ####   \\\\\\", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 85, L"\\O\\########\\\\\\", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 86, L"\\\\\\  ####  \\\\\\", FG_WHITE | BG_BLACK);
        }
        else if (CarDirection == 1) {
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 80, L"      //####//", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 81, L"         ##   ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 82, L"       ####   ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 83, L"      ####    ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 84, L"///  ####  ///", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 85, L"///########/O/", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 86, L"///  ####  ///", FG_WHITE | BG_BLACK);
        }
        else {
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 80, L"   ||####||   ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 81, L"      ##      ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 82, L"     ####     ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 83, L"     ####     ", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 84, L"|||  ####  |||", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 85, L"|||########|||", FG_WHITE | BG_BLACK);
            DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos) - 7), 86, L"|||  ####  |||", FG_WHITE | BG_BLACK);
        }


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