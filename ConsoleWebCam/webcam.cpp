#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include "escapi.h"
#include "../olcConsoleGameEngine.h"

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
    int mode = 3;

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
        if (GetKey('3').bPressed)
        {
            mode = 3;
        }

        float inv255 = 1.0f / 255.0f;
        for (int y = 0; y < ScreenHeight(); y++)
        {
            for (int x = 0; x < ScreenWidth(); x++)
            {
                int offsetX = mirror ? (capture.mWidth - x - 1) : x;
                int offsetY = (flip ? (capture.mHeight - y - 1) : y) * capture.mWidth;
                Pixel *p = (Pixel *)capture.mTargetBuf + offsetX + offsetY;
                short sym, fg, bg;
                float r = p->c[2] * inv255;
                float g = p->c[1] * inv255;
                float b = p->c[0] * inv255;
                switch (mode)
                {
                    case 1:
                        BlackWhite(r, g, b, sym, fg, bg);
                        break;
                    case 2:
                        Grayscale(r, g, b, sym, fg, bg);
                        break;
                    case 3:
                        HSV(r, g, b, sym, fg, bg);
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

    void rgb2hsv(float r, float g, float b, float &h, float &s, float &v)
    {
        float Cmax = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
        float Cmin = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
        float delta = Cmax - Cmin;
        if (delta == 0)
        {
            h = 0;
        }
        else if (Cmax == r)
        {
            h = 60.0f * (g - b) / delta;
        }
        else if (Cmax == g)
        {
            h = 60.0f * ((b - r) / delta + 2.0f);
        }
        else if (Cmax == b)
        {
            h = 60.0f * ((r - g) / delta + 4.0f);
        }
        h = fmod(h, 360.0f);
        s = (Cmax != 0) ? (delta / Cmax) : 0;
        v = Cmax;
    }

    void HSV(float r, float g, float b, short &sym, short &fg, short &bg)
    {
        float h, s, v;
        rgb2hsv(r, g, b, h, s, v);

        if (s < 0.2f)
        {
            Grayscale(r, g, b, sym, fg, bg);
        }
        else
        {
            short blend[5] = { PIXEL_QUARTER, PIXEL_HALF, PIXEL_THREEQUARTERS, PIXEL_SOLID };
            int id = (int)(fmod(h, 60.0f) / 60.0f * 4.0f);
            sym = blend[id];
            if (h < 60.f)
            {
                bg = v < 0.5f ? BG_DARK_RED : BG_RED;
                fg = v < 0.5f ? FG_DARK_YELLOW : FG_YELLOW;
            }
            else if (h < 160.f)
            {
                bg = v < 0.5f ? BG_DARK_YELLOW : BG_YELLOW;
                fg = v < 0.5f ? FG_DARK_GREEN : FG_GREEN;
            }
            else if (h < 180.f)
            {
                bg = v < 0.5f ? BG_DARK_GREEN : BG_GREEN;
                fg = v < 0.5f ? FG_DARK_CYAN : FG_CYAN;
            }
            else if (h < 240.f)
            {
                bg = v < 0.5f ? BG_DARK_CYAN : BG_CYAN;
                fg = v < 0.5f ? FG_DARK_BLUE : FG_BLUE;
            }
            else if (h < 300.f)
            {
                bg = v < 0.5f ? BG_DARK_BLUE : BG_BLUE;
                fg = v < 0.5f ? FG_DARK_MAGENTA : FG_MAGENTA;
            }
            else
            {
                bg = v < 0.5f ? BG_DARK_MAGENTA : BG_MAGENTA;
                fg = v < 0.5f ? FG_DARK_RED : FG_RED;
            }
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
