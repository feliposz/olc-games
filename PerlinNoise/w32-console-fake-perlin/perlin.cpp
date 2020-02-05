#include "olcConsoleGameEngine.h"
using namespace std;

class Game : public olcConsoleGameEngine
{
    float *RandomSeed = nullptr;
    float *RandomNoise = nullptr;
    int Size = 0;

    virtual bool OnUserCreate() override
    {
        Size = ScreenWidth();
        RandomSeed = new float[Size];
        RandomNoise = new float[Size];

        for (int i = 0; i < Size; i++) {
            RandomSeed[i] = (float)rand() / RAND_MAX;
        }

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

        PerlinNoise1D(Size, RandomSeed, 8, RandomNoise);

        for (int x = 0; x < ScreenWidth(); x++) {
            int y = (int)(RandomNoise[x] * ScreenHeight() / 2);
            DrawLine(x, ScreenHeight() / 2 - y, x, ScreenHeight() / 2, PIXEL_SOLID, FG_GREEN);
        }

        return true;
    }

    void PerlinNoise1D(int size, float *seed, int octaves, float *noise)
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
                scale /= 2.0f;
            }
            noise[x] = n / acc;
        }
    }

};

int main()
{
    Game game;
    game.ConstructConsole(256, 256, 3, 3);
    game.Start();

    return 0;
}