#include <vector>
#include "olcConsoleGameEngine.h"

using namespace std;

const float PI = 3.1415926f;

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
    vector<pair<float, float>> playerModel;
    vector<pair<float, float>> asteroidModel;

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

        playerModel.push_back(make_pair(-1, 1));
        playerModel.push_back(make_pair(1, 1));
        playerModel.push_back(make_pair(0, -2));

        for (int i = 0; i < 20; i++) {
            float angle = (float)i / 20.0f * 2.0f * PI;
            float radius = (float)rand() / (float)RAND_MAX * 0.4 + 0.8;
            asteroidModel.push_back(make_pair(radius * sin(angle), radius * cos(angle)));
        }

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

        if (m_keys[VK_LEFT].bHeld) {
            player.angle -= 5.0f * fElapsedTime;
        }

        if (m_keys[VK_RIGHT].bHeld) {
            player.angle += 5.0f * fElapsedTime;
        }

        if (m_keys[VK_UP].bHeld) {
            player.dx += sin(player.angle) * 1.0f * fElapsedTime;
            player.dy += -cos(player.angle) * 1.0f * fElapsedTime;
        }

        player.x += player.dx;
        player.y += player.dy;

        WrapCoordinates(player.x, player.y, player.x, player.y);

        for (auto &a : asteroids) {
            a.x += a.dx * fElapsedTime;
            a.y += a.dy * fElapsedTime;
            a.angle += 0.5f * fElapsedTime;

            WrapCoordinates(a.x, a.y, a.x, a.y);

            DrawWireFrameModel(asteroidModel, a.x, a.y, a.angle, a.size, FG_YELLOW);
        }

        DrawWireFrameModel(playerModel, player.x, player.y, player.angle, player.size);

        return true;
    }

    void DrawWireFrameModel(vector<pair<float, float>> points, float x, float y, float angle, float scale, short color = 15)
    {
        for (int i = 0; i < points.size(); i++) {
            int j = (i + 1) % points.size();
            float x1 = x + scale * (points[i].first * cos(angle) - points[i].second * sin(angle));
            float y1 = y + scale * (points[i].first * sin(angle) + points[i].second * cos(angle));
            float x2 = x + scale * (points[j].first * cos(angle) - points[j].second * sin(angle));
            float y2 = y + scale * (points[j].first * sin(angle) + points[j].second * cos(angle));
            DrawLine(x1, y1, x2, y2, PIXEL_SOLID, color);
        }
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