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
    GameObject player;

    void WrapCoordinates(float xi, float yi, float &xo, float &yo)
    {
        xo = xi;
        yo = yi;
        if (xo < 0) {
            xo += ScreenWidth();
        }
        if (xo >= ScreenWidth()) {
            xo -= ScreenWidth();
        }
        if (yo < 0) {
            yo += ScreenHeight();
        }
        if (yo >= ScreenHeight()) {
            yo -= ScreenHeight();
        }
    }

public:
    virtual bool OnUserCreate() override
    {
        asteroids.push_back({20.0f, 20.0f, 20.0f, 20.0f, 16, 0.0f});
        player.x = ScreenWidth() / 2;
        player.y = ScreenHeight() / 2;
        player.dx = 0;
        player.dy = 0;
        player.size = 3;
        player.angle = 0;
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ', FG_BLACK);

        for (auto &a : asteroids) {
            a.x += a.dx * fElapsedTime;
            a.y += a.dy * fElapsedTime;

            WrapCoordinates(a.x, a.y, a.x, a.y);

            for (int x = 0; x < a.size; x++) {
                for (int y = 0; y < a.size; y++) {
                    Draw(x + a.x, y + a.y);
                }
            }
        }

        float px[3] = { -1, 1, 0 };
        float py[3] = { 0, 0, -2 };

        for (int i = 0; i < 3; i++) {
            int j = (i + 1) % 3;
            DrawLine(player.x + px[i] * player.size, player.y + py[i] * player.size, player.x + px[j] * player.size, player.y + py[j] * player.size);
        }

        return true;
    }

    void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
    {
        float fx = (float)x;
        float fy = (float)y;
        WrapCoordinates(fx, fy, fx, fy);
        olcConsoleGameEngine::Draw((int)fx, (int)fy, c, col);
    }
};

int main() 
{
    AsteroidsGame game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

    return 0;
}