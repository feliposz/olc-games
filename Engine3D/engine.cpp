#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <fstream>
#include <strstream>
#include <algorithm>

struct vec3d
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
};

struct triangle
{
    vec3d p[3];
    olc::Pixel color;
};

vec3d Vector_Add(vec3d a, vec3d b)
{
    vec3d c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
}

vec3d Vector_Sub(vec3d a, vec3d b)
{
    vec3d c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return c;
}

vec3d Vector_Mul(vec3d a, float b)
{
    vec3d c;
    c.x = a.x * b;
    c.y = a.y * b;
    c.z = a.z * b;
    return c;
}

vec3d Vector_Div(vec3d a, float b)
{
    vec3d c;
    c.x = a.x / b;
    c.y = a.y / b;
    c.z = a.z / b;
    return c;
}

float Vector_DotProduct(vec3d a, vec3d b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Vector_Length(vec3d a)
{
    return sqrtf(Vector_DotProduct(a, a));
}

vec3d Vector_Normalize(vec3d a)
{
    vec3d c;
    float length = Vector_Length(a);
    c.x = a.x / length;
    c.y = a.y / length;
    c.z = a.z / length;
    return c;
}

vec3d Vector_CrossProduct(vec3d a, vec3d b)
{
    return { a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x };
}

vec3d Vector_IntersectPlane(vec3d planePos, vec3d planeNormal, vec3d lineStart, vec3d lineEnd)
{
    planeNormal = Vector_Normalize(planeNormal);
    float planeD = Vector_DotProduct(planeNormal, planePos);
    float aD = Vector_DotProduct(lineStart, planeNormal);
    float bD = Vector_DotProduct(lineEnd, planeNormal);
    float t = (planeD - aD) / (bD - aD);
    vec3d lineStartToEnd = Vector_Sub(lineEnd, lineStart);
    vec3d lineToIntersect = Vector_Mul(lineStartToEnd, t);
    return Vector_Add(lineStart, lineToIntersect);
}

int Triangle_ClipAgainstPlane(vec3d planePos, vec3d planeNormal, triangle &inTri, triangle &out1, triangle &out2, bool enableDebug = false)
{
    int insideCount = 0;
    int outsideCount = 0;
    vec3d insidePoints[3];
    vec3d outsidePoints[3];

    // Classify which points are "inside" and "outside" the clipping plane
    for (int i = 0; i < 3; i++)
    {
        if (Vector_DotProduct(planeNormal, Vector_Sub(inTri.p[i], planePos)) > 0)
        {
            insidePoints[insideCount++] = inTri.p[i];
        }
        else
        {
            outsidePoints[outsideCount++] = inTri.p[i];
        }
    }

    if (insideCount == 3)
    {
        out1 = inTri;
        return 1;
    }
    else if (insideCount == 1)
    {
        out1.p[0] = insidePoints[0];
        out1.p[1] = Vector_IntersectPlane(planePos, planeNormal, insidePoints[0], outsidePoints[0]);
        out1.p[2] = Vector_IntersectPlane(planePos, planeNormal, insidePoints[0], outsidePoints[1]);
        out1.color = enableDebug ? olc::BLUE : inTri.color;
        return 1;
    }
    else if (insideCount == 2)
    {
        vec3d newP2 = Vector_IntersectPlane(planePos, planeNormal, insidePoints[0], outsidePoints[0]);
        vec3d newP0 = Vector_IntersectPlane(planePos, planeNormal, insidePoints[1], outsidePoints[0]);

        out1.p[0] = insidePoints[0];
        out1.p[1] = insidePoints[1];
        out1.p[2] = newP2;
        out1.color = enableDebug ? olc::GREEN : inTri.color;

        out2.p[0] = insidePoints[1];
        out2.p[1] = newP2;
        out2.p[2] = newP0;
        out2.color = enableDebug ? olc::RED : inTri.color;

        return 2;
    }

    return 0;
}

vec3d Triangle_Normal(triangle tri)
{
    vec3d a = Vector_Sub(tri.p[1], tri.p[0]);
    vec3d b = Vector_Sub(tri.p[2], tri.p[0]);
    vec3d n = Vector_CrossProduct(a, b);
    return Vector_Normalize(n);
}

struct mat4x4
{
    float m[4][4] = { 0 };
};

mat4x4 Matrix_MakeIdentity()
{
    mat4x4 id = { 0 };;
    id.m[0][0] = 1;
    id.m[1][1] = 1;
    id.m[2][2] = 1;
    id.m[3][3] = 1;
    return id;
}

mat4x4 Matrix_MakeRotationX(float a)
{
    mat4x4 rotX = { 0 };;
    rotX.m[0][0] = 1;
    rotX.m[1][1] = cosf(a);
    rotX.m[1][2] = sinf(a);
    rotX.m[2][1] = -sinf(a);
    rotX.m[2][2] = cosf(a);
    rotX.m[3][3] = 1;
    return rotX;
}

mat4x4 Matrix_MakeRotationY(float a)
{
    mat4x4 rotY = { 0 };
    rotY.m[1][1] = 1;
    rotY.m[0][0] = cosf(a);
    rotY.m[0][2] = sinf(a);
    rotY.m[2][0] = -sinf(a);
    rotY.m[2][2] = cosf(a);
    rotY.m[3][3] = 1;
    return rotY;
}

mat4x4 Matrix_MakeRotationZ(float a)
{
    mat4x4 rotZ = { 0 };
    rotZ.m[0][0] = cosf(a);
    rotZ.m[0][1] = sinf(a);
    rotZ.m[1][0] = -sinf(a);
    rotZ.m[1][1] = cosf(a);
    rotZ.m[2][2] = 1;
    rotZ.m[3][3] = 1;
    return rotZ;
}

mat4x4 Matrix_MakeTranslation(vec3d a)
{
    mat4x4 tran = { 0 };
    tran.m[0][0] = 1;
    tran.m[1][1] = 1;
    tran.m[2][2] = 1;
    tran.m[3][3] = 1;
    tran.m[3][0] = a.x;
    tran.m[3][1] = a.y;
    tran.m[3][2] = a.z;
    return tran;
}

mat4x4 Matrix_MakeProjection(float aspectRatio, float zNear, float zFar, float fov)
{
    mat4x4 proj = { 0 };
    float fovRadians = (fov / 180.0f) * 3.1415926f;
    float F = 1.0f / tanf(fovRadians * 0.5f);
    float q = zFar / (zFar - zNear);
    proj.m[0][0] = aspectRatio * F;
    proj.m[1][1] = F;
    proj.m[2][2] = q;
    proj.m[3][2] = -zNear * q;
    proj.m[2][3] = 1.0f;
    return proj;
}

vec3d Matrix_MultiplyVector(vec3d i, mat4x4 &m)
{
    vec3d o;
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    o.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
    return o;
}

mat4x4 Matrix_MultiplyMatrix(mat4x4 &a, mat4x4 &b)
{
    mat4x4 c;
    c.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0];
    c.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0];
    c.m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0];
    c.m[3][0] = a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] + a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0];
    c.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1];
    c.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1];
    c.m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1];
    c.m[3][1] = a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] + a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1];
    c.m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2];
    c.m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2];
    c.m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2];
    c.m[3][2] = a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2];
    c.m[0][3] = a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] + a.m[0][2] * b.m[2][3] + a.m[0][3] * b.m[3][3];
    c.m[1][3] = a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] + a.m[1][2] * b.m[2][3] + a.m[1][3] * b.m[3][3];
    c.m[2][3] = a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] + a.m[2][2] * b.m[2][3] + a.m[2][3] * b.m[3][3];
    c.m[3][3] = a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] + a.m[3][2] * b.m[2][3] + a.m[3][3] * b.m[3][3];
    return c;
}

mat4x4 Matrix_PointAt(vec3d &pos, vec3d &target, vec3d &up)
{
    vec3d nFwd = Vector_Normalize(Vector_Sub(target, pos));
    vec3d nUp = Vector_Normalize(Vector_Sub(up, Vector_Mul(nFwd, Vector_DotProduct(up, nFwd))));
    vec3d nRight = Vector_CrossProduct(nUp, nFwd);

    mat4x4 pa;

    pa.m[0][0] = nRight.x;
    pa.m[0][1] = nRight.y;
    pa.m[0][2] = nRight.z;
    pa.m[0][3] = 0;

    pa.m[1][0] = nUp.x;
    pa.m[1][1] = nUp.y;
    pa.m[1][2] = nUp.z;
    pa.m[1][3] = 0;

    pa.m[2][0] = nFwd.x;
    pa.m[2][1] = nFwd.y;
    pa.m[2][2] = nFwd.z;
    pa.m[2][3] = 0;

    pa.m[3][0] = pos.x;
    pa.m[3][1] = pos.y;
    pa.m[3][2] = pos.z;
    pa.m[3][3] = 1;

    return pa;
}

mat4x4 Matrix_QuickInverse(mat4x4 &m)
{
    mat4x4 qi;
    // Only for Rotation / Translation Matrices
    qi.m[0][0] = m.m[0][0];
    qi.m[0][1] = m.m[1][0];
    qi.m[0][2] = m.m[2][0];
    qi.m[0][3] = 0;

    qi.m[1][0] = m.m[0][1];
    qi.m[1][1] = m.m[1][1];
    qi.m[1][2] = m.m[2][1];
    qi.m[1][3] = 0;

    qi.m[2][0] = m.m[0][2];
    qi.m[2][1] = m.m[1][2];
    qi.m[2][2] = m.m[2][2];
    qi.m[2][3] = 0;

    qi.m[3][0] = -(m.m[3][0] * qi.m[0][0] + m.m[3][1] * qi.m[1][0] + m.m[3][2] * qi.m[2][0]);
    qi.m[3][1] = -(m.m[3][0] * qi.m[0][1] + m.m[3][1] * qi.m[1][1] + m.m[3][2] * qi.m[2][1]);
    qi.m[3][2] = -(m.m[3][0] * qi.m[0][2] + m.m[3][1] * qi.m[1][2] + m.m[3][2] * qi.m[2][2]);
    qi.m[3][3] = 1;
    return qi;
}

struct mesh
{
    std::vector<triangle> tris;

    bool LoadFromOBJFile(std::string filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        std::vector<vec3d> vertices;

        while (!file.eof())
        {
            char line[256];
            file.getline(line, 256);

            char prefix;
            vec3d v;

            std::strstream ss(line, 256);
            ss >> prefix;

            if (prefix == 'v')
            {
                ss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            else if (prefix == 'f')
            {
                int a, b, c;
                ss >> a >> b >> c;
                a--;
                b--;
                c--;
                tris.push_back({ vertices[a], vertices[b], vertices[c] });
            }
        }

        file.close();

        return true;
    }
};


class Engine3D : public olc::PixelGameEngine
{
    float zNear = 0.1f;

    mesh *obj;
    mesh cube;
    mesh ship;
    mesh axis;
    mesh teapot;
    mesh mountains;

    mat4x4 proj;
    vec3d camera;
    vec3d lookDir;
    vec3d light;
    float time;
    float yaw;
    bool enableRotation;
    bool enableDebug;
    bool enableWireframe;

public:
    Engine3D()
    {
        sAppName = "Engine 3D";
    }

    bool OnUserCreate() override
    {
        cube.tris = {
            // SOUTH
            { 0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
            // EAST
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
            // NORTH
            { 1.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
            // WEST
            { 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f },
            // TOP
            { 0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f },
            // BOTTOM
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
        };

        ship.LoadFromOBJFile("VideoShip.obj");
        teapot.LoadFromOBJFile("teapot.obj");
        axis.LoadFromOBJFile("axis.obj");
        mountains.LoadFromOBJFile("mountains.obj");

        obj = &axis;

        float aspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
        float zFar = 1000.0f;
        float fov = 90.0f;
        proj = Matrix_MakeProjection(aspectRatio, zNear, zFar, fov);

        camera = { 0, 0, 0 };
        light = { 2, 1, -1 };
        light = Vector_Normalize(light);
        yaw = 0;
        enableRotation = false;
        enableDebug = false;
        enableWireframe = false;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        int32_t height = ScreenHeight();

        if (GetKey(olc::UP).bHeld)
        {
            camera.y += fElapsedTime * 8.0f;
        }
        if (GetKey(olc::DOWN).bHeld)
        {
            camera.y -= fElapsedTime * 8.0f;
        }

        if (GetKey(olc::RIGHT).bHeld)
        {
            camera.x += fElapsedTime * 8.0f;
        }
        if (GetKey(olc::LEFT).bHeld)
        {
            camera.x -= fElapsedTime * 8.0f;
        }

        if (GetKey(olc::A).bHeld)
        {
            yaw += fElapsedTime * 2.0f;
        }
        if (GetKey(olc::D).bHeld)
        {
            yaw -= fElapsedTime * 2.0f;
        }

        vec3d forwardMove = Vector_Mul(lookDir, 8.0f * fElapsedTime);
        vec3d rightMove = Vector_CrossProduct(forwardMove, { 0, 1, 0 });

        if (GetKey(olc::W).bHeld)
        {
            camera = Vector_Add(camera, forwardMove);
        }
        if (GetKey(olc::S).bHeld)
        {
            camera = Vector_Sub(camera, forwardMove);
        }
        if (GetKey(olc::Q).bHeld)
        {
            camera = Vector_Add(camera, rightMove);
        }
        if (GetKey(olc::E).bHeld)
        {
            camera = Vector_Sub(camera, rightMove);
        }

        if (GetKey(olc::K1).bPressed)
        {
            obj = &axis;
        }
        if (GetKey(olc::K2).bPressed)
        {
            obj = &teapot;
        }
        if (GetKey(olc::K3).bPressed)
        {
            obj = &ship;
        }
        if (GetKey(olc::K4).bPressed)
        {
            obj = &cube;
        }
        if (GetKey(olc::K5).bPressed)
        {
            obj = &mountains;
        }
        if (GetKey(olc::R).bPressed)
        {
            enableRotation = !enableRotation;
        }
        if (GetKey(olc::G).bPressed)
        {
            enableDebug = !enableDebug;
        }
        if (GetKey(olc::Z).bPressed)
        {
            enableWireframe = !enableWireframe;
        }

        FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

        float offX = 1.0f;
        float offY = 1.0f;
        float scale = 0.5f * (float)ScreenWidth();

        time += fElapsedTime;

        float a = enableRotation ? time : 0;
        float b = enableRotation ? time * 0.5f : 0;

        std::vector<triangle> renderTris;

        mat4x4 rotX = Matrix_MakeRotationX(b);
        mat4x4 rotZ = Matrix_MakeRotationZ(a);
        mat4x4 tran = Matrix_MakeTranslation({ 0, 0, 5.0f });

        mat4x4 world = Matrix_MultiplyMatrix(rotZ, rotX);
        world = Matrix_MultiplyMatrix(world, tran);

        vec3d up = { 0, 1, 0 };
        vec3d target = { 0, 0, 1 };
        mat4x4 cameraRot = Matrix_MakeRotationY(yaw);
        lookDir = Matrix_MultiplyVector(target, cameraRot);
        target = Vector_Add(camera, lookDir);
        mat4x4 pointAt = Matrix_PointAt(camera, target, up);
        mat4x4 view = Matrix_QuickInverse(pointAt);

        for (auto tri : obj->tris)
        {
            triangle triWorld;
            triWorld.p[0] = Matrix_MultiplyVector(tri.p[0], world);
            triWorld.p[1] = Matrix_MultiplyVector(tri.p[1], world);
            triWorld.p[2] = Matrix_MultiplyVector(tri.p[2], world);

            vec3d normal = Triangle_Normal(triWorld);
            vec3d visibility = Vector_Sub(triWorld.p[0], camera);
            float culling = Vector_DotProduct(normal, visibility);

            if (culling < 0)
            {
                triangle triView;
                triView.p[0] = Matrix_MultiplyVector(triWorld.p[0], view);
                triView.p[1] = Matrix_MultiplyVector(triWorld.p[1], view);
                triView.p[2] = Matrix_MultiplyVector(triWorld.p[2], view);

                // shading
                float fShade = Vector_DotProduct(normal, light);
                fShade = fShade < 0.1f ? 0.1f : (fShade > 1.0f ? 1.0f : fShade);
                uint8_t shade = (uint8_t)(255.0f * fShade);
                triView.color = { shade, shade, shade };

                // clip triangles against camera "near" plane

                triangle triClipped[2];
                vec3d nearPlanePos = { 0, 0, zNear };
                vec3d nearPlaneNormal = { 0, 0, 1 }; // forward
                int numTriangles = Triangle_ClipAgainstPlane(nearPlanePos, nearPlaneNormal, triView, triClipped[0], triClipped[1], enableDebug);

                for (int i = 0; i < numTriangles; i++)
                {
                    triangle triProj;
                    triProj.p[0] = Matrix_MultiplyVector(triClipped[i].p[0], proj);
                    triProj.p[1] = Matrix_MultiplyVector(triClipped[i].p[1], proj);
                    triProj.p[2] = Matrix_MultiplyVector(triClipped[i].p[2], proj);
                    triProj.color = triClipped[i].color;

                    // Normalize projection
                    triProj.p[0] = Vector_Div(triProj.p[0], triProj.p[0].w);
                    triProj.p[1] = Vector_Div(triProj.p[1], triProj.p[1].w);
                    triProj.p[2] = Vector_Div(triProj.p[2], triProj.p[2].w);

                    // offset and scale to screen coords

                    triProj.p[0] = Vector_Add(triProj.p[0], { offX, offY, 0, 0 });
                    triProj.p[1] = Vector_Add(triProj.p[1], { offX, offY, 0, 0 });
                    triProj.p[2] = Vector_Add(triProj.p[2], { offX, offY, 0, 0 });

                    triProj.p[0] = Vector_Mul(triProj.p[0], scale);
                    triProj.p[1] = Vector_Mul(triProj.p[1], scale);
                    triProj.p[2] = Vector_Mul(triProj.p[2], scale);

                    renderTris.push_back(triProj);
                }
            }
        }

        // sort by the average z of the triangles
        std::sort(renderTris.begin(), renderTris.end(), [](triangle a, triangle b)
        {
            float za = (a.p[0].z + a.p[1].z + a.p[2].z) / 3.0f;
            float zb = (b.p[0].z + b.p[1].z + b.p[2].z) / 3.0f;
            return za > zb;
        });

        // clip triangles agains screen border "planes"
        vec3d planePos[4] = { {0, 0, 0}, { 0, ScreenHeight() - 1, 0 }, { 0, 0, 0 }, { ScreenWidth() - 1, 0, 0 } };
        vec3d planeNormal[4] = { { 0, 1, 0 }, { 0, -1, 0 }, { 1, 0, 0 }, { -1, 0, 0 } };

        // render resulting triangles

        for (auto tri : renderTris)
        {
            std::list<triangle> current;
            std::list<triangle> next;

            // place current triangle into queue to be clipped agains 4 planes
            current.push_back(tri);

            for (int plane = 0; plane < 4; plane++)
            {
                while (current.size() > 0)
                {
                    triangle test = current.front();
                    current.pop_front();

                    // clip agains current plane
                    triangle triClipped[2];
                    int numTriangles = Triangle_ClipAgainstPlane(planePos[plane], planeNormal[plane], test, triClipped[0], triClipped[1], enableDebug);

                    // place triangles on the next queue to be clipped
                    for (int i = 0; i < numTriangles; i++)
                    {
                        next.push_back(triClipped[i]);
                    }
                }
                // move to next plane queue
                current = next;
                next.clear();
            }

            for (auto tri : current)
            {
                // reverse Y!
                if (enableWireframe)
                {
                    DrawTriangle(tri.p[0].x, height - tri.p[0].y, tri.p[1].x, height - tri.p[1].y, tri.p[2].x, height - tri.p[2].y, olc::YELLOW);
                }
                else
                {
                    FillTriangle(tri.p[0].x, height - tri.p[0].y, tri.p[1].x, height - tri.p[1].y, tri.p[2].x, height - tri.p[2].y, tri.color);
                }
            }
        }

        if (enableDebug)
        {
            char Buffer[256];
            sprintf_s(Buffer, sizeof(Buffer), "C: %.2f %.2f %.2f", camera.x, camera.y, camera.z);
            DrawString(0, 0, Buffer, olc::YELLOW);
            sprintf_s(Buffer, sizeof(Buffer), "L: %.2f %.2f %.2f", lookDir.x, lookDir.y, lookDir.z);
            DrawString(0, 15, Buffer, olc::YELLOW);
            sprintf_s(Buffer, sizeof(Buffer), "T: %.2f %.2f %.2f", target.x, target.y, target.z);
            DrawString(0, 30, Buffer, olc::YELLOW);
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
    return 0;
}
