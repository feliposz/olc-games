#include <vector>
#include "olcConsoleGameEngine.h"

using namespace std;

struct GameObject {
    float x, y;
    float dx, dy;
    int size;
    float angle;
};

class AsteroidsGame : public olcConsoleGameEngine
{
private:
    vector<GameObject> asteroids;

public:
    virtual bool OnUserCreate() override
    {
        asteroids.push_back({20.0f, 20.0f, 20.0f, 20.0f, 16, 0.0f});
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ', FG_BLACK);

        for (auto &a : asteroids) {
            a.x += a.dx * fElapsedTime;
            a.y += a.dy * fElapsedTime;

            for (int x = 0; x < a.size; x++) {
                for (int y = 0; y < a.size; y++) {
                    Draw(x + a.x, y + a.y);
                }
            }
        }

        return true;
    }
};

int main() 
{
    AsteroidsGame game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

    return 0;
}