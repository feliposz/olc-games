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
    float carAngle = 0;
    float carX = 0;
    float carY = 0;

    olc::GFX3D::vec3d eye = { 0, 0, -5 };
    olc::GFX3D::vec3d lookDir = { 0, 0, 1 };
    olc::GFX3D::vec3d up = { 0, 1, 0 };

    olc::Sprite allTexture;
    olc::Sprite car;
    olc::Sprite *grass;
    olc::Sprite *road[12];
    olc::Sprite *roof;
    olc::Sprite *doorNorth, *doorSouth, *doorWest, *doorEast;
    olc::Sprite *windowNorth, *windowSouth, *windowWest, *windowEast;

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
        car.LoadFromFile("car_top1.png");

        // split road tiles into individual sprites
        int i = 0;
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 3; x++)
            {
                road[i] = new olc::Sprite(96, 96);
                SetDrawTarget(road[i]);
                DrawPartialSprite(0, 0, &allTexture, x * 96, y * 96, 96, 96);
                i++;
            }
        }

        // grass texture is just a special case for road tiles
        grass = road[2];

        // split tiles for building textures

        roof = new olc::Sprite(96, 96);
        SetDrawTarget(roof);
        DrawPartialSprite(0, 0, &allTexture, 352, 64, 96, 96);

        doorNorth = new olc::Sprite(96, 32);
        SetDrawTarget(doorNorth);
        DrawPartialSprite(0, 0, &allTexture, 352, 0, 96, 32);

        windowNorth = new olc::Sprite(96, 32);
        SetDrawTarget(windowNorth);
        DrawPartialSprite(0, 0, &allTexture, 352, 32, 96, 32);

        windowSouth = new olc::Sprite(96, 32);
        SetDrawTarget(windowSouth);
        DrawPartialSprite(0, 0, &allTexture, 352, 160, 96, 32);

        doorSouth = new olc::Sprite(96, 32);
        SetDrawTarget(doorSouth);
        DrawPartialSprite(0, 0, &allTexture, 352, 192, 96, 32);

        doorWest = new olc::Sprite(32, 96);
        SetDrawTarget(doorWest);
        DrawPartialSprite(0, 0, &allTexture, 288, 64, 32, 96);

        windowWest = new olc::Sprite(32, 96);
        SetDrawTarget(windowWest);
        DrawPartialSprite(0, 0, &allTexture, 320, 64, 32, 96);

        windowEast = new olc::Sprite(32, 96);
        SetDrawTarget(windowEast);
        DrawPartialSprite(0, 0, &allTexture, 448, 64, 32, 96);

        doorEast = new olc::Sprite(32, 96);
        SetDrawTarget(doorEast);
        DrawPartialSprite(0, 0, &allTexture, 480, 64, 32, 96);

        SetDrawTarget(nullptr);

        walls.tris =
        {
            // EAST
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f , 1.0f, 0.2f, 1.0f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.2f, 1.0f,    1.0f , 0.0f, 0.2f, 1.0f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },

            // WEST
            { 0.0f, 0.0f, 0.2f, 1.0f,    0.0f, 1.0f, 0.2f, 1.0f,    0.0f , 1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f, },
            { 0.0f, 0.0f, 0.2f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f , 0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // SOUTH
            { 0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.2f, 1.0f,    1.0f , 1.0f, 0.2f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.2f, 1.0f,    1.0f , 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

            // NORTH
            { 1.0f, 0.0f, 0.2f, 1.0f,    0.0f, 0.0f, 0.2f, 1.0f,    0.0f , 0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f, },
            { 1.0f, 0.0f, 0.2f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    1.0f , 0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f, },

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

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        float cameraSpeed = 2.0f;
        bool followCar = true;
        if (GetKey(olc::S).bHeld)
        {
            eye.y += cameraSpeed * fElapsedTime;
            followCar = false;
        }
        if (GetKey(olc::W).bHeld)
        {
            eye.y -= cameraSpeed * fElapsedTime;
            followCar = false;
        }
        if (GetKey(olc::D).bHeld)
        {
            eye.x += cameraSpeed * fElapsedTime;
            followCar = false;
        }
        if (GetKey(olc::A).bHeld)
        {
            eye.x -= cameraSpeed * fElapsedTime;
            followCar = false;
        }
        if (GetKey(olc::X).bHeld)
        {
            eye.z += cameraSpeed * fElapsedTime;
        }
        if (GetKey(olc::Z).bHeld)
        {
            eye.z -= cameraSpeed * fElapsedTime;
        }

        int mapX = (int)floorf(carX);
        int mapY = (int)floorf(carY);
        MapCell *cell = nullptr;
        if ((mapX >= 0) && (mapX < mapWidth) && (mapY >= 0) && (mapY < mapHeight))
        {
            cell = &map[mapY * mapWidth + mapX];
        }

        if (GetKey(olc::R).bPressed)
        {
            if (cell)
            {
                cell->road = !cell->road;
            }
        }

        if (GetKey(olc::T).bPressed)
        {
            if (cell)
            {
                cell->height++;
            }
        }

        if (GetKey(olc::E).bPressed)
        {
            if (cell)
            {
                cell->height--;
            }
        }

        float carSpeed = 2.0f;
        float turnSpeed = 2.0f;
        float deltaCarX = 0;
        float deltaCarY = 0;
        if (GetKey(olc::RIGHT).bHeld)
        {
            carAngle += turnSpeed * fElapsedTime;
        }
        if (GetKey(olc::LEFT).bHeld)
        {
            carAngle -= turnSpeed * fElapsedTime;
        }
        if (GetKey(olc::UP).bHeld)
        {
            deltaCarX = carSpeed * fElapsedTime * cos(carAngle);
            deltaCarY = carSpeed * fElapsedTime * sin(carAngle);
        }
        if (GetKey(olc::DOWN).bHeld)
        {
            deltaCarX = -carSpeed * fElapsedTime * cos(carAngle);
            deltaCarY = -carSpeed * fElapsedTime * sin(carAngle);
        }

        carX += deltaCarX;
        carY += deltaCarY;

        if (followCar)
        {
            eye.x += (carX - eye.x) * fElapsedTime * carSpeed + 1.5f * deltaCarX;
            eye.y += (carY - eye.y) * fElapsedTime * carSpeed + 1.5f * deltaCarY;
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
                    olc::GFX3D::mat4x4 transform = olc::GFX3D::Math::Mat_MakeTranslation(x, y, 0);
                    pipeRender.SetTransform(transform);
                    pipeRender.SetTexture(road[GetRoadIndex(x, y)]);
                    pipeRender.Render(flatQuad.tris);
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
                    else if (cell->height > 0)
                    {
                        for (int h = 0; h < cell->height; h++)
                        {
                            olc::GFX3D::mat4x4 transform = olc::GFX3D::Math::Mat_MakeTranslation(x, y, -(h + 1) * 0.2f);
                            pipeRender.SetTransform(transform);
                            pipeRender.SetTexture(h == 0 ? doorNorth : windowNorth);
                            pipeRender.Render(walls.tris);
                        }
                        olc::GFX3D::mat4x4 transform = olc::GFX3D::Math::Mat_MakeTranslation(x, y, -cell->height * 0.2f);
                        pipeRender.SetTransform(transform);
                        pipeRender.SetTexture(roof);
                        pipeRender.Render(flatQuad.tris);
                    }
                }
            }
        }

        {
            olc::GFX3D::mat4x4 carOffset = olc::GFX3D::Math::Mat_MakeTranslation(-0.5f, -0.5f, 0);
            olc::GFX3D::mat4x4 carScale = olc::GFX3D::Math::Mat_MakeScale(0.4f, 0.2f, 0);
            olc::GFX3D::mat4x4 carRotZ = olc::GFX3D::Math::Mat_MakeRotationZ(carAngle);
            olc::GFX3D::mat4x4 carWorld = olc::GFX3D::Math::Mat_MakeTranslation(carX, carY, -0.1f);
            olc::GFX3D::mat4x4 carTransform = olc::GFX3D::Math::Mat_MultiplyMatrix(carOffset, carScale);
            carTransform = olc::GFX3D::Math::Mat_MultiplyMatrix(carTransform, carRotZ);
            carTransform = olc::GFX3D::Math::Mat_MultiplyMatrix(carTransform, carWorld);
            pipeRender.SetTransform(carTransform);
            pipeRender.SetTexture(&car);

            SetPixelMode(olc::Pixel::ALPHA);
            pipeRender.Render(flatQuad.tris);
            SetPixelMode(olc::Pixel::NORMAL);
        }

        return true;
    }

    bool CheckRoad(int x, int y)
    {
        if ((x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight))
        {
            return map[y * mapWidth + x].road;
        }
        else
        {
            return false;
        }
    };

    int GetRoadIndex(int x, int y)
    {
        bool west = CheckRoad(x - 1, y);
        bool east = CheckRoad(x + 1, y);
        bool north = CheckRoad(x, y + 1);
        bool south = CheckRoad(x, y - 1);

        int i = 0;

        if (!west && east && !north && south) i = 3;
        else if (west && east && !north && south) i = 4;
        else if (west && !east && !north && south) i = 5;
        else if (!west && east && north && south) i = 6;
        else if (west && east && north && south) i = 7;
        else if (west && !east && north && south) i = 8;
        else if (!west && east && north && !south) i = 9;
        else if (west && east && north && !south) i = 10;
        else if (west && !east && north && !south) i = 11;
        else if (!west && !east && (north || south)) i = 0;
        else if ((west || east) && !north && !south) i = 1;

        return i;
    }

};

int main()
{
    CarCrime demo;
    if (demo.Construct(768, 480, 2, 2))
        demo.Start();
    return 0;
}
