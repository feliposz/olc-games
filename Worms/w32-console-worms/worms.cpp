#include <vector>
#include <utility>
#include "olcConsoleGameEngine.h"
using namespace std;

const float PI = 3.1415926f;
const int MapWidth = 1024;
const int MapHeight = 512;

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

    virtual void Draw(olcConsoleGameEngine *engine, float offsetX, float offsetY, bool zoom) = 0;
    virtual void OnCollision() = 0;
    virtual void TakeDamage(float damage) = 0;
};

class Dummy : public PhysicsObject {

public:
    Dummy(float x = 0, float y = 0) : PhysicsObject(x, y)
    {
    }

    virtual void Draw(olcConsoleGameEngine * engine, float offsetX, float offsetY, bool zoom) override
    {
        if (zoom) {
            engine->DrawWireFrameModel(model, px * ((float)engine->ScreenWidth() / MapWidth), py * ((float)engine->ScreenHeight() / MapHeight), atan2(vy, vx), radius * 0.5f);
        }
        else {
            engine->DrawWireFrameModel(model, px - offsetX, py - offsetY, atan2(vy, vx), radius);
        }
    }

    virtual void OnCollision()
    {
    }

    virtual void TakeDamage(float damage)
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

    virtual void Draw(olcConsoleGameEngine * engine, float offsetX, float offsetY, bool zoom) override
    {
        if (zoom) {
            engine->DrawWireFrameModel(model, px * ((float)engine->ScreenWidth() / MapWidth), py * ((float)engine->ScreenHeight() / MapHeight), atan2(vy, vx), radius * 0.5f, FG_DARK_GREEN);
        }
        else {
            engine->DrawWireFrameModel(model, px - offsetX, py - offsetY, atan2(vy, vx), radius, FG_DARK_GREEN);
        }
    }

    virtual void OnCollision()
    {
        bounces--;
        if (bounces <= 0) {
            dead = true;
        }
    }

    virtual void TakeDamage(float damage)
    {
    }

private:
    static vector<pair<float, float>> model;
};

class Worm : public PhysicsObject {

public:
    float shootAngle = 0;
    bool energizing = false;
    float energy = 0;
    float health = 1;
    int team = 0;
    bool playable = true;

    Worm(float x = 0, float y = 0) : PhysicsObject(x, y)
    {
        friction = 0.2f;
    }

    virtual void Draw(olcConsoleGameEngine * engine, float offsetX, float offsetY, bool zoom) override
    {
        const int spriteW = 8, spriteH = 8;
        const int offsetSpriteX = -3, offsetSpriteY = -3, offsetEnergyX = -3, offsetEnergyY = -8;
        const int offsetHealthX = -3, offsetHealthY = 6;

        float posX = px - offsetX;
        float posY = py - offsetY;

        if (zoom) {
            posX = px * ((float)engine->ScreenWidth() / MapWidth);
            posY = py * ((float)engine->ScreenHeight() / MapHeight);
        }

        if (health > 0) {
            engine->DrawPartialSprite(posX + offsetSpriteX, posY + offsetSpriteY, &spriteSheet, team * spriteW, 0, spriteW, spriteH);
            engine->DrawLine(posX + offsetHealthX, posY + offsetHealthY, posX + offsetHealthX + (int)(spriteW * health), posY + offsetHealthY, PIXEL_SOLID, FG_BLUE);
            if (energizing) {
                engine->DrawLine(posX + offsetEnergyX, posY + offsetEnergyY, posX + offsetEnergyX + (int)(spriteW * energy), posY + offsetEnergyY, PIXEL_SOLID, FG_RED);
                engine->DrawLine(posX + offsetEnergyX, posY + offsetEnergyY + 1, posX + offsetEnergyX + (int)(spriteW * energy), posY + offsetEnergyY + 1, PIXEL_SOLID, FG_GREEN);
            }
        }
        else {
            engine->DrawPartialSprite(posX + offsetSpriteX, posY + offsetSpriteY, &spriteSheet, team * spriteW, spriteH, spriteW, spriteH);
        }
    }

    virtual void OnCollision()
    {
    }

    virtual void TakeDamage(float damage)
    {
        if (playable) {
            health -= damage;
            if (health < 0) {
                health = 0;
                playable = false;
            }
        }
    }

private:
    static olcSprite spriteSheet;
};

class Team {

private:
    int size;
    int current;

public:
    vector<Worm *> Members;

    Team(int s)
    {
        size = s;
        current = -1;
    }

    float TeamHealth()
    {
        float health = 0;
        for (auto &o : Members) {
            health += o->health;
        }
        return health;
    }

    bool IsTeamAlive()
    {
        return TeamHealth() > 0;
    }

    Worm *GetNextMember()
    {
        for (int i = 0; i < size; i++) {
            current = (current + 1) % size;
            if (Members[current]->health > 0) {
                return Members[current];
            }
        }
        return nullptr;
    }
};

class Missile : public PhysicsObject {

public:
    Missile(float x = 0, float y = 0) : PhysicsObject(x, y)
    {
        radius = 2.5f;
    }

    virtual void Draw(olcConsoleGameEngine * engine, float offsetX, float offsetY, bool zoom) override
    {
        if (zoom) {
            engine->DrawWireFrameModel(model, px * ((float)engine->ScreenWidth() / MapWidth), py * ((float)engine->ScreenHeight() / MapHeight), atan2(vy, vx), radius * 0.5f, FG_YELLOW);
        }
        else {
            engine->DrawWireFrameModel(model, px - offsetX, py - offsetY, atan2(vy, vx), radius, FG_YELLOW);
        }
    }

    virtual void OnCollision()
    {
        dead = true;
        explode = true;
    }

    virtual void TakeDamage(float damage)
    {
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
    olcSprite sprite(L"Sprites/worms1.spr");
    return sprite;
}

vector<pair<float, float>> Dummy::model = DefineDummy();
vector<pair<float, float>> Debris::model = DefineDebris();
vector<pair<float, float>> Missile::model = DefineMissile();
olcSprite Worm::spriteSheet = DefineWorm();

class WormsGame : public olcConsoleGameEngine
{
    unsigned char *Map;
    float CameraX = 0.0f;
    float CameraY = 0.0f;
    float TargetCameraX = 0.0f;
    float TargetCameraY = 0.0f;
    int CameraBorder = 10;
    list<unique_ptr<PhysicsObject>> Objects;
    int SelectedTeam = 0;
    int PreviousTeam = -1;
    PhysicsObject *SelectedUnit = nullptr;
    PhysicsObject *CameraFollowing = nullptr;
    bool PlayerHasControl = false;
    bool PlayerActionComplete = false;
    bool WorldIsStable = false;
    bool ZoomMode = false;
    vector<unique_ptr<Team>> Teams;

    enum GAMESTATE {
        GS_RESET,
        GS_CREATING_MAP,
        GS_DEPLOY_UNITS,
        GS_DEPLOYING_UNITS,
        GS_UNITS_DEPLOYED,
        GS_PLAYER_CONTROL,
        GS_CAMERA_MOVEMENT,
        GS_GAME_OVER
    } GameState, NextGameState;

    virtual bool OnUserCreate() override
    {
        Map = new unsigned char[MapWidth * MapHeight];
        GameState = GS_RESET;
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        const float cameraSpeed = 400.0f;
        const float cameraFollowSpeed = 4.0f;

        const int teamCount = 3;
        const int teamSize = 1;

        switch (GameState) {
        case GS_RESET:
        {
            PlayerHasControl = false;
            PlayerActionComplete = false;
            WorldIsStable = false;
            NextGameState = GS_CREATING_MAP;
        }
        break;
        case GS_CREATING_MAP:
        {
            PlayerHasControl = false;
            GenerateMap();
            NextGameState = GS_DEPLOY_UNITS;
        }
        break;
        case GS_DEPLOY_UNITS:
        {
            PlayerHasControl = false;
            for (int team = 0; team < teamCount; team++) {
                Team *newTeam = new Team(teamSize);
                for (int i = 0; i < teamSize; i++) {
                    float deployPos = (team + 1) * ((float)MapWidth / (teamCount + 2));
                    float deployArea = (float)MapWidth / (teamCount + 2) * 0.5f;
                    Worm * worm = new Worm(deployPos + (float)rand() / RAND_MAX * deployArea, 0);
                    worm->team = team;
                    Objects.push_back(unique_ptr<Worm>((Worm*)worm));
                    newTeam->Members.push_back(worm);
                }
                Teams.push_back(unique_ptr<Team>(newTeam));
            }
            SelectedTeam = 0;
            SelectedUnit = Teams[SelectedTeam]->GetNextMember();
            CameraFollowing = SelectedUnit;
            NextGameState = GS_DEPLOYING_UNITS;
        }
        break;
        case GS_DEPLOYING_UNITS:
        {
            PlayerHasControl = false;
            if (WorldIsStable) {
                NextGameState = GS_PLAYER_CONTROL;
            }
        }
        break;
        case GS_PLAYER_CONTROL:
        {
            PlayerHasControl = true;
            if (PlayerActionComplete) {
                NextGameState = GS_CAMERA_MOVEMENT;
            }
        }
        break;
        case GS_CAMERA_MOVEMENT:
        {
            PlayerHasControl = false;
            PlayerActionComplete = false;
            if (WorldIsStable) {
                int countTeamsAlive = 0;
                for (auto &t : Teams) {
                    if (t->IsTeamAlive()) {
                        countTeamsAlive++;
                    }
                }
                if (countTeamsAlive > 0) {
                    for (int i = 0; i < teamCount; i++) {
                        SelectedTeam = (SelectedTeam + 1) % teamCount;
                        if (Teams[SelectedTeam]->IsTeamAlive()) { // skip dead teams
                            SelectedUnit = Teams[SelectedTeam]->GetNextMember();
                            break;
                        }
                    }
                    CameraFollowing = SelectedUnit;
                }
                if (countTeamsAlive > 1) {
                    NextGameState = GS_PLAYER_CONTROL;
                }
                else {
                    NextGameState = GS_GAME_OVER;
                }
            }
        }
        break;
        case GS_GAME_OVER:
        {
            PlayerHasControl = false;
            PlayerActionComplete = false;
        }
        break;
        }

        GameState = NextGameState;

        // Debug Controls

        if (m_keys[L'M'].bReleased) {
            GenerateMap();
        }

        if (m_mouse[2].bReleased) {
            Objects.push_back(unique_ptr<Missile>(new Missile(m_mousePosX + CameraX, m_mousePosY + CameraY)));
        }

        if (m_mouse[1].bReleased) {
            Explosion(m_mousePosX + CameraX, m_mousePosY + CameraY, 10);
        }


        if (m_keys[VK_TAB].bReleased) {
            ZoomMode = !ZoomMode;
        }

        //if (m_mouse[0].bReleased) {
        //    PhysicsObject *worm = new Worm(m_mousePosX + CameraX, m_mousePosY + CameraY);
        //    Objects.push_back(unique_ptr<Worm>((Worm*)worm));
        //    SelectedUnit = worm;
        //    CameraFollowing = worm;
        //}


        if (CameraFollowing) {
            TargetCameraX = CameraFollowing->px - ScreenWidth() / 2;
            TargetCameraY = CameraFollowing->py - ScreenHeight() / 2;
            CameraX += (TargetCameraX - CameraX) * cameraFollowSpeed * fElapsedTime;
            CameraY += (TargetCameraY - CameraY) * cameraFollowSpeed * fElapsedTime;
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

        if (SelectedUnit && PlayerHasControl) {
            Worm *worm = (Worm *)SelectedUnit;
            if (m_keys[L'Z'].bPressed) {
                SelectedUnit->stable = false;
                SelectedUnit->vx = 4.0f * cosf(worm->shootAngle);
                SelectedUnit->vy = 8.0f * sinf(worm->shootAngle);
            }
            if (m_keys[L'A'].bHeld) {
                worm->shootAngle -= 1.5f * fElapsedTime;
            }
            if (m_keys[L'S'].bHeld) {
                worm->shootAngle += 1.5f * fElapsedTime;
            }

            bool shoot = false;
            if (m_keys[VK_SPACE].bPressed) {
                worm->energizing = true;
            }
            if (m_keys[VK_SPACE].bHeld) {
                if (worm->energizing) {
                    worm->energy += fElapsedTime * 0.6f;
                    if (worm->energy >= 1.0f) {
                        shoot = true;
                    }
                }
            }
            if (m_keys[VK_SPACE].bReleased) {
                if (worm->energizing) {
                    shoot = true;
                }
            }
            if (shoot) {
                PhysicsObject *missile = new Missile(worm->px, worm->py);
                missile->vx = worm->energy * 40.0f * cosf(worm->shootAngle);
                missile->vy = worm->energy * 40.0f * sinf(worm->shootAngle);
                CameraFollowing = missile;
                Objects.push_back(unique_ptr<Missile>((Missile *)missile));
                shoot = false;
                worm->energizing = false;
                worm->energy = 0;
                PlayerActionComplete = true;
            }
        }

        // Update objects
        for (auto &o : Objects) {
            // Physics steps
            for (int step = 0; step < 10; step++) {
                bool collided = false;
                if (!o->stable) {
                    o->ay += 2.0f;
                    o->vx += o->ax * fElapsedTime;
                    o->vy += o->ay * fElapsedTime;
                    o->ax = 0.0f;
                    o->ay = 0.0f;
                }
                o->stable = false;
                float potentialX = o->px + o->vx * fElapsedTime;
                float potentialY = o->py + o->vy * fElapsedTime;
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

                    o->OnCollision();
                    if (o->explode) {
                        Explosion(o->px, o->py, 20);
                        CameraFollowing = nullptr;
                    }
                }
                else {
                    o->px = potentialX;
                    o->py = potentialY;
                }
            }
        }

        WorldIsStable = true;
        for (auto &o : Objects) {
            if (!o->stable) {
                WorldIsStable = false;
                break;
            }
        }

        Objects.remove_if([](unique_ptr<PhysicsObject> &o) { return o->dead; });

        // Draw terrain
        for (int y = 0; y < ScreenHeight(); y++) {
            for (int x = 0; x < ScreenWidth(); x++) {
                short ColorTerrain = FG_DARK_GREEN;
                short ColorSky = FG_CYAN;
                short PixelTerrain = PIXEL_SOLID;
                short PixelSky = PIXEL_SOLID;

                int mapY = (int)(CameraY + y);
                int mapX = (int)(CameraX + x);

                if (ZoomMode) {
                    mapY = (int)((float)y / ScreenHeight() * MapHeight);
                    mapX = (int)((float)x / ScreenWidth() * MapWidth);
                }

                switch (mapY / 32) {
                case 0: PixelSky = PIXEL_SOLID; ColorSky = FG_DARK_BLUE; break;
                case 1: PixelSky = PIXEL_QUARTER; ColorSky = FG_BLUE | BG_DARK_BLUE; break;
                case 2: PixelSky = PIXEL_HALF; ColorSky = FG_BLUE | BG_DARK_BLUE; break;
                case 3: PixelSky = PIXEL_THREEQUARTERS; ColorSky = FG_BLUE | BG_DARK_BLUE; break;
                case 4: PixelSky = PIXEL_SOLID; ColorSky = FG_BLUE; break;
                case 5: PixelSky = PIXEL_QUARTER; ColorSky = FG_CYAN | BG_BLUE; break;
                case 6: PixelSky = PIXEL_HALF; ColorSky = FG_CYAN | BG_BLUE; break;
                case 7: PixelSky = PIXEL_THREEQUARTERS; ColorSky = FG_CYAN | BG_BLUE; break;
                default: PixelSky = PIXEL_SOLID; ColorSky = FG_CYAN; break;
                }

                bool isTerrain = Map[mapY * MapWidth + mapX] > 0;
                Draw(x, y, isTerrain ? PixelTerrain : PixelSky, isTerrain ? ColorTerrain : ColorSky);
            }
        }

        // Draw health bar
        short barColor[4] = { FG_RED, FG_BLUE, FG_MAGENTA, FG_GREEN };
        int barY = 5;
        int teamIndex = 0;
        for (auto &t : Teams) {
            int barSize = (ScreenWidth() - 10) / teamSize * t->TeamHealth();
            Fill(5, barY, barSize + 5, barY + 3, PIXEL_SOLID, barColor[teamIndex % 4]);
            barY += 5;
            teamIndex++;
        }

        // Draw objects
        for (auto &o : Objects) {
            o->Draw(this, CameraX, CameraY, ZoomMode);
        }

        for (int digit = 0; digit < 10; digit++) {
            DrawDigit(digit, 5 + digit * 10, 25);
        }

        if (SelectedUnit && SelectedUnit->stable) {
            Worm *worm = (Worm *)SelectedUnit;
            float CrossHairX = SelectedUnit->px - CameraX + cosf(worm->shootAngle) * 10.0f;
            float CrossHairY = SelectedUnit->py - CameraY + sinf(worm->shootAngle) * 10.0f;
            if (ZoomMode) {
                CrossHairX = SelectedUnit->px * ((float)ScreenWidth() / MapWidth) + cosf(worm->shootAngle) * 10.0f;
                CrossHairY = SelectedUnit->py * ((float)ScreenHeight() / MapHeight) + sinf(worm->shootAngle) * 10.0f;
            }
            Draw(CrossHairX, CrossHairY, PIXEL_SOLID, FG_BLACK);
            Draw(CrossHairX - 1, CrossHairY, PIXEL_SOLID, FG_BLACK);
            Draw(CrossHairX + 1, CrossHairY, PIXEL_SOLID, FG_BLACK);
            Draw(CrossHairX, CrossHairY - 1, PIXEL_SOLID, FG_BLACK);
            Draw(CrossHairX, CrossHairY + 1, PIXEL_SOLID, FG_BLACK);
        }

        return true;
    }

    void Explosion(float x, float y, float radius)
    {
        const float baseDamage = 0.5f;
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
                o->TakeDamage(distance * baseDamage);
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

    void DrawDigit(int digit, int left, int top, int segSize = 4, int padSize = 1)
    {
        // order (rows 1, 2 and 3, columns up-left, up-right, down-left, down-right)
        int segments[10][7] = {
            { 1, 0, 1, 1, 1, 1, 1 }, // 0
            { 0, 0, 0, 0, 1, 0, 1 }, // 1
            { 1, 1, 1, 0, 1, 1, 0 }, // 2
            { 1, 1, 1, 0, 1, 0, 1 }, // 3
            { 0, 1, 0, 1, 1, 0, 1 }, // 4
            { 1, 1, 1, 1, 0, 0, 1 }, // 5
            { 1, 1, 1, 1, 0, 1, 1 }, // 6
            { 1, 0, 0, 0, 1, 0, 1 }, // 7
            { 1, 1, 1, 1, 1, 1, 1 }, // 8
            { 1, 1, 1, 1, 1, 0, 1 }  // 9
        };

        // Horizontal segments
        for (int row = 0; row < 3; row++) {
            bool segOn = segments[digit][row] > 0;
            int x = left + padSize;
            int y = top + row * segSize;
            if (segOn) {
                DrawLine(x, y, x + segSize - 2 * padSize, y, PIXEL_SOLID, segOn ? FG_BLACK : FG_BLUE);
            }
        }

        // Vertical segments
        for (int row = 0; row < 2; row++) {
            for (int col = 0; col < 2; col++) {
                bool segOn = segments[digit][3 + row * 2 + col] > 0;
                int x = left + col * segSize;
                int y = top + padSize + row * segSize;
                if (segOn) {
                    DrawLine(x, y, x, y + segSize - 2 * padSize, PIXEL_SOLID, segOn ? FG_BLACK : FG_BLUE);
                }
            }
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