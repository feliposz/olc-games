#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include "escapi.h"
#include "olcConsoleGameEngine.h"

union Pixel
{
    uint32_t d;
    uint8_t c[4];
};

class WebCam : public olcConsoleGameEngine
{
private:
    SimpleCapParams capture;
    bool mirror = true;
    bool flip = false;
    int mode = 2;

public:

    virtual bool OnUserCreate() override
    {
        bool success = false;
        int devices = setupESCAPI();

        if (devices > 0)
        {
            capture.mWidth = ScreenWidth();
            capture.mHeight = ScreenHeight();
            Pixel *pixels = new Pixel[capture.mWidth * capture.mHeight];
            capture.mTargetBuf = (int *)pixels;

            success = initCapture(0, &capture);
        }
        return success;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        doCapture(0);
        while (!isCaptureDone(0)) {}

        if (GetKey('M').bPressed)
        {
            mirror = !mirror;
        }
        if (GetKey('V').bPressed)
        {
            flip = !flip;
        }
        if (GetKey('1').bPressed)
        {
            mode = 1;
        }
        if (GetKey('2').bPressed)
        {
            mode = 2;
        }

        for (int y = 0; y < ScreenHeight(); y++)
        {
            for (int x = 0; x < ScreenWidth(); x++)
            {
                int offsetX = mirror ? (capture.mWidth - x - 1) : x;
                int offsetY = (flip ? (capture.mHeight - y - 1) : y) * capture.mWidth;
                Pixel *p = (Pixel *)capture.mTargetBuf + offsetX + offsetY;
                short sym, fg, bg;
                float r = p->c[2] / 255.0f;
                float g = p->c[1] / 255.0f;
                float b = p->c[0] / 255.0f;
                switch (mode)
                {
                    case 1:
                        BlackWhite(r, g, b, sym, fg, bg);
                        break;
                    case 2:
                        Grayscale(r, g, b, sym, fg, bg);
                        break;
                }
                Draw(x, y, sym, fg | bg);
            }
        }

        return true;
    }

    void BlackWhite(float r, float g, float b, short &sym, short &fg, short &bg)
    {
        sym = PIXEL_SOLID;
        fg = g >= 0.5f ? FG_WHITE : FG_BLACK;
        bg = BG_BLACK;
    }

    void Grayscale(float r, float g, float b, short &sym, short &fg, short &bg)
    {
        float brightness = 0.299f * r + 0.587f * g + 0.114f * b;
        int index = (int)(13.0f * brightness);
        switch (index)
        {
            case 0: bg = BG_BLACK; fg = FG_BLACK; sym = PIXEL_SOLID; break;
            case 1: bg = BG_BLACK; fg = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
            case 2: bg = BG_BLACK; fg = FG_DARK_GREY; sym = PIXEL_HALF; break;
            case 3: bg = BG_BLACK; fg = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
            case 4: bg = BG_BLACK; fg = FG_DARK_GREY; sym = PIXEL_SOLID; break;
            case 5: bg = FG_DARK_GREY; fg = FG_GREY; sym = PIXEL_QUARTER; break;
            case 6: bg = FG_DARK_GREY; fg = FG_GREY; sym = PIXEL_HALF; break;
            case 7: bg = FG_DARK_GREY; fg = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
            case 8: bg = FG_DARK_GREY; fg = FG_GREY; sym = PIXEL_SOLID; break;
            case 9: bg = FG_GREY; fg = FG_WHITE; sym = PIXEL_QUARTER; break;
            case 10: bg = FG_GREY; fg = FG_WHITE; sym = PIXEL_HALF; break;
            case 11: bg = FG_GREY; fg = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
            default: bg = FG_GREY; fg = FG_WHITE; sym = PIXEL_SOLID; break;
        }
    }

    ~WebCam()
    {
        delete capture.mTargetBuf;
        deinitCapture(0);
    }
};

int main()
{
    WebCam game;
    game.ConstructConsole(320, 240, 4, 4);
    game.Start();

    return 0;
}
