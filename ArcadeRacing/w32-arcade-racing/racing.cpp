#include "olcConsoleGameEngine.h"

class RacingGame : public olcConsoleGameEngine
{
    float CarPos;

    // Inherited via olcConsoleGameEngine
    virtual bool OnUserCreate() override
    {
        CarPos = 0;
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        for (int y = 0; y < ScreenHeight() / 2; y++) {
            int row = y + ScreenHeight() / 2;
            float Perspective = 0.1f + 0.9f * ((float)y / ScreenHeight() * 2.0f);
            for (int x = 0; x < ScreenWidth(); x++) {
                short color = 0;

                float RoadCenter = 0.5f;
                float RoadWidth = 0.6f * Perspective;
                float ClipWidth = 0.15f * Perspective;

                int LeftGrass = (int)((RoadCenter - RoadWidth / 2.0f - ClipWidth) * ScreenWidth());
                int LeftClip = (int)((RoadCenter - RoadWidth / 2.0f) * ScreenWidth());
                int RightClip = (int)((RoadCenter + RoadWidth / 2.0f) * ScreenWidth());
                int RightGrass = (int)((RoadCenter + RoadWidth / 2.0f + ClipWidth) * ScreenWidth());

                if (x < LeftGrass) {
                    color = BG_DARK_GREEN;
                }
                else if (x < LeftClip) {
                    color = BG_RED;
                }
                else if (x >= RightGrass) {
                    color = BG_DARK_GREEN;
                }
                else if (x >= RightClip) {
                    color = BG_RED;
                }
                else {
                    color = BG_DARK_GREY;
                }

                Draw(x, row, ' ', color);
                
            }
        }

        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos)-7), 80, L"   ||####||   ", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos)-7), 81, L"      ##      ", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos)-7), 82, L"     ####     ", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos)-7), 83, L"     ####     ", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos)-7), 84, L"|||  ####  |||", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos)-7), 85, L"|||########|||", FG_WHITE | BG_BLACK);
        DrawStringAlpha((int)(ScreenWidth() * (0.5f + CarPos)-7), 86, L"|||  ####  |||", FG_WHITE | BG_BLACK);

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