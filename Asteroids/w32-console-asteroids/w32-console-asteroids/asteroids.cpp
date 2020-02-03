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
    vector<pair<float, float>> playerModel;

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

            WrapCoordinates(a.x, a.y, a.x, a.y);

            for (int x = 0; x < a.size; x++) {
                for (int y = 0; y < a.size; y++) {
                    Draw(x + a.x, y + a.y);
                }
            }
        }

        DrawWireFrameModel(playerModel, player.x, player.y, player.angle, player.size);

        return true;
    }

    void DrawWireFrameModel(vector<pair<float, float>> points, float x, float y, float angle, float scale)
    {
        for (int i = 0; i < points.size(); i++) {
            int j = (i + 1) % points.size();
            float x1 = x + scale * (points[i].first * cos(angle) - points[i].second * sin(angle));
            float y1 = y + scale * (points[i].first * sin(angle) + points[i].second * cos(angle));
            float x2 = x + scale * (points[j].first * cos(angle) - points[j].second * sin(angle));
            float y2 = y + scale * (points[j].first * sin(angle) + points[j].second * cos(angle));
            DrawLine(x1, y1, x2, y2);
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