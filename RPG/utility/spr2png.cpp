#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "olcConsoleGameEngine.h"

using namespace std;

void ConvertConsoleSpriteToPixelSprite(string inputFile, string outputFile)
{
    olcSprite input;
    input.Load(olc::ConvertS2W(inputFile));

    olc::Sprite output(input.nWidth, input.nHeight);

    for (int y = 0; y < input.nHeight; y++) {
        for (int x = 0; x < input.nWidth; x++) {
            olc::Pixel foregroundColor;
            olc::Pixel backgroundColor;
            switch (input.GetColour(x, y) & 0x0F) {
            case FG_BLACK:         foregroundColor = olc::BLACK; break;
            case FG_DARK_BLUE:     foregroundColor = olc::DARK_BLUE; break;
            case FG_DARK_GREEN:    foregroundColor = olc::DARK_GREEN; break;
            case FG_DARK_CYAN:     foregroundColor = olc::DARK_CYAN; break;
            case FG_DARK_RED:      foregroundColor = olc::DARK_RED; break;
            case FG_DARK_MAGENTA:  foregroundColor = olc::DARK_MAGENTA; break;
            case FG_DARK_YELLOW:   foregroundColor = olc::DARK_YELLOW; break;
            case FG_GREY:          foregroundColor = olc::GREY; break;
            case FG_DARK_GREY:     foregroundColor = olc::DARK_GREY; break;
            case FG_BLUE:          foregroundColor = olc::BLUE; break;
            case FG_GREEN:         foregroundColor = olc::GREEN; break;
            case FG_CYAN:          foregroundColor = olc::CYAN; break;
            case FG_RED:           foregroundColor = olc::RED; break;
            case FG_MAGENTA:       foregroundColor = olc::MAGENTA; break;
            case FG_YELLOW:        foregroundColor = olc::YELLOW; break;
            case FG_WHITE:         foregroundColor = olc::WHITE; break;
            }
            switch (input.GetColour(x, y) & 0xF0) {
            case BG_BLACK:         backgroundColor = olc::BLACK; break;
            case BG_DARK_BLUE:     backgroundColor = olc::DARK_BLUE; break;
            case BG_DARK_GREEN:    backgroundColor = olc::DARK_GREEN; break;
            case BG_DARK_CYAN:     backgroundColor = olc::DARK_CYAN; break;
            case BG_DARK_RED:      backgroundColor = olc::DARK_RED; break;
            case BG_DARK_MAGENTA:  backgroundColor = olc::DARK_MAGENTA; break;
            case BG_DARK_YELLOW:   backgroundColor = olc::DARK_YELLOW; break;
            case BG_GREY:          backgroundColor = olc::GREY; break;
            case BG_DARK_GREY:     backgroundColor = olc::DARK_GREY; break;
            case BG_BLUE:          backgroundColor = olc::BLUE; break;
            case BG_GREEN:         backgroundColor = olc::GREEN; break;
            case BG_CYAN:          backgroundColor = olc::CYAN; break;
            case BG_RED:           backgroundColor = olc::RED; break;
            case BG_MAGENTA:       backgroundColor = olc::MAGENTA; break;
            case BG_YELLOW:        backgroundColor = olc::YELLOW; break;
            case BG_WHITE:         backgroundColor = olc::WHITE; break;
            }
            olc::Pixel blendColor = olc::WHITE;
            switch (input.GetGlyph(x, y)) {
            case PIXEL_SOLID:
                blendColor = foregroundColor; 
                break;
            case PIXEL_THREEQUARTERS:
                blendColor.r = (int)(foregroundColor.r * 0.75f + backgroundColor.r * 0.25f);
                blendColor.g = (int)(foregroundColor.g * 0.75f + backgroundColor.g * 0.25f);
                blendColor.b = (int)(foregroundColor.b * 0.75f + backgroundColor.b * 0.25f);
                break;
            case PIXEL_HALF:
                blendColor.r = (int)(foregroundColor.r * 0.50f + backgroundColor.r * 0.50f);
                blendColor.g = (int)(foregroundColor.g * 0.50f + backgroundColor.g * 0.50f);
                blendColor.b = (int)(foregroundColor.b * 0.50f + backgroundColor.b * 0.50f);
                break;
            case PIXEL_QUARTER:
                blendColor.r = (int)(foregroundColor.r * 0.25f + backgroundColor.r * 0.75f);
                blendColor.g = (int)(foregroundColor.g * 0.25f + backgroundColor.g * 0.75f);
                blendColor.b = (int)(foregroundColor.b * 0.25f + backgroundColor.b * 0.75f);
                break;
            case L' ':
                blendColor = olc::BLANK;
                break;
            default:
                blendColor = backgroundColor;
                break;
            }
            output.SetPixel(x, y, blendColor);
        }
    }
    output.SaveToPGESprFile(outputFile);
}

// TODO: Use Gdiplus to convert to PNG
// REFERENCE: https://stackoverflow.com/questions/39551863/creating-gdi-bitmaps-in-memory-and-then-saving-as-png

int main()
{
    string inputBaseDir = "D:/GitHub/javidx9/videos/olcRolePlayingGame/rpgdata/gfx";
    string outputBaseDir = "D:/GitHub/feliposz/w32-pixel-rpg/rpgdata/gfx";

    //string inputBaseDir = "D:/GitHub/javidx9/videos/JarioSprites";
    //string outputBaseDir = "D:/GitHub/feliposz/w32-pixel-rpg/Sprites";


    WIN32_FIND_DATAA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    hFind = FindFirstFileA((inputBaseDir + "/*.spr").c_str(), &ffd);
    do {
        cout << "IN: " << (inputBaseDir + "/" + ffd.cFileName) << endl;
        cout << "OUT: " << (outputBaseDir + "/" + ffd.cFileName) << endl;
        ConvertConsoleSpriteToPixelSprite(inputBaseDir + "/" + ffd.cFileName, outputBaseDir + "/" + ffd.cFileName);
    } while (FindNextFileA(hFind, &ffd) != 0);
    FindClose(hFind);

    return 0;
}