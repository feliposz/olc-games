#include <stdio.h>
#include <stdint.h>
#include "escapi.h"

union Pixel
{
    uint32_t d;
    uint8_t c[4];
};

int main()
{
    int devices = setupESCAPI();

    if (devices > 0)
    {
        SimpleCapParams capture;
        capture.mWidth = 320;
        capture.mHeight = 240;
        Pixel *pixels = new Pixel[capture.mWidth * capture.mHeight];
        char out[80 * 24 + 1];
        capture.mTargetBuf = (int *)pixels;

        if (initCapture(0, &capture))
        {
            for (;;)
            {
                doCapture(0);
                while (!isCaptureDone(0)) {}

                char *ptr = out;
                for (int y = 0; y < 24; y++)
                {
                    for (int x = 0; x < 79; x++)
                    {
                        Pixel *p = pixels + (y*10) * capture.mWidth + (x*4);
                        *ptr++ = p->c[1] >= 127 ? '.' : '@';
                    }
                    *ptr++ = '\n';
                }
                printf("%s\n", out);
            }
            out[sizeof(out) - 1] = 0;
            deinitCapture(0);
        }
        else
        {
            printf("Capture failed - device may already be in use.\n");
        }

    }
    else
    {
        printf("error initializing escapi or no camera present");
    }

    return 0;
}