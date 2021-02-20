#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "olcPGEX_Graphics3D.h"

class CarCrime : public olc::PixelGameEngine
{
    olc::GFX3D::PipeLine pipeRender;
public:
    CarCrime()
    {
        sAppName = "CarCrime";
    }
public:

    bool OnUserCreate() override
    {
        olc::GFX3D::ConfigureDisplay();
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        return true;
    }

};

int main()
{
    CarCrime demo;
    if (demo.Construct(768, 480, 2, 2))
        demo.Start();
    return 0;
}
