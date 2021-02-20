#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "olcPGEX_Graphics3D.h"

struct MapCell
{
    bool road;
    int height;
};

class CarCrime : public olc::PixelGameEngine
{
    float theta;

    olc::GFX3D::vec3d eye = { 0, 0, -5 };
    olc::GFX3D::vec3d lookDir = { 0, 0, 1 };
    olc::GFX3D::vec3d up = { 0, 1, 0 };

    olc::Sprite allTexture;
    olc::Sprite *grass;

    olc::GFX3D::PipeLine pipeRender;
    olc::GFX3D::mesh flatQuad;
    olc::GFX3D::mesh walls;

    int mapWidth, mapHeight;
    MapCell *map;

public:
    CarCrime()
    {
        sAppName = "CarCrime";
    }
public:

    bool OnUserCreate() override
    {
        mapWidth = 64;
        mapHeight = 32;
        map = new MapCell[mapWidth * mapHeight];
        for (int y = 0; y < mapHeight; y++)
        {
            for (int x = 0; x < mapWidth; x++)
            {
                MapCell *cell = &map[y * mapWidth + x];
                cell->road = false;
                cell->height = 0;
            }
        }

        allTexture.LoadFromFile("City_Roads1_mip0.png");

        grass = new olc::Sprite(96, 96);
        SetDrawTarget(grass);
        DrawPartialSprite(0, 0, &allTexture, 192, 0, 96, 96);

        SetDrawTarget(nullptr);

        walls.tris =
        {
            // EAST
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f , 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f , 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // WEST
            { 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f , 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f , 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // SOUTH
            { 0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    1.0f , 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f , 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // NORTH
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,    0.0f , 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    1.0f , 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },
        };

        flatQuad.tris =
        {
            { 0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,     1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },
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
        float cameraSpeed = 2.0f;
        if (GetKey(olc::S).bHeld)
        {
            eye.y += cameraSpeed * fElapsedTime;
        }
        if (GetKey(olc::W).bHeld)
        {
            eye.y -= cameraSpeed * fElapsedTime;
        }
        if (GetKey(olc::D).bHeld)
        {
            eye.x += cameraSpeed * fElapsedTime;
        }
        if (GetKey(olc::A).bHeld)
        {
            eye.x -= cameraSpeed * fElapsedTime;
        }
        if (GetKey(olc::X).bHeld)
        {
            eye.z += cameraSpeed * fElapsedTime;
        }
        if (GetKey(olc::Z).bHeld)
        {
            eye.z -= cameraSpeed * fElapsedTime;
        }

        Clear(olc::BLUE);
        olc::GFX3D::ClearDepth();

        olc::GFX3D::vec3d lookTarget = olc::GFX3D::Math::Vec_Add(eye, lookDir);

        pipeRender.SetCamera(eye, lookTarget, up);

        for (int y = 0; y < mapHeight; y++)
        {
            for (int x = 0; x < mapWidth; x++)
            {
                MapCell *cell = &map[y * mapWidth + x];
                if (cell->road)
                {

                }
                else
                {
                    if (cell->height == 0)
                    {
                        olc::GFX3D::mat4x4 transform = olc::GFX3D::Math::Mat_MakeTranslation(x, y, 0);
                        pipeRender.SetTransform(transform);
                        pipeRender.SetTexture(grass);
                        pipeRender.Render(flatQuad.tris);
                    }
                    else
                    {

                    }
                }
            }
        }

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
