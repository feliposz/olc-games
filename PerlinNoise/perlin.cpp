#include <string>
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
using namespace std;

class Game : public olc::PixelGameEngine
{
    float *RandomSeed1D = nullptr;
    float *RandomNoise1D = nullptr;
    float *RandomSeed2D = nullptr;
    float *RandomNoise2D = nullptr;
    int Size = 0;
    int Width = 0;
    int Height = 0;
    int Octaves = 8;
    int Mode = 1;
    float ScaleBias = 2.0f;

    virtual bool OnUserCreate() override
    {
        Size = ScreenWidth();
        Width = ScreenWidth();
        Height = ScreenHeight();
        RandomSeed1D = new float[Size];
        RandomNoise1D = new float[Size];
        RandomSeed2D = new float[Width*Height];
        RandomNoise2D = new float[Width*Height];

        FillRandomSeed(Size, RandomSeed1D);
        FillRandomSeed(Width * Height, RandomSeed2D);

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);

        if (GetKey(olc::K1).bReleased) {
            Mode = 1;
        }

        if (GetKey(olc::K2).bReleased) {
            Mode = 2;
        }

        if (GetKey(olc::LEFT).bReleased && Octaves > 1) {
            Octaves--;
        }

        if (GetKey(olc::RIGHT).bReleased && Octaves < 8) {
            Octaves++;
        }

        if (GetKey(olc::UP).bReleased && ScaleBias < 10.0f) {
            ScaleBias += 0.2f;
        }

        if (GetKey(olc::DOWN).bReleased && ScaleBias > 0.2f) {
            ScaleBias -= 0.2f;
        }

        if (Mode == 1) {
            if (GetKey(olc::S).bReleased) {
                FillRandomSeed(Size, RandomSeed1D);
            }
            PerlinNoise1D(Size, RandomSeed1D, Octaves, ScaleBias, RandomNoise1D);

            for (int x = 0; x < ScreenWidth(); x++) {
                int y = (int)(RandomNoise1D[x] * ScreenHeight() / 2);
                DrawLine(x, ScreenHeight() / 2 - y, x, ScreenHeight() / 2, olc::GREEN);
            }
        }
        else {
            if (GetKey(olc::S).bReleased) {
                FillRandomSeed(Width * Height, RandomSeed2D);
            }
            PerlinNoise2D(Width, Height, RandomSeed2D, Octaves, ScaleBias, RandomNoise2D);

            for (int y = 0; y < ScreenHeight(); y++) {
                for (int x = 0; x < ScreenWidth(); x++) {
                    int z = (int)(RandomNoise2D[y * ScreenWidth() + x] * 21);
                    float blend = 0;
                    olc::Pixel color = olc::BLACK;
                    switch (z) {
                    case 0: blend = 1.00f; color = olc::BLACK; break;
                    case 1: blend = 0.25f; color = olc::BLUE; break;
                    case 2: blend = 0.50f; color = olc::BLUE; break;
                    case 3: blend = 0.75f; color = olc::BLUE; break;
                    case 4: blend = 1.00f; color = olc::BLUE; break;
                    case 5: blend = 0.25f; color = olc::GREEN; break;
                    case 6: blend = 0.50f; color = olc::GREEN; break;
                    case 7: blend = 0.75f; color = olc::GREEN; break;
                    case 8: blend = 1.00f; color = olc::GREEN; break;
                    case 9: blend = 0.25f; color = olc::YELLOW; break;
                    case 10: blend = 0.50f; color = olc::YELLOW; break;
                    case 11: blend = 0.75f; color = olc::YELLOW; break;
                    case 12: blend = 1.00f; color = olc::YELLOW; break;
                    case 13: blend = 0.25f; color = olc::RED; break;
                    case 14: blend = 0.50f; color = olc::RED; break;
                    case 15: blend = 0.75f; color = olc::RED; break;
                    case 16: blend = 1.00f; color = olc::RED; break;
                    case 17: blend = 0.25f; color = olc::WHITE; break;
                    case 18: blend = 0.50f; color = olc::WHITE; break;
                    case 19: blend = 0.75f; color = olc::WHITE; break;
                    case 20: blend = 1.00f; color = olc::WHITE; break;
                    }
                    Draw(x, y, PixelLerp(olc::BLACK, color, blend));
                }
            }
        }

        return true;
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

    void PerlinNoise2D(int width, int height, float *seed, int octaves, float bias, float *noise)
    {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float scale = 1.0f;
                float acc = 0.0f;
                float n = 0.0f;
                for (int o = 0; o < octaves; o++) {
                    int pitchX = width >> o;
                    int pitchY = height >> o;
                    int sample1X = (x / pitchX) * pitchX;
                    int sample2X = (sample1X + pitchX) % width;
                    int sample1Y = (y / pitchY) * pitchX;
                    int sample2Y = (sample1Y + pitchY) % height;
                    float blendX = (float)(x - sample1X) / (float)pitchX;
                    float blendY = (float)(y - sample1Y) / (float)pitchY;
                    float sampleTop = (1.0f - blendX) * seed[sample1Y * width + sample1X] + blendX * seed[sample1Y * width + sample2X];
                    float sampleBottom = (1.0f - blendX) * seed[sample2Y * width + sample1X] + blendX * seed[sample2Y * width + sample2X];
                    float interpolation = (1.0f - blendY) * sampleTop + blendY * sampleBottom;
                    n += interpolation * scale;
                    acc += scale;
                    scale /= bias;
                }
                noise[y * width + x] = n / acc;
            }
        }
    }

};

int main()
{
    Game game;
    if (game.Construct(256, 256, 4, 4))
        game.Start();

    return 0;
}