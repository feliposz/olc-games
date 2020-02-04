#include <vector>
#include "olcConsoleGameEngine.h"
using namespace std;

const int GridSize = 16;

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
    Node *Nodes;

    virtual bool OnUserCreate() override
    {
        InitNodes();
        UpdateNeighbors();

        return true;
    }

    void InitNodes()
    {
        Nodes = new Node[GridSize * GridSize];

        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                Node *n = &Nodes[y * GridSize + x];
                n->global = INT32_MAX;
                n->local = INT32_MAX;
                n->x = x;
                n->y = y;
                n->visited = false;
                n->obstacle = false;
                n->parent = NULL;
            }
        }
    }

    void UpdateNeighbors()
    {
        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                Node *current = &Nodes[y * GridSize + x];
                Node *left = &Nodes[y * GridSize + x - 1];
                Node *right = &Nodes[y * GridSize + x + 1];
                Node *up = &Nodes[(y - 1) * GridSize + x];
                Node *down = &Nodes[(y + 1) * GridSize + x];
                current->neighbors.clear();
                if (x > 0 && !left->obstacle) {
                    current->neighbors.push_back(left);
                }
                if (x < GridSize - 1 && !right->obstacle) {
                    current->neighbors.push_back(right);
                }
                if (y > 0 && !up->obstacle) {
                    current->neighbors.push_back(up);
                }
                if (y < GridSize - 1 && !down->obstacle) {
                    current->neighbors.push_back(down);
                }
            }
        }
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