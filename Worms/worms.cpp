#include <vector>
#include <utility>
#define OLC_PGE_APPLICATION
#include "../olcPixelGameEngine.h"
using namespace std;

const float PI = 3.1415926f;
const int MapWidth = 1024;
const int MapHeight = 512;
const float CameraSpeed = 400.0f;
const float CameraFollowSpeed = 4.0f;
const int TeamCount = 4;
const int TeamSize = 4;
const float MaxTimer = 16;
const float MissileSpeed = 40.0f;
const float Gravity = 2.0f;

float CalculateDistance(float x1, float y1, float x2, float y2)
{
    return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float NormalizeAngle(float angle)
{
    while (angle < PI) {
        angle += PI * 2;
    }
    while (angle > PI) {
        angle -= PI * 2;
    }
    return angle;
}

void DrawWireFrameModel(olc::PixelGameEngine * engine, const std::vector<std::pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, olc::Pixel col = olc::WHITE)
{
    // pair.first = x coordinate
    // pair.second = y coordinate

    // Create translated model vector of coordinate pairs
    std::vector<std::pair<float, float>> vecTransformedCoordinates;
    int verts = vecModelCoordinates.size();
    vecTransformedCoordinates.resize(verts);

    // Rotate
    for (int i = 0; i < verts; i++)
    {
        vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
        vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
    }

    // Scale
    for (int i = 0; i < verts; i++)
    {
        vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
        vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
    }

    // Translate
    for (int i = 0; i < verts; i++)
    {
        vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
        vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
    }

    // Draw Closed Polygon
    for (int i = 0; i < verts + 1; i++)
    {
        int j = (i + 1);
        engine->DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
            (int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, col);
    }
}

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

    virtual void Draw(olc::PixelGameEngine *engine, float offsetX, float offsetY, bool zoom) = 0;
    virtual void OnCollision() = 0;
    virtual void TakeDamage(float damage) = 0;
};

class Dummy : public PhysicsObject {

public:
    Dummy(float x = 0, float y = 0) : PhysicsObject(x, y)
    {
    }

    virtual void Draw(olc::PixelGameEngine * engine, float offsetX, float offsetY, bool zoom) override
    {
        if (zoom) {
            DrawWireFrameModel(engine, model, px * ((float)engine->ScreenWidth() / MapWidth), py * ((float)engine->ScreenHeight() / MapHeight), atan2(vy, vx), radius * 0.5f);
        }
        else {
            DrawWireFrameModel(engine, model, px - offsetX, py - offsetY, atan2(vy, vx), radius);
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

    virtual void Draw(olc::PixelGameEngine * engine, float offsetX, float offsetY, bool zoom) override
    {
        if (zoom) {
            DrawWireFrameModel(engine, model, px * ((float)engine->ScreenWidth() / MapWidth), py * ((float)engine->ScreenHeight() / MapHeight), atan2(vy, vx), radius * 0.5f, olc::DARK_GREEN);
        }
        else {
            DrawWireFrameModel(engine, model, px - offsetX, py - offsetY, atan2(vy, vx), radius, olc::DARK_GREEN);
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

    virtual void Draw(olc::PixelGameEngine * engine, float offsetX, float offsetY, bool zoom) override
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

        engine->SetPixelMode(olc::Pixel::MASK);
        if (health > 0) {
            engine->DrawPartialSprite(posX + offsetSpriteX, posY + offsetSpriteY, spriteSheet, team * spriteW, 0, spriteW, spriteH);
            engine->DrawLine(posX + offsetHealthX, posY + offsetHealthY, posX + offsetHealthX + (int)(spriteW * health), posY + offsetHealthY, olc::BLUE);
            if (energizing) {
                engine->DrawLine(posX + offsetEnergyX, posY + offsetEnergyY, posX + offsetEnergyX + (int)(spriteW * energy), posY + offsetEnergyY, olc::RED);
                engine->DrawLine(posX + offsetEnergyX, posY + offsetEnergyY + 1, posX + offsetEnergyX + (int)(spriteW * energy), posY + offsetEnergyY + 1, olc::GREEN);
            }
        }
        else {
            engine->DrawPartialSprite(posX + offsetSpriteX, posY + offsetSpriteY, spriteSheet, team * spriteW, spriteH, spriteW, spriteH);
        }
        engine->SetPixelMode(olc::Pixel::NORMAL);
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

    static olc::Sprite *spriteSheet;
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

    virtual void Draw(olc::PixelGameEngine * engine, float offsetX, float offsetY, bool zoom) override
    {
        if (zoom) {
            DrawWireFrameModel(engine, model, px * ((float)engine->ScreenWidth() / MapWidth), py * ((float)engine->ScreenHeight() / MapHeight), atan2(vy, vx), radius * 0.5f, olc::YELLOW);
        }
        else {
            DrawWireFrameModel(engine, model, px - offsetX, py - offsetY, atan2(vy, vx), radius, olc::YELLOW);
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

olc::Sprite *DefineWorm()
{
    olc::Sprite *sprite = new olc::Sprite("assets/worms1.png");
    return sprite;
}

vector<pair<float, float>> Dummy::model = DefineDummy();
vector<pair<float, float>> Debris::model = DefineDebris();
vector<pair<float, float>> Missile::model = DefineMissile();
olc::Sprite *Worm::spriteSheet = nullptr;

class WormsGame : public olc::PixelGameEngine
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
    bool PlayerAIControl = false;
    vector<unique_ptr<Team>> Teams;
    float Timer = MaxTimer;
    float AITargetPosition = 0;
    float AITargetAngle = 0;
    bool AIMoveRight = false;
    bool AIMoveLeft = false;
    float AITargetEnergy = 0.75f;
    Worm *AITargetEnemy = nullptr;

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

    enum AISTATE {
        AI_CHOOSE_STRATEGY,
        AI_POSITIONING,
        AI_CHOOSE_TARGET,
        AI_APPROACH_TARGET,
        AI_AIM,
        AI_FIRE,
        AI_IDLE
    } AIState, NextAIState;

    virtual bool OnUserCreate() override
    {
        Map = new unsigned char[MapWidth * MapHeight];
        GameState = GS_RESET;
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {

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
            for (int team = 0; team < TeamCount; team++) {
                Team *newTeam = new Team(TeamSize);
                for (int i = 0; i < TeamSize; i++) {
                    float deployPos = (team + 1) * ((float)MapWidth / (TeamCount + 2));
                    float deployArea = (float)MapWidth / (TeamCount + 2) * 0.5f;
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
            PlayerAIControl = true; // Testing - SelectedTeam == 0; 
            PlayerHasControl = true;
            Timer -= fElapsedTime;
            if (Timer < 0) {
                Timer = 0;
                PlayerActionComplete = true;
            }
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
                    for (int i = 0; i < TeamCount; i++) {
                        SelectedTeam = (SelectedTeam + 1) % TeamCount;
                        if (Teams[SelectedTeam]->IsTeamAlive()) { // skip dead teams
                            SelectedUnit = Teams[SelectedTeam]->GetNextMember();
                            break;
                        }
                    }
                    // Make sure worm is not energizing from previous turn
                    Worm *worm = (Worm *)SelectedUnit;
                    worm->energizing = false;
                    worm->energy = 0;
                    CameraFollowing = SelectedUnit;
                }
                if (countTeamsAlive > 1) {
                    NextGameState = GS_PLAYER_CONTROL;
                    AIState = AI_CHOOSE_STRATEGY;
                    Timer = 15;
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

        if (GetKey(olc::M).bReleased) {
            GenerateMap();
        }

        if (GetMouse(2).bReleased) {
            Objects.push_back(unique_ptr<Missile>(new Missile(GetMouseX() + CameraX, GetMouseY() + CameraY)));
        }

        if (GetMouse(1).bReleased) {
            Explosion(GetMouseX() + CameraX, GetMouseY() + CameraY, 10);
        }

        // Toggle zoom

        if (GetKey(olc::TAB).bReleased) {
            ZoomMode = !ZoomMode;
        }

        // Camera control

        if (CameraFollowing) {
            TargetCameraX = CameraFollowing->px - ScreenWidth() / 2;
            TargetCameraY = CameraFollowing->py - ScreenHeight() / 2;
            CameraX += (TargetCameraX - CameraX) * CameraFollowSpeed * fElapsedTime;
            CameraY += (TargetCameraY - CameraY) * CameraFollowSpeed * fElapsedTime;
        }

        if (GetMouseX() < CameraBorder) {
            CameraX -= CameraSpeed * fElapsedTime;
        }
        if (GetMouseY() < CameraBorder) {
            CameraY -= CameraSpeed * fElapsedTime;
        }
        if (GetMouseX() > ScreenWidth() - CameraBorder) {
            CameraX += CameraSpeed * fElapsedTime;
        }
        if (GetMouseY() > ScreenHeight() - CameraBorder) {
            CameraY += CameraSpeed * fElapsedTime;
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

        // Control

        if (SelectedUnit && PlayerHasControl) {
            Worm *worm = (Worm *)SelectedUnit;

            bool controlJump = GetKey(olc::Z).bPressed;
            bool controlLeft = GetKey(olc::A).bHeld;
            bool controlRight = GetKey(olc::S).bHeld;
            bool controlShootPressed = GetKey(olc::SPACE).bPressed;
            bool controlShootHeld = GetKey(olc::SPACE).bPressed;
            bool controlShootReleased = GetKey(olc::SPACE).bReleased;

            if (PlayerAIControl) {

                // Reset controls
                controlJump = false;
                controlLeft = false;
                controlRight = false;
                controlShootPressed = false;
                controlShootHeld = false;
                controlShootReleased = false;

                // AI state machine
                switch (AIState) {
                case AI_CHOOSE_STRATEGY:
                {
                    float strategy = (float)rand() / RAND_MAX;
                    if (strategy < 0.3f) {
                        // Do nothing
                        AITargetPosition = worm->px;
                    }
                    else if (strategy < 0.6f) {
                        // Walk towards middle
                        if (worm->px > MapWidth * 0.5f) {
                            AITargetPosition = worm->px - 100.0f;
                        }
                        else {
                            AITargetPosition = worm->px + 100.0f;
                        }
                    }
                    else {
                        // Walk alway from closest team mate
                        PhysicsObject *closest = nullptr;
                        float minDistance = INFINITY;
                        for (auto &t : Teams[SelectedTeam]->Members) {
                            if (t != worm && t->health > 0) {
                                float distance = CalculateDistance(t->px, t->py, worm->px, worm->py);
                                if (distance < minDistance) {
                                    minDistance = distance;
                                    closest = t;
                                }
                            }
                        }
                        if (closest) {
                            if (worm->px < closest->px) {
                                AITargetPosition = worm->px - 50.0f;
                            }
                            else {
                                AITargetPosition = worm->px + 50.0f;
                            }
                        }
                        else {
                            AITargetPosition = worm->px;
                        }
                    }
                    if (AITargetPosition < 0) {
                        AITargetPosition = 0;
                    }
                    if (AITargetPosition > ScreenWidth() - 1) {
                        AITargetPosition = ScreenWidth() - 1;
                    }
                    if (AITargetPosition > worm->px) {
                        AIMoveRight = true;
                        AIMoveLeft = false;
                    }
                    else if (AITargetPosition < worm->px) {
                        AIMoveLeft = true;
                        AIMoveRight = false;
                    }
                    NextAIState = AI_POSITIONING;
                }
                break;
                case AI_POSITIONING:
                {
                    if (Timer < 7.0f) {
                        NextAIState = AI_CHOOSE_TARGET;
                    }
                    else {

                        if (AIMoveRight) {
                            AITargetAngle = -PI * 0.25f;
                        }
                        else if (AIMoveLeft) {
                            AITargetAngle = -PI * 0.75f;
                        }

                        if (fabs(AITargetAngle - worm->shootAngle) > 0.1f) {
                            if (AITargetAngle < worm->shootAngle) {
                                controlLeft = true;
                            }
                            else if (AITargetAngle > worm->shootAngle) {
                                controlRight = true;
                            }
                        }
                        else {
                            if (AIMoveRight && AITargetPosition > worm->px) {
                                controlJump = true;
                            }
                            else if (AIMoveLeft && AITargetPosition < worm->px) {
                                controlJump = true;
                            }
                            else {
                                NextAIState = AI_CHOOSE_TARGET;
                            }
                        }
                    }
                }
                break;
                case AI_CHOOSE_TARGET:
                {
                    int targetTeam;
                    do {
                        targetTeam = rand() % TeamCount;
                        if (!Teams[targetTeam]->IsTeamAlive()) {
                            continue;
                        }
                    } while (targetTeam == SelectedTeam);

                    AITargetEnemy = nullptr;
                    float maxHealth = 0;
                    for (auto &e : Teams[targetTeam]->Members) {
                        if (e->health > maxHealth) {
                            maxHealth = e->health;
                            AITargetEnemy = e;
                        }
                    }

                    NextAIState = AI_APPROACH_TARGET;
                }
                break;
                case AI_APPROACH_TARGET:
                {
                    if (Timer < 5.0f) {
                        AITargetAngle = worm->shootAngle;
                        NextAIState = AI_AIM;
                    }
                    else {

                        // Check aiming angles
                        float v = AITargetEnergy * MissileSpeed;
                        float dx = -(AITargetEnemy->px - worm->px);
                        float dy = -(AITargetEnemy->py - worm->py);
                        float delta = v * v * v * v - Gravity * (Gravity * dx * dx + 2.0f * dy * v * v);

                        bool canShoot = delta >= 0 && dx != 0;

                        if (canShoot) {
                            float angle1 = NormalizeAngle(atanf((v*v + sqrtf(delta)) / (Gravity * dx)));
                            float angle2 = NormalizeAngle(atanf((v*v - sqrtf(delta)) / (Gravity * dx)));
                            AITargetAngle = angle1; // -(dx > 0 ? PI : 0); // ????
                            NextAIState = AI_AIM;
                        } 
                        else {

                            AITargetPosition = AITargetEnemy->px;
                            if (AITargetPosition > worm->px) {
                                AIMoveRight = true;
                                AIMoveLeft = false;
                            }
                            else if (AITargetPosition < worm->px) {
                                AIMoveLeft = true;
                                AIMoveRight = false;
                            }

                            // Can't reach target yet, get closer
                            if (AIMoveRight) {
                                AITargetAngle = -PI * -0.25f;
                            }
                            else if (AIMoveLeft) {
                                AITargetAngle = -PI * 0.75f;
                            }

                            if (fabs(AITargetAngle - worm->shootAngle) > 0.1f) {
                                if (AITargetAngle < worm->shootAngle) {
                                    controlLeft = true;
                                }
                                else if (AITargetAngle > worm->shootAngle) {
                                    controlRight = true;
                                }
                            }
                            else {
                                if (AIMoveRight && AITargetPosition > worm->px) {
                                    controlJump = true;
                                }
                                else if (AIMoveLeft && AITargetPosition < worm->px) {
                                    controlJump = true;
                                }
                            }
                        }
                    }
                }
                break;
                case AI_AIM:
                {
                    if (fabs(AITargetAngle - worm->shootAngle) > 0.1f) {
                        if (AITargetAngle < worm->shootAngle) {
                            controlLeft = true;
                        }
                        else if (AITargetAngle > worm->shootAngle) {
                            controlRight = true;
                        }
                    }
                    else {
                        NextAIState = AI_FIRE;
                    }
                }
                break;
                case AI_FIRE:
                {
                    if (!worm->energizing) {
                        controlShootPressed = true;
                    }
                    else if (worm->energy < AITargetEnergy) {
                        controlShootHeld = true;
                    }
                    else {
                        controlShootReleased = true;
                        NextAIState = AI_IDLE;
                    }                    
                }
                break;
                case AI_IDLE:
                {
                    // Do nothing
                }
                break;
                }
                AIState = NextAIState;
            }

            if (controlJump && SelectedUnit->stable) {
                SelectedUnit->stable = false;
                SelectedUnit->vx = 4.0f * cosf(worm->shootAngle);
                SelectedUnit->vy = 8.0f * sinf(worm->shootAngle);
            }
            if (controlLeft) {
                worm->shootAngle -= 1.5f * fElapsedTime;
            }
            if (controlRight) {
                worm->shootAngle += 1.5f * fElapsedTime;
            }
            
            worm->shootAngle = NormalizeAngle(worm->shootAngle);

            bool shoot = false;

            if (controlShootPressed) {
                worm->energizing = true;
            }
            if (controlShootHeld) {
                if (worm->energizing) {
                    worm->energy += fElapsedTime * 0.6f;
                    if (worm->energy >= 1.0f) {
                        shoot = true;
                    }
                }
            }
            if (controlShootReleased) {
                if (worm->energizing) {
                    shoot = true;
                }
            }

            if (shoot) {
                PhysicsObject *missile = new Missile(worm->px, worm->py);
                missile->vx = worm->energy * MissileSpeed * cosf(worm->shootAngle);
                missile->vy = worm->energy * MissileSpeed * sinf(worm->shootAngle);
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
                if (o->stable) break; // DEBUG
                bool collided = false;
                if (!o->stable) {
                    o->ay += Gravity;
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
            olc::Pixel ColorSky = olc::CYAN;
            int mapY = (int)(CameraY + y);
            if (ZoomMode)
            {
                mapY = (int)((float)y / ScreenHeight() * MapHeight);
            }
            ColorSky = PixelLerp(olc::DARK_BLUE, olc::CYAN, (float)mapY / MapHeight);
            for (int x = 0; x < ScreenWidth(); x++) {
                olc::Pixel ColorTerrain = olc::DARK_GREEN;                

                int mapX = (int)(CameraX + x);

                if (ZoomMode) {
                    mapX = (int)((float)x / ScreenWidth() * MapWidth);
                }

                bool isTerrain = Map[mapY * MapWidth + mapX] > 0;
                Draw(x, y, isTerrain ? ColorTerrain : ColorSky);
            }
        }

        // Draw health bar
        olc::Pixel barColor[4] = { olc::RED, olc::BLUE, olc::MAGENTA, olc::GREEN };
        int barY = 5;
        int teamIndex = 0;
        for (auto &t : Teams) {
            int barSize = (ScreenWidth() - 10) / TeamSize * t->TeamHealth();
            FillRect(5, barY, 5, 3, barColor[teamIndex % 4]);
            barY += 5;
            teamIndex++;
        }

        // Draw timer
        if (PlayerHasControl) {
            int digit0 = (int)Timer / 10;
            int digit1 = (int)Timer % 10;
            DrawDigit(digit0, 5, 25);
            DrawDigit(digit1, 12, 25);
        }

        // Draw objects
        for (auto &o : Objects) {
            o->Draw(this, CameraX, CameraY, ZoomMode);
        }

        if (SelectedUnit && PlayerHasControl) {
            Worm *worm = (Worm *)SelectedUnit;
            float CrossHairX = SelectedUnit->px - CameraX + cosf(worm->shootAngle) * 10.0f;
            float CrossHairY = SelectedUnit->py - CameraY + sinf(worm->shootAngle) * 10.0f;
            if (ZoomMode) {
                CrossHairX = SelectedUnit->px * ((float)ScreenWidth() / MapWidth) + cosf(worm->shootAngle) * 10.0f;
                CrossHairY = SelectedUnit->py * ((float)ScreenHeight() / MapHeight) + sinf(worm->shootAngle) * 10.0f;
            }
            Draw(CrossHairX, CrossHairY, olc::BLACK);
            Draw(CrossHairX - 1, CrossHairY, olc::BLACK);
            Draw(CrossHairX + 1, CrossHairY, olc::BLACK);
            Draw(CrossHairX, CrossHairY - 1, olc::BLACK);
            Draw(CrossHairX, CrossHairY + 1, olc::BLACK);
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
                DrawLine(x, y, x + segSize - 2 * padSize, y, segOn ? olc::BLACK : olc::BLUE);
            }
        }

        // Vertical segments
        for (int row = 0; row < 2; row++) {
            for (int col = 0; col < 2; col++) {
                bool segOn = segments[digit][3 + row * 2 + col] > 0;
                int x = left + col * segSize;
                int y = top + padSize + row * segSize;
                if (segOn) {
                    DrawLine(x, y, x, y + segSize - 2 * padSize, segOn ? olc::BLACK : olc::BLUE);
                }
            }
        }
    }
};

int main()
{
    WormsGame game;
    if (game.Construct(256, 160, 6, 6))
    {
        Worm::spriteSheet = DefineWorm();
        game.Start();
    }

    return 0;
}