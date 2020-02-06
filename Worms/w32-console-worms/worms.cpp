#include <vector>
#include <utility>
#include "olcConsoleGameEngine.h"
using namespace std;

const float PI = 3.1415926f;

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
        model.push_back(make_pair(cosf((float)i / 10 * PI * 2), sinf((float)i / 10 * PI * 2)));
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
                short color = Map[((int)CameraY + y) * MapWidth + (int)CameraX + x] ? FG_DARK_GREEN : FG_CYAN;
                Draw(x, y, PIXEL_SOLID, color);
            }
        }

        for (auto &o : Objects) {
            o->Draw(this, CameraX, CameraY);

            // Physics steps
            for (int step = 0; step < 10; step++) {
                o->ay += 2.0f;
                o->vx += o->ax * fElapsedTime;
                o->vy += o->ay * fElapsedTime;
                float potentialX = o->px + o->vx * fElapsedTime;
                float potentialY = o->py + o->vy * fElapsedTime;

                float direction = atan2(o->vy, o->vx);

                bool collided = false;

                // Debug direction vector
                //int debugX = (int)(o->px - CameraX);
                //int debugY = (int)(o->py - CameraY);
                //DrawLine(debugX, debugY, debugX + (int)(cosf(direction) * 10), debugY + (int)(sinf(direction) * 10), PIXEL_SOLID, FG_YELLOW);

                // Test collision in a semi-circle (approximate to get a ner pixel-perfect collision check)
                for (float testAngle = direction - PI/2; testAngle < direction + PI/2; testAngle += PI / (2.0f * o->radius)) {
                    float testX = potentialX + cosf(testAngle) * o->radius;
                    float testY = potentialY + sinf(testAngle) * o->radius;
                    if (Map[(int)testY * MapWidth + (int)testX] > 0) {
                        collided = true;
                    }
                    // Debug collision points
                    //Draw(testX - (int)CameraX, testY - (int)CameraY, PIXEL_SOLID, collided ? FG_RED : FG_BLUE);
                }

                if (collided) {
                    o->vx = 0.0f;
                    o->vy = 0.0f;
                }
                else {
                    o->px = potentialX;
                    o->py = potentialY;
                }

                o->ax = 0.0f;
                o->ay = 0.0f;
            }
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
                Map[y * MapWidth + x] = Noise[x] * MapHeight > y ? 0 : 1;
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