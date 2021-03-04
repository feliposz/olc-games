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

        for (int y = 0; y < ScreenHeight(); y++)
        {
            for (int x = 0; x < ScreenWidth(); x++)
            {
                int offsetX = mirror ? (capture.mWidth - x - 1) : x;
                int offsetY = (flip ? (capture.mHeight - y - 1) : y) * capture.mWidth;
                Pixel *p = (Pixel *)capture.mTargetBuf + offsetX + offsetY;
                short sym, fg, bg;
                BlackWhite(p->c[2], p->c[1], p->c[0], sym, fg, bg);
                Draw(x, y, sym, fg | bg);
            }
        }

        return true;
    }

    void BlackWhite(int r, int g, int b, short &sym, short &fg, short &bg)
    {
        sym = PIXEL_SOLID;
        fg = g >= 127 ? FG_WHITE : FG_BLACK;
        bg = BG_BLACK;
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
