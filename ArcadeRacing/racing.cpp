#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <utility>
using namespace std;

class RacingGame : public olc::PixelGameEngine
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
        if (GetKey(olc::UP).bHeld) {
            Speed += 2.0f * fElapsedTime;
        }
        else {
            Speed -= 1.0f * fElapsedTime;
        }

        if (fabs(PlayerCurvature - TrackCurvature) > 0.8f) {
            Speed -= 6.0f * fElapsedTime;
        }

        int CarDirection = 0;

        if (GetKey(olc::LEFT).bHeld) {
            PlayerCurvature -= 0.7f * (1.0f - Speed / 2.0f) * fElapsedTime;
            CarDirection = -1;
        }

        if (GetKey(olc::RIGHT).bHeld) {
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
        olc::Pixel skyColor0 = olc::BLUE;
        olc::Pixel skyColor1 = olc::DARK_BLUE;
        for (int y = 0; y < ScreenHeight() / 2; y++) {
            DrawLine(0, y, ScreenWidth(), y, olc::PixelLerp(skyColor0, skyColor1, y / (ScreenHeight() / 2.0)));
        }

        // Render Mountains
        olc::Pixel mountainColor = olc::PixelLerp(olc::DARK_RED, olc::DARK_YELLOW, 0.5f);
        for (int x = 0; x < ScreenWidth(); x++) {
            int MountainHeight = 20 + ScreenHeight() * 0.1f * (
                0.5f * sinf(0.04f * (x*0.5f + 50 * PlayerCurvature))
                + 0.2f * sinf(0.12f * (x*0.5f + 50 * PlayerCurvature))
                + 0.3f * sinf(0.21f * (x*0.5f + 50 * PlayerCurvature))
                + 0.1f * sinf(0.07f * (x*0.5f + 50 * PlayerCurvature))
                );
            if (MountainHeight > 0) {
                DrawLine(x, ScreenHeight() / 2 - MountainHeight, x, ScreenHeight() / 2, mountainColor);
            }
        }

        // Render Track
        for (int y = 0; y < ScreenHeight() / 2; y++) {
            int row = y + ScreenHeight() / 2;
            float Perspective = 0.1f + 0.8f * ((float)y / ScreenHeight() * 2.0f);
            for (int x = 0; x < ScreenWidth(); x++) {
                olc::Pixel color = olc::BLACK;

                float RoadCenter = 0.5f + Curvature * powf(1.0f - Perspective, 3);
                float RoadWidth = 0.6f * Perspective;
                float ClipWidth = 0.15f * Perspective;
                RoadWidth *= 0.5f;

                int LeftGrass = (int)((RoadCenter - RoadWidth - ClipWidth) * ScreenWidth());
                int LeftClip = (int)((RoadCenter - RoadWidth) * ScreenWidth());
                int RightClip = (int)((RoadCenter + RoadWidth) * ScreenWidth());
                int RightGrass = (int)((RoadCenter + RoadWidth + ClipWidth) * ScreenWidth());

                olc::Pixel GrassColor = sinf(20.0f * powf(1.0f - Perspective, 3) + Distance * 0.1f) > 0 ? olc::GREEN : olc::DARK_GREEN;
                olc::Pixel StripeColor = sinf(80.0f * powf(1.0f - Perspective, 2) + Distance) > 0 ? olc::RED : olc::WHITE;

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
                    color = ((x & 1) != (y & 1)) ? olc::WHITE : olc::DARK_GREY;
                }
                else {
                    color = olc::DARK_GREY;
                }

                Draw(x, row, color);

            }
        }

        int CarCol = (int)(ScreenWidth() * (0.5f + CarPos) - 7);
        int CarRow = 140;
        
        olc::Pixel CarColor = olc::BLACK;

        // Render Car
        if (CarDirection == -1)
        {
            DrawString(CarCol, CarRow + 0 * 8, "\\\\####\\\\  ", CarColor);
            DrawString(CarCol, CarRow + 1 * 8, "   ##     ", CarColor);
            DrawString(CarCol, CarRow + 2 * 8, "\\\\######\\\\", CarColor);
            DrawString(CarCol, CarRow + 3 * 8, "\\\\######\\\\", CarColor);
        }
        else if (CarDirection == 1)
        {
            DrawString(CarCol, CarRow + 0 * 8, "  //####//", CarColor);
            DrawString(CarCol, CarRow + 1 * 8, "     ##   ", CarColor);
            DrawString(CarCol, CarRow + 2 * 8, "//######//", CarColor);
            DrawString(CarCol, CarRow + 3 * 8, "//######//", CarColor);
        }
        else
        {
            DrawString(CarCol, CarRow + 0 * 8, " ||####|| ", CarColor);
            DrawString(CarCol, CarRow + 1 * 8, "    ##    ", CarColor);
            DrawString(CarCol, CarRow + 2 * 8, "||######||", CarColor);
            DrawString(CarCol, CarRow + 3 * 8, "||######||", CarColor);
        }

        return true;
    }
};

int main()
{
    RacingGame game;
    if (game.Construct(320, 200, 4, 4))
        game.Start();

    return 0;
}