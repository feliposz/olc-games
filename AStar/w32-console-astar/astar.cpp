#include <vector>
#include "olcConsoleGameEngine.h"
using namespace std;

struct Node
{
    bool visited;
    bool obstacle;
    float x, y;
    int local;
    int global;
    vector<Node *> neighbors;
    Node *parent;
};

class AstarGame : public olcConsoleGameEngine
{
    // Inherited via olcConsoleGameEngine
    virtual bool OnUserCreate() override
    {
        return true;
    }
    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        return true;
    }
};

int main()
{
    AstarGame game;
    game.ConstructConsole(160, 160, 6, 6);
    game.Start();

    return 0;
}