#include <string>
#include "olcConsoleGameEngine.h"
using namespace std;

class Game : public olcConsoleGameEngine
{
    float *RandomSeed = nullptr;
    float *RandomNoise = nullptr;
    int Size = 0;
    int Octaves = 8;
    float ScaleBias = 2.0f;

    virtual bool OnUserCreate() override
    {
        Size = ScreenWidth();
        RandomSeed = new float[Size];
        RandomNoise = new float[Size];

        GenerateRandomSeed1D(Size, RandomSeed);

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

        if (m_keys[L'S'].bReleased) {
            GenerateRandomSeed1D(Size, RandomSeed);
        }

        if (m_keys[VK_LEFT].bReleased && Octaves > 1) {
            Octaves--;
        }

        if (m_keys[VK_RIGHT].bReleased && Octaves < 8) {
            Octaves++;
        }

        if (m_keys[VK_UP].bReleased && ScaleBias < 10.0f) {
            ScaleBias += 0.2f;
        }

        if (m_keys[VK_DOWN].bReleased && ScaleBias > 0.2f) {
            ScaleBias -= 0.2f;
        }

        PerlinNoise1D(Size, RandomSeed, Octaves, ScaleBias, RandomNoise);

        for (int x = 0; x < ScreenWidth(); x++) {
            int y = (int)(RandomNoise[x] * ScreenHeight() / 2);
            DrawLine(x, ScreenHeight() / 2 - y, x, ScreenHeight() / 2, PIXEL_SOLID, FG_GREEN);
        }

        return true;
    }

    void GenerateRandomSeed1D(int size, float *seed)
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
    Game game;
    game.ConstructConsole(128, 128, 6, 6);
    game.Start();

    return 0;
}