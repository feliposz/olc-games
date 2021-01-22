#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct vec3d
{
    float x, y, z;
};

struct triangle
{
    vec3d p[3];
};

struct mesh
{
    std::vector<triangle> tris;
};

struct mat4x4
{
    float m[4][4] = { 0 };
};

void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
{
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f)
    {
        o.x /= w;
        o.y /= w;
        o.z /= w;
    }
}

vec3d CrossProduct(vec3d a, vec3d b)
{
    return { a.y * b.z - a.z * b.y,
             a.z * b.x - a.x * b.z,
             a.x * b.y - a.y * b.x };
}

float DotProduct(vec3d a, vec3d b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3d TriangleNormal(triangle tri)
{
    vec3d a = { tri.p[1].x - tri.p[0].x,
                tri.p[1].y - tri.p[0].y,
                tri.p[1].z - tri.p[0].z };
    vec3d b = { tri.p[2].x - tri.p[0].x,
                tri.p[2].y - tri.p[0].y,
                tri.p[2].z - tri.p[0].z };
    vec3d n = CrossProduct(a, b);
    float length = sqrtf(DotProduct(n, n));
    n.x /= length;
    n.y /= length;
    n.z /= length;
    return n;
}


class Engine3D : public olc::PixelGameEngine
{
    mesh cube;
    mat4x4 proj;
    mat4x4 rotZ;
    mat4x4 rotX;
    float time;

public:
    Engine3D()
    {
        sAppName = "Engine 3D";
    }

    bool OnUserCreate() override
    {
        cube.tris = {
            // SOUTH
            { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },
            // EAST
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },
            // NORTH
            { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },
            // WEST
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },
            // TOP
            { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },
            // BOTTOM
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
        };
        
        float aspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
        float zNear = 0.1f;
        float zFar = 1000.0f;
        float fov = 90.0f;
        float fovRadians = (fov / 180.0f) * 3.1415926f;
        float F = 1.0f / tanf(fovRadians * 0.5f);
        float q = zFar / (zFar - zNear);

        proj.m[0][0] = aspectRatio * F;
        proj.m[1][1] = F;
        proj.m[2][2] = q;
        proj.m[3][2] = -zNear * q;
        proj.m[2][3] = 1.0f;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

        float offX = 1.0f;
        float offY = 1.0f;
        float scale = 0.5f * (float)ScreenWidth();

        vec3d camera = { 0, 0, 0 };
        vec3d light = { 0, 0, -1 };

        time += fElapsedTime;

        float a = time;
        float b = time * 0.5f;
        
        rotZ.m[0][0] = cosf(a);
        rotZ.m[0][1] = -sinf(a);
        rotZ.m[1][0] = sinf(a);
        rotZ.m[1][1] = cosf(a);
        rotZ.m[2][2] = 1;
        rotZ.m[3][3] = 1;

        rotX.m[0][0] = 1;
        rotX.m[1][1] = cosf(b);
        rotX.m[1][2] = -sinf(b);
        rotX.m[2][1] = sinf(b);
        rotX.m[2][2] = cosf(b);
        rotX.m[3][3] = 1;
  
        for (auto tri : cube.tris)
        {
            triangle triRotZ, triRotXZ, triTran, triProj;
            MultiplyMatrixVector(tri.p[0], triRotZ.p[0], rotZ);
            MultiplyMatrixVector(tri.p[1], triRotZ.p[1], rotZ);
            MultiplyMatrixVector(tri.p[2], triRotZ.p[2], rotZ);

            MultiplyMatrixVector(triRotZ.p[0], triRotXZ.p[0], rotX);
            MultiplyMatrixVector(triRotZ.p[1], triRotXZ.p[1], rotX);
            MultiplyMatrixVector(triRotZ.p[2], triRotXZ.p[2], rotX);

            triTran = triRotXZ;
            triTran.p[0].z += 3.0f;
            triTran.p[1].z += 3.0f;
            triTran.p[2].z += 3.0f;

            vec3d normal = TriangleNormal(triTran);
            vec3d visibility = { triTran.p[0].x - camera.x,
                                 triTran.p[0].y - camera.y,
                                 triTran.p[0].z - camera.z };
            float culling = DotProduct(normal, visibility);

            if (culling < 0)
            {
                MultiplyMatrixVector(triTran.p[0], triProj.p[0], proj);
                MultiplyMatrixVector(triTran.p[1], triProj.p[1], proj);
                MultiplyMatrixVector(triTran.p[2], triProj.p[2], proj);

                triProj.p[0].x += offX; triProj.p[0].y += offY;
                triProj.p[1].x += offX; triProj.p[1].y += offY;
                triProj.p[2].x += offX; triProj.p[2].y += offY;

                triProj.p[0].x *= scale; triProj.p[0].y *= scale;
                triProj.p[1].x *= scale; triProj.p[1].y *= scale;
                triProj.p[2].x *= scale; triProj.p[2].y *= scale;

                uint8_t shade = (uint8_t) (255.0f * DotProduct(normal, light));
                olc::Pixel color = { shade, shade, shade };

                FillTriangle(triProj.p[0].x, triProj.p[0].y, triProj.p[1].x, triProj.p[1].y, triProj.p[2].x, triProj.p[2].y, color);
                //DrawTriangle(triProj.p[0].x, triProj.p[0].y, triProj.p[1].x, triProj.p[1].y, triProj.p[2].x, triProj.p[2].y, olc::BLUE);
            }
        }

        return true;
    }
};

int main()
{
    Engine3D game;
    if (game.Construct(256, 240, 4, 4) == olc::OK)
    {
        game.Start();
    }
}