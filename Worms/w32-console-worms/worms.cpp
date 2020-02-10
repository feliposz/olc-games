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
    float friction = 0.8f;
    bool stable = false;
    bool dead = false;
    bool explode = false;
    int bounces = 1;

    PhysicsObject(float x = 0, float y = 0)
    {
        px = x;
        py = y;
    }

    virtual void Draw(olcConsoleGameEngine *engine, float offsetX, float offsetY) = 0;
    virtual void OnCollision() = 0;
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

    virtual void OnCollision()
    {
    }

private:
    static vector<pair<float, float>> model;
};

class Debris : public PhysicsObject {

public:
    Debris(float x = 0, float y = 0) : PhysicsObject(x, y)
    {
        float angle = PI * 2 * (float)rand() / RAND_MAX;
        vx = 10.f * cosf(angle);
        vy = 10.f * sinf(angle);
        radius = 2.0f;
        bounces = 3;
        friction = 1.0f;
    }

    virtual void Draw(olcConsoleGameEngine * engine, float offsetX, float offsetY) override
    {
        engine->DrawWireFrameModel(model, px - offsetX, py - offsetY, atan2(vy, vx), radius, FG_DARK_GREEN);
    }

    virtual void OnCollision()
    {
        bounces--;
        if (bounces <= 0) {
            dead = true;
        }
    }

private:
    static vector<pair<float, float>> model;
};

class Worm : public PhysicsObject {

public:
    float shootAngle = 0;

    Worm(float x = 0, float y = 0) : PhysicsObject(x, y)
    {
        friction = 0.4f;
    }

    virtual void Draw(olcConsoleGameEngine * engine, float offsetX, float offsetY) override
    {
        int offsetSpriteX = 3;
        int offsetSpriteY = 3;
        engine->DrawSprite(px - offsetX - offsetSpriteX, py - offsetY - offsetSpriteY, &sprite);
    }

    virtual void OnCollision()
    {
    }

private:
    static olcSprite sprite;
};

class Missile : public PhysicsObject {

public:
    Missile(float x = 0, float y = 0) : PhysicsObject(x, y)
    {
    }

    virtual void Draw(olcConsoleGameEngine * engine, float offsetX, float offsetY) override
    {
        engine->DrawWireFrameModel(model, px - offsetX, py - offsetY, atan2(vy, vx), radius, FG_YELLOW);
    }

    virtual void OnCollision()
    {
        dead = true;
        explode = true;
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

vector<pair<float, float>> DefineDebris()
{
    vector<pair<float, float>> model;
    model.push_back(make_pair(-0.5f, -0.5f));
    model.push_back(make_pair(-0.5f, 0.5f));
    model.push_back(make_pair(0.5f, 0.5f));
    model.push_back(make_pair(0.5f, -0.5f));
    return model;
}

vector<pair<float, float>> DefineMissile()
{
    vector<pair<float, float>> model;

    model.push_back({ 0.0f, 0.0f });
    model.push_back({ 1.0f, 1.0f });
    model.push_back({ 2.0f, 1.0f });
    model.push_back({ 2.5f, 0.0f });
    model.push_back({ 2.0f, -1.0f });
    model.push_back({ 1.0f, -1.0f });
    model.push_back({ 0.0f, 0.0f });
    model.push_back({ -1.0f, -1.0f });
    model.push_back({ -2.5f, -1.0f });
    model.push_back({ -2.0f, 0.0f });
    model.push_back({ -2.5f, 1.0f });
    model.push_back({ -1.0f, 1.0f });

    // Scale points
    for (auto &p : model) {
        p.first /= 2.5f;
        p.second /= 2.5f;
    }

    return model;
}

olcSprite DefineWorm()
{
    olcSprite sprite(L"Sprites/worms.spr");
    return sprite;
}

vector<pair<float, float>> Dummy::model = DefineDummy();
vector<pair<float, float>> Debris::model = DefineDebris();
vector<pair<float, float>> Missile::model = DefineMissile();
olcSprite Worm::sprite = DefineWorm();

class WormsGame : public olcConsoleGameEngine
{
    unsigned char *Map;
    int MapWidth = 512;
    int MapHeight = 256;
    float CameraX = 0.0f;
    float CameraY = 0.0f;
    float TargetCameraX = 0.0f;
    float TargetCameraY = 0.0f;
    int CameraBorder = 10;
    list<unique_ptr<PhysicsObject>> Objects;
    PhysicsObject *SelectedUnit = nullptr;
    PhysicsObject *CameraFollowing = nullptr;

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
            Objects.push_back(unique_ptr<Missile>(new Missile(m_mousePosX + CameraX, m_mousePosY + CameraY)));
        }

        if (m_mouse[1].bReleased) {
            Explosion(m_mousePosX + CameraX, m_mousePosY + CameraY, 10);
        }

        if (m_mouse[0].bReleased) {
            PhysicsObject *worm = new Worm(m_mousePosX + CameraX, m_mousePosY + CameraY);
            Objects.push_back(unique_ptr<Worm>((Worm*)worm));
            SelectedUnit = worm;
            CameraFollowing = worm;
        }


        if (CameraFollowing) {
            TargetCameraX = CameraFollowing->px - ScreenWidth() / 2;
            TargetCameraY = CameraFollowing->py - ScreenHeight() / 2;
        }
        else {
            TargetCameraX = CameraX;
            TargetCameraY = CameraY;
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

        CameraX += (TargetCameraX - CameraX) * fElapsedTime;
        CameraY += (TargetCameraY - CameraY) * fElapsedTime;

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

        if (SelectedUnit && SelectedUnit->stable) {
            Worm *worm = (Worm *)SelectedUnit;
            if (m_keys[L'Z'].bPressed) {
                SelectedUnit->stable = false;
                SelectedUnit->vx = 4.0f * cosf(worm->shootAngle);
                SelectedUnit->vy = 8.0f * sinf(worm->shootAngle);
            }
            if (m_keys[L'A'].bHeld) {
                worm->shootAngle -= 1.0f * fElapsedTime;
            }
            if (m_keys[L'S'].bHeld) {
                worm->shootAngle += 1.0f * fElapsedTime;
            }
        }

        // Draw terrain
        for (int y = 0; y < ScreenHeight(); y++) {
            for (int x = 0; x < ScreenWidth(); x++) {
                short color = Map[((int)CameraY + y) * MapWidth + (int)CameraX + x] ? FG_DARK_GREEN : FG_CYAN;
                Draw(x, y, PIXEL_SOLID, color);
            }
        }

        // Update objects
        for (auto &o : Objects) {
            // Physics steps
            for (int step = 0; step < 10; step++) {
                if (!o->stable) {
                    bool collided = false;
                    o->ay += 2.0f;
                    o->vx += o->ax * fElapsedTime;
                    o->vy += o->ay * fElapsedTime;
                    float potentialX = o->px + o->vx * fElapsedTime;
                    float potentialY = o->py + o->vy * fElapsedTime;
                    o->ax = 0.0f;
                    o->ay = 0.0f;
                    o->stable = false;
                    float responseX = 0;
                    float responseY = 0;
                    float direction = atan2(o->vy, o->vx);

                    // Test collision in a semi-circle (approximate to get a ner pixel-perfect collision check)
                    for (float testAngle = direction - PI / 2; testAngle < direction + PI / 2; testAngle += PI / (2.0f * o->radius)) {
                        float testX = cosf(testAngle) * o->radius;
                        float testY = sinf(testAngle) * o->radius;
                        int testMapX = (int)(potentialX + testX);
                        int testMapY = (int)(potentialY + testY);

                        if (testMapX < 0) testMapX = 0;
                        if (testMapX > MapWidth - 1) testMapX = MapWidth - 1;
                        if (testMapY < 0) testMapY = 0;
                        if (testMapY > MapHeight - 1) testMapY = MapHeight - 1;

                        if (Map[testMapY * MapWidth + testMapX] > 0) {
                            collided = true;
                            responseX -= testX;
                            responseY -= testY;
                        }
                    }

                    if (collided) {
                        o->stable = true;

                        // Normalize response vector
                        float responseMag = sqrtf(responseX*responseX + responseY * responseY);
                        responseX /= responseMag;
                        responseY /= responseMag;

                        // Dot product and reflection of movement
                        float dot = o->vx * responseX + o->vy * responseY;
                        float reflectX = o->vx - 2.0f * dot * responseX * o->friction;
                        float reflectY = o->vy - 2.0f * dot * responseY * o->friction;
                        o->vx = reflectX;
                        o->vy = reflectY;

                        float vectorMag = sqrtf(o->vx*o->vx + o->vy*o->vy);

                        o->OnCollision();
                        if (o->explode) {
                            Explosion(o->px, o->py, 20);
                        }
                    }
                    else {
                        o->px = potentialX;
                        o->py = potentialY;
                    }
                }
            }
        }

        Objects.remove_if([](unique_ptr<PhysicsObject> &o) { return o->dead; });

        // Draw objects
        for (auto &o : Objects) {
            o->Draw(this, CameraX, CameraY);
        }

        if (SelectedUnit && SelectedUnit->stable) {
            Worm *worm = (Worm *)SelectedUnit;
            float ShootingX = SelectedUnit->px - CameraX + cosf(worm->shootAngle) * 10.0f;
            float ShootingY = SelectedUnit->py - CameraY + sinf(worm->shootAngle) * 10.0f;
            Draw(ShootingX, ShootingY, PIXEL_SOLID, FG_BLACK);
            Draw(ShootingX - 1, ShootingY, PIXEL_SOLID, FG_BLACK);
            Draw(ShootingX + 1, ShootingY, PIXEL_SOLID, FG_BLACK);
            Draw(ShootingX, ShootingY - 1, PIXEL_SOLID, FG_BLACK);
            Draw(ShootingX, ShootingY + 1, PIXEL_SOLID, FG_BLACK);
        }

        return true;
    }

    void Explosion(float x, float y, float radius)
    {
        // Shockwave
        for (auto &o : Objects) {
            float dx = o->px - x;
            float dy = o->py - y;
            float distance = sqrtf(dx * dx + dy * dy);
            if (distance < 0.5f) distance = 0.5f;
            if (distance < radius) {
                o->vx += dx / distance * radius;
                o->vy += dy / distance * radius;
                o->stable = false;
            }
        }
        // Destroy terrain in a circular area
        int sqSize = radius * radius;
        for (int py = -radius; py < radius; py++) {
            for (int px = -radius; px < radius; px++) {
                int sqDistance = px * px + py * py;
                if (sqDistance < sqSize) {
                    Map[((int)y + py) * MapWidth + (int)x + px] = 0;
                }
            }
        }
        for (int i = 0; i < 10; i++) {
            Objects.push_back(unique_ptr<Debris>(new Debris(x, y)));
        }
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