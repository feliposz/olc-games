#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "olcPGEX_Graphics3D.h"

class CarCrime : public olc::PixelGameEngine
{
    float theta;

    olc::GFX3D::vec3d eye = { 0, 0, -5 };
    olc::GFX3D::vec3d lookDir = { 0, 0, 1 };
    olc::GFX3D::vec3d up = { 0, 1, 0 };

    olc::Sprite allTexture;

    olc::GFX3D::PipeLine pipeRender;
    olc::GFX3D::mesh meshCube;

public:
    CarCrime()
    {
        sAppName = "CarCrime";
    }
public:

    bool OnUserCreate() override
    {
        allTexture.LoadFromFile("City_Roads1_mip0.png");

        meshCube.tris =
        {
            // SOUTH
            { 0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,     1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // EAST
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f , 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f , 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // NORTH
            { 1.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f , 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f , 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // WEST
            { 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f , 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f , 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // TOP
            { 0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    1.0f , 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f , 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // BOTTOM
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,    0.0f , 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    1.0f , 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },
        };

        olc::GFX3D::ConfigureDisplay();

        float fov = 90.0f;
        float aspect = (float)ScreenHeight() / (float)ScreenWidth();
        float nearZ = 0.1f;
        float farZ = 1000.0f;
        pipeRender.SetProjection(fov, aspect, nearZ, farZ, 0, 0, ScreenWidth(), ScreenHeight());

        theta = 0;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);
        olc::GFX3D::ClearDepth();

        olc::GFX3D::vec3d lookTarget = olc::GFX3D::Math::Vec_Add(eye, lookDir);

        pipeRender.SetCamera(eye, lookTarget, up);

        olc::GFX3D::mat4x4 rotX = olc::GFX3D::Math::Mat_MakeRotationX(theta);
        olc::GFX3D::mat4x4 rotZ = olc::GFX3D::Math::Mat_MakeRotationZ(2.0f * theta);
        olc::GFX3D::mat4x4 transform = olc::GFX3D::Math::Mat_MultiplyMatrix(rotX, rotZ);

        pipeRender.SetTransform(transform);
        pipeRender.SetTexture(&allTexture);
        pipeRender.Render(meshCube.tris);

        theta += fElapsedTime;

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
