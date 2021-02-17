#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <fstream>
#include <strstream>
#include <algorithm>

struct vec2d
{
    float u = 0.0f;
    float v = 0.0f;
    float w = 1.0f;
};

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
    vec2d t[3];
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

vec3d Vector_IntersectPlane(vec3d planePos, vec3d planeNormal, vec3d lineStart, vec3d lineEnd, float &t)
{
    planeNormal = Vector_Normalize(planeNormal);
    float planeD = Vector_DotProduct(planeNormal, planePos);
    float aD = Vector_DotProduct(lineStart, planeNormal);
    float bD = Vector_DotProduct(lineEnd, planeNormal);
    t = (planeD - aD) / (bD - aD);
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
    vec2d insideTex[3];
    vec2d outsideTex[3];

    // Classify which points are "inside" and "outside" the clipping plane
    for (int i = 0; i < 3; i++)
    {
        if (Vector_DotProduct(planeNormal, Vector_Sub(inTri.p[i], planePos)) > 0)
        {
            insidePoints[insideCount] = inTri.p[i];
            insideTex[insideCount] = inTri.t[i];
            insideCount++;
        }
        else
        {
            outsidePoints[outsideCount] = inTri.p[i];
            outsideTex[outsideCount] = inTri.t[i];
            outsideCount++;
        }
    }

    if (insideCount == 3)
    {
        out1 = inTri;
        return 1;
    }
    else if (insideCount == 1)
    {
        float t0, t1;
        out1.p[0] = insidePoints[0];
        out1.p[1] = Vector_IntersectPlane(planePos, planeNormal, insidePoints[0], outsidePoints[0], t0);
        out1.p[2] = Vector_IntersectPlane(planePos, planeNormal, insidePoints[0], outsidePoints[1], t1);

        out1.color = enableDebug ? olc::BLUE : inTri.color;

        out1.t[0] = insideTex[0];
        out1.t[1].u = insideTex[0].u + t0 * (outsideTex[0].u - insideTex[0].u);
        out1.t[1].v = insideTex[0].v + t0 * (outsideTex[0].v - insideTex[0].v);
        out1.t[1].w = insideTex[0].w + t0 * (outsideTex[0].w - insideTex[0].w);
        out1.t[2].u = insideTex[0].u + t1 * (outsideTex[1].u - insideTex[0].u);
        out1.t[2].v = insideTex[0].v + t1 * (outsideTex[1].v - insideTex[0].v);
        out1.t[2].w = insideTex[0].w + t1 * (outsideTex[1].w - insideTex[0].w);

        return 1;
    }
    else if (insideCount == 2)
    {
        float t0, t1;
        vec3d newP2 = Vector_IntersectPlane(planePos, planeNormal, insidePoints[0], outsidePoints[0], t0);
        vec3d newP0 = Vector_IntersectPlane(planePos, planeNormal, insidePoints[1], outsidePoints[0], t1);
        vec2d newT2 = {
            insideTex[0].u + t0 * (outsideTex[0].u - insideTex[0].u),
            insideTex[0].v + t0 * (outsideTex[0].v - insideTex[0].v),
            insideTex[0].w + t0 * (outsideTex[0].w - insideTex[0].w),
        };
        vec2d newT0 = {
            insideTex[1].u + t1 * (outsideTex[0].u - insideTex[1].u),
            insideTex[1].v + t1 * (outsideTex[0].v - insideTex[1].v),
            insideTex[1].w + t1 * (outsideTex[0].w - insideTex[1].w),            
        };

        out1.p[0] = insidePoints[0];
        out1.p[1] = insidePoints[1];
        out1.p[2] = newP2;
        out1.t[0] = insideTex[0];
        out1.t[1] = insideTex[1];
        out1.t[2] = newT2;
        out1.color = enableDebug ? olc::GREEN : inTri.color;

        out2.p[0] = insidePoints[1];
        out2.p[1] = newP2;
        out2.p[2] = newP0;
        out2.t[0] = insideTex[1];
        out2.t[1] = newT2;
        out2.t[2] = newT0;
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

    bool LoadFromOBJFile(std::string filename, bool HasTexture = false)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        std::vector<vec3d> vertices;
        std::vector<vec2d> texUVs;

        while (!file.eof())
        {
            char line[256];
            file.getline(line, 256);

            char prefix;
            vec3d v;
            vec2d t;

            std::strstream ss(line, 256);
            ss >> prefix;

            if (prefix == 'v')
            {
                if (ss.peek() == 't')
                {
                    ss >> prefix >> t.u >> t.v;
                    // UV coordinates start at left/bottom corner of texture
                    t.v = 1.0f - t.v;
                    texUVs.push_back(t);
                }
                else
                {
                    ss >> v.x >> v.y >> v.z;
                    vertices.push_back(v);
                }
            }
            else if (prefix == 'f')
            {
                if (!HasTexture)
                {
                    int a, b, c;
                    ss >> a >> b >> c;
                    tris.push_back({ vertices[a - 1], vertices[b - 1], vertices[c - 1] });
                }
                else
                {
                    int va, vb, vc, vd, ta, tb, tc, td;
                    char dummy; // for handling '/'
                    ss >> va >> dummy >> ta;
                    ss >> vb >> dummy >> tb;
                    ss >> vc >> dummy >> tc;
                    triangle triABC = { vertices[va - 1], vertices[vb - 1], vertices[vc - 1], texUVs[ta - 1], texUVs[tb - 1], texUVs[tc - 1] };
                    tris.push_back(triABC);

                    // if there is a fourth vertex, it is a quad, composed of 2 triangles ABC and ACD
                    if (ss.peek())
                    {
                        ss >> vd >> dummy >> td;
                        triangle triACD = { vertices[va - 1], vertices[vc - 1], vertices[vd - 1], texUVs[ta - 1], texUVs[tc - 1], texUVs[td - 1] };
                        tris.push_back(triACD);
                    }
                }
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
    mesh artisan;

    mat4x4 proj;
    vec3d camera;
    vec3d lookDir;
    vec3d light;
    float time;
    float yaw;
    bool enableRotation;
    bool enableDebug;
    bool enableWireframe;
    bool enableBilinearFiltering;
    bool enableShading;

    olc::Sprite *texture;
    olc::Sprite cubeTexture;
    olc::Sprite artisanTexture;

public:
    Engine3D()
    {
        sAppName = "Engine 3D";
    }

    bool OnUserCreate() override
    {
        cube.tris = {
            // SOUTH
            { 0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 1.0f, },
            { 0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, },

            // EAST
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, },

            // NORTH
            { 1.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, },

            // WEST
            { 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 1.0f, },
            { 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, },

            // TOP
            { 0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 1.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, },

            // BOTTOM
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, },

        };

        ship.LoadFromOBJFile("VideoShip.obj");
        teapot.LoadFromOBJFile("teapot.obj");
        axis.LoadFromOBJFile("axis.obj");
        mountains.LoadFromOBJFile("mountains.obj");
        artisan.LoadFromOBJFile("Artisans Hub.obj", true);
        artisanTexture = olc::Sprite();
        artisanTexture.LoadFromFile("High.png");

        cubeTexture = olc::Sprite();
        cubeTexture.LoadFromPGESprFile("toml_modernish.spr");

        obj = &artisan;
        texture = &artisanTexture;

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

        float moveSpeed = 8.0f;
        float turnSpeed = 2.0f;

        if (GetKey(olc::UP).bHeld)
        {
            camera.y += fElapsedTime * moveSpeed;
        }
        if (GetKey(olc::DOWN).bHeld)
        {
            camera.y -= fElapsedTime * moveSpeed;
        }

        if (GetKey(olc::RIGHT).bHeld)
        {
            camera.x += fElapsedTime * moveSpeed;
        }
        if (GetKey(olc::LEFT).bHeld)
        {
            camera.x -= fElapsedTime * moveSpeed;
        }

        if (GetKey(olc::A).bHeld)
        {
            yaw += fElapsedTime * turnSpeed;
        }
        if (GetKey(olc::D).bHeld)
        {
            yaw -= fElapsedTime * turnSpeed;
        }

        vec3d forwardMove = Vector_Mul(lookDir, moveSpeed * fElapsedTime);
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
            texture = NULL;
        }
        if (GetKey(olc::K2).bPressed)
        {
            obj = &teapot;
            texture = NULL;
        }
        if (GetKey(olc::K3).bPressed)
        {
            obj = &ship;
            texture = NULL;
        }
        if (GetKey(olc::K4).bPressed)
        {
            obj = &cube;
            texture = NULL;
        }
        if (GetKey(olc::K5).bPressed)
        {
            obj = &mountains;
            texture = NULL;
        }
        if (GetKey(olc::K6).bPressed)
        {
            obj = &artisan;
            texture = &artisanTexture;
        }
        if (GetKey(olc::K7).bPressed)
        {
            obj = &cube;
            texture = &cubeTexture;
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
        if (GetKey(olc::F).bPressed)
        {
            enableBilinearFiltering = !enableBilinearFiltering;
        }
        if (GetKey(olc::H).bPressed)
        {
            enableShading = !enableShading;
        }

        FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLUE);

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
            triWorld.t[0] = tri.t[0];
            triWorld.t[1] = tri.t[1];
            triWorld.t[2] = tri.t[2];

            vec3d normal = Triangle_Normal(triWorld);
            vec3d visibility = Vector_Sub(triWorld.p[0], camera);
            float culling = Vector_DotProduct(normal, visibility);

            if (culling < 0)
            {
                triangle triView;
                triView.p[0] = Matrix_MultiplyVector(triWorld.p[0], view);
                triView.p[1] = Matrix_MultiplyVector(triWorld.p[1], view);
                triView.p[2] = Matrix_MultiplyVector(triWorld.p[2], view);
                triView.t[0] = triWorld.t[0];
                triView.t[1] = triWorld.t[1];
                triView.t[2] = triWorld.t[2];

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
                    triProj.t[0] = triClipped[i].t[0];
                    triProj.t[1] = triClipped[i].t[1];
                    triProj.t[2] = triClipped[i].t[2];

                    // Normalize texture coordinates by project "w"
                    triProj.t[0].u = triProj.t[0].u / triProj.p[0].w;
                    triProj.t[1].u = triProj.t[1].u / triProj.p[1].w;
                    triProj.t[2].u = triProj.t[2].u / triProj.p[2].w;
                    triProj.t[0].v = triProj.t[0].v / triProj.p[0].w;
                    triProj.t[1].v = triProj.t[1].v / triProj.p[1].w;
                    triProj.t[2].v = triProj.t[2].v / triProj.p[2].w;
                    triProj.t[0].w = 1.0f / triProj.p[0].w;
                    triProj.t[1].w = 1.0f / triProj.p[1].w;
                    triProj.t[2].w = 1.0f / triProj.p[2].w;

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
                if (texture)
                {
                    TexturedTriangle(tri.p[0].x, height - tri.p[0].y, tri.t[0].u, tri.t[0].v, tri.t[0].w,
                                     tri.p[1].x, height - tri.p[1].y, tri.t[1].u, tri.t[1].v, tri.t[1].w,
                                     tri.p[2].x, height - tri.p[2].y, tri.t[2].u, tri.t[2].v, tri.t[2].w,
                                     *texture, tri.color);
                }
                else
                {
                    FillTriangle(tri.p[0].x, height - tri.p[0].y, tri.p[1].x, height - tri.p[1].y, tri.p[2].x, height - tri.p[2].y, tri.color);
                }
                if (enableWireframe)
                {
                    DrawTriangle(tri.p[0].x, height - tri.p[0].y, tri.p[1].x, height - tri.p[1].y, tri.p[2].x, height - tri.p[2].y, olc::YELLOW);
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

    void TexturedTriangle(int x1, int y1, float u1, float v1, float w1, int x2, int y2, float u2, float v2, float w2, int x3, int y3, float u3, float v3, float w3, olc::Sprite &texture, olc::Pixel color)
    {
        // sort vertices vertically
        if (y1 > y2)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
            std::swap(u1, u2);
            std::swap(v1, v2);
            std::swap(w1, w2);
        }
        if (y1 > y3)
        {
            std::swap(x1, x3);
            std::swap(y1, y3);
            std::swap(u1, u3);
            std::swap(v1, v3);
            std::swap(w1, w3);
        }
        if (y2 > y3)
        {
            std::swap(x2, x3);
            std::swap(y2, y3);
            std::swap(u2, u3);
            std::swap(v2, v3);
            std::swap(w2, w3);
        }

        // draw first half (y1 to y2)

        float dx1 = x2 - x1;
        float dy1 = y2 - y1;
        float dx2 = x3 - x1;
        float dy2 = y3 - y1;

        float du1 = u2 - u1;
        float dv1 = v2 - v1;
        float dw1 = w2 - w1;
        float du2 = u3 - u1;
        float dv2 = v3 - v1;
        float dw2 = w3 - w1;

        float dax = dy1 ? (dx1 / fabs(dy1)) : 0;
        float dbx = dy2 ? (dx2 / fabs(dy2)) : 0;

        float dau = dy1 ? (du1 / fabs(dy1)) : 0;
        float dav = dy1 ? (dv1 / fabs(dy1)) : 0;
        float daw = dy1 ? (dw1 / fabs(dy1)) : 0;

        float dbu = dy2 ? (du2 / fabs(dy2)) : 0;
        float dbv = dy2 ? (dv2 / fabs(dy2)) : 0;
        float dbw = dy2 ? (dw2 / fabs(dy2)) : 0;

        if (dy1)
        {
            for (int y = y1; y <= y2; y++)
            {
                int ax = x1 + dax * (y - y1);
                int bx = x1 + dbx * (y - y1);
                float au = u1 + dau * (y - y1);
                float av = v1 + dav * (y - y1);
                float aw = w1 + daw * (y - y1);
                float bu = u1 + dbu * (y - y1);
                float bv = v1 + dbv * (y - y1);
                float bw = w1 + dbw * (y - y1);
                if (ax > bx)
                {
                    std::swap(ax, bx);
                    std::swap(au, bu);
                    std::swap(av, bv);
                    std::swap(aw, bw);
                }
                float t = 0;
                float tStep = 1.0f / (bx - ax);
                for (int x = ax; x <= bx; x++)
                {
                    float u = au + t * (bu - au);
                    float v = av + t * (bv - av);
                    float w = aw + t * (bw - aw);
                    u /= w;
                    v /= w;
                    olc::Pixel sample = enableBilinearFiltering ? texture.SampleBL(u, v) : texture.Sample(u, v);
                    if (enableShading)
                    {
                        sample.r = (sample.r * color.r) / 255;
                        sample.g = (sample.g * color.g) / 255;
                        sample.b = (sample.b * color.b) / 255;
                    }
                    Draw(x, y, sample);
                    t += tStep;
                }
            }
        }

        // draw second half (y2 to y3)

        dx1 = x3 - x2;
        dy1 = y3 - y2;
        du1 = u3 - u2;
        dv1 = v3 - v2;
        dw1 = w3 - w2;
        dax = (dy1 != 0) ? (dx1 / fabs(dy1)) : 0;
        dau = (dy1 != 0) ? (du1 / fabs(dy1)) : 0;
        dav = (dy1 != 0) ? (dv1 / fabs(dy1)) : 0;
        daw = (dy1 != 0) ? (dw1 / fabs(dy1)) : 0;

        if (dy1)
        {
            for (int y = y2; y <= y3; y++)
            {
                int ax = x2 + dax * (y - y2);
                float au = u2 + dau * (y - y2);
                float av = v2 + dav * (y - y2);
                float aw = w2 + daw * (y - y2);
                int bx = x1 + dbx * (y - y1);
                float bu = u1 + dbu * (y - y1);
                float bv = v1 + dbv * (y - y1);
                float bw = w1 + dbw * (y - y1);
                if (ax > bx)
                {
                    std::swap(ax, bx);
                    std::swap(au, bu);
                    std::swap(av, bv);
                    std::swap(aw, bw);
                }
                float t = 0;
                float tStep = 1.0f / (bx - ax);
                for (int x = ax; x <= bx; x++)
                {
                    float u = au + t * (bu - au);
                    float v = av + t * (bv - av);
                    float w = aw + t * (bw - aw);
                    u /= w;
                    v /= w;
                    olc::Pixel sample = enableBilinearFiltering ? texture.SampleBL(u, v) : texture.Sample(u, v);
                    if (enableShading)
                    {
                        sample.r = (sample.r * color.r) / 255;
                        sample.g = (sample.g * color.g) / 255;
                        sample.b = (sample.b * color.b) / 255;
                    }
                    Draw(x, y, sample);
                    t += tStep;
                }
            }
        }

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
