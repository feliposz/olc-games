#include <vector>
#include <algorithm>
#include "olcConsoleGameEngine.h"

using namespace std;

const float PI = 3.1415926f;

struct GameObject {
    float x, y;
    float dx, dy;
    int size;
    float angle;
    bool active;
};

class AsteroidsGame : public olcConsoleGameEngine
{
private:
    GameObject player;
    vector<GameObject> asteroids;
    vector<GameObject> bullets;
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
        asteroids.push_back({ 20.0f, 20.0f, 20.0f, 20.0f, 16, 0.0f, true });

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

        if (m_keys[VK_SPACE].bReleased) {
            bullets.push_back({ player.x, player.y, 1.0f * sin(player.angle), -1.0f * cos(player.angle), 0, 0.0f, true });
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

        vector<GameObject> newAsteroids;

        for (auto &b : bullets) {
            Draw(b.x, b.y, PIXEL_SOLID, FG_RED);
            b.x += b.dx;
            b.y += b.dy;

            if (b.x >= 0 && b.x < ScreenWidth() && b.y >= 0 && b.y < ScreenHeight()) {
                for (auto &a : asteroids) {
                    if (PointInsideCircle(b.x, b.y, a.x, a.y, a.size)) {
                        b.active = false;
                        a.active = false;
                        if (a.size > 4) {
                            newAsteroids.push_back({ a.x, a.y, 20.0f * sin(player.angle - PI / 2), 20.0f * cos(player.angle - PI / 2), a.size / 2, a.angle, true });
                            newAsteroids.push_back({ a.x, a.y, 20.0f * sin(player.angle + PI / 2), 20.0f * cos(player.angle + PI / 2), a.size / 2, a.angle, true });
                        }
                    }
                }
            }
            else {
                b.active = false;
            }
        }

        bullets.erase(remove_if(bullets.begin(), bullets.end(), [&](GameObject o) {
            return !o.active;
        }), bullets.end());

        asteroids.erase(remove_if(asteroids.begin(), asteroids.end(), [&](GameObject o) {
            return !o.active;
        }), asteroids.end());

        for (auto &a : newAsteroids) {
            asteroids.push_back(a);
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

    bool PointInsideCircle(float x1, float y1, float x2, float y2, float size)
    {
        return sqrtf((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) < size;
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