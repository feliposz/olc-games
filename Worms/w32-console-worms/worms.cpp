#include <vector>
#include <utility>
#include "olcConsoleGameEngine.h"
using namespace std;

class PhysicsObject {
public:
    float px = 0, py = 0;
    float vx = 0, vy = 0;
    float ax = 0, ay = 0;
    float radius = 4.0f;
    bool stable = false;

    PhysicsObject(float x = 0, float y = 0)
    {
        px = x;
        py = y;
    }

    virtual void Draw(olcConsoleGameEngine *engine, float offsetX, float offsetY) = 0;
};

class Dummy : public PhysicsObject {

public:
    Dummy(float x = 0, float y = 0) : PhysicsObject(x, y)
    {
    }

    virtual void Draw(olcConsoleGameEngine * engine, float offsetX, float offsetY) override
    {
        engine->DrawWireFrameModel(model, px - offsetX, py - offsetY, atan2(vy, vx), radius);
    }

private:
    static vector<pair<float, float>> model;
};

vector<pair<float, float>> DefineDummy()
{
    vector<pair<float, float>> model;
    model.push_back(make_pair(0, 0));
    for (int i = 0; i <= 10; i++) {
        model.push_back(make_pair(sin((float)i / 10 * 3.14159f * 2), cos((float)i / 10 * 3.14159f * 2)));
    }
    return model;
}

vector<pair<float, float>> Dummy::model = DefineDummy();

class WormsGame : public olcConsoleGameEngine
{
    unsigned char *Map;
    int MapWidth = 512;
    int MapHeight = 256;
    float CameraX = 0.0f;
    float CameraY = 0.0f;
    int CameraBorder = 10;
    list<unique_ptr<PhysicsObject>> Objects;

    virtual bool OnUserCreate() override
    {
        Map = new unsigned char[MapWidth * MapHeight];
        GenerateMap();
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        float cameraSpeed = 400.0f;

        if (m_keys[L'M'].bReleased) {
            GenerateMap();
        }

        if (m_mouse[2].bReleased) {
            Objects.push_back(unique_ptr<Dummy>(new Dummy(m_mousePosX + CameraX, m_mousePosY + CameraY)));
        }

        if (m_mousePosX < CameraBorder) {
            CameraX -= cameraSpeed * fElapsedTime;
        }
        if (m_mousePosY < CameraBorder) {
            CameraY -= cameraSpeed * fElapsedTime;
        }
        if (m_mousePosX > ScreenWidth() - CameraBorder) {
            CameraX += cameraSpeed * fElapsedTime;
        }
        if (m_mousePosY > ScreenHeight() - CameraBorder) {
            CameraY += cameraSpeed * fElapsedTime;
        }

        if (CameraX < 0.0f) {
            CameraX = 0.0f;
        }
        if (CameraX > MapWidth - ScreenWidth()) {
            CameraX = MapWidth - ScreenWidth();
        }
        if (CameraY < 0.0f) {
            CameraY = 0.0f;
        }
        if (CameraY > MapHeight - ScreenHeight()) {
            CameraY = MapHeight - ScreenHeight();
        }

        for (int y = 0; y < ScreenHeight(); y++) {
            for (int x = 0; x < ScreenWidth(); x++) {
                short color = Map[((int)CameraY + y) * MapWidth + (int)CameraX + x] ? FG_CYAN : FG_DARK_GREEN;
                Draw(x, y, PIXEL_SOLID, color);
            }
        }

        for (auto &o : Objects) {
            o->Draw(this, CameraX, CameraY);
        }

        return true;
    }

    void GenerateMap()
    {
        float *Seed = new float[MapWidth];
        float *Noise = new float[MapWidth];
        FillRandomSeed(MapWidth, Seed);
        Seed[0] = 0.5f;
        PerlinNoise1D(MapWidth, Seed, 10, 2.0f, Noise);

        for (int y = 0; y < MapHeight; y++) {
            for (int x = 0; x < MapWidth; x++) {
                Map[y * MapWidth + x] = Noise[x] * MapHeight > y ? 1 : 0;
            }
        }

        delete[] Seed;
        delete[] Noise;
    }

    void FillRandomSeed(int size, float *seed)
    {
        for (int i = 0; i < size; i++) {
            seed[i] = (float)rand() / RAND_MAX;
        }
    }

    void PerlinNoise1D(int size, float *seed, int octaves, float bias, float *noise)
    {
        for (int x = 0; x < size; x++) {
            float scale = 1.0f;
            float acc = 0.0f;
            float n = 0.0f;
            for (int o = 0; o < octaves; o++) {
                int pitch = size >> o;
                int sample1 = (x / pitch) * pitch;
                int sample2 = (sample1 + pitch) % size;
                float blend = (float)(x - sample1) / (float)pitch;
                float interpolation = (1.0f - blend) * seed[sample1] + blend * seed[sample2];
                n += interpolation * scale;
                acc += scale;
                scale /= bias;
            }
            noise[x] = n / acc;
        }
    }
};

int main()
{
    WormsGame game;
    game.ConstructConsole(256, 160, 6, 6);
    game.Start();

    return 0;
}