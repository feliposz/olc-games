#include <vector>
#include "olcConsoleGameEngine.h"
using namespace std;

const int GridSize = 16;
const int CellSize = 5;
const int CellSpace = 4;
const int MarginTop = 10;
const int MarginLeft = 10;

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
    Node *StartNode;
    Node *EndNode;

    virtual bool OnUserCreate() override
    {
        InitNodes();
        UpdateNeighbors();
        StartNode = &Nodes[0];
        EndNode = &Nodes[GridSize * GridSize - 1];

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
        // Draw link to neighbors
        for (int i = 0; i < GridSize * GridSize; i++) {
            for (auto n : Nodes[i].neighbors) {
                int x1 = MarginLeft + (int)((Nodes[i].x) * (CellSize + CellSpace) + CellSize/2);
                int y1 = MarginTop + (int)((Nodes[i].y) * (CellSize + CellSpace) + CellSize / 2);
                int x2 = MarginLeft + (int)((n->x) * (CellSize + CellSpace) + CellSize / 2);
                int y2 = MarginTop + (int)((n->y) * (CellSize + CellSpace) + CellSize / 2);
                DrawLine(x1, y1, x2, y2, PIXEL_SOLID, FG_DARK_BLUE);
            }
        }

        // Draw nodes
        for (int i = 0; i < GridSize * GridSize; i++) {
            short color = FG_DARK_BLUE;
            if (Nodes[i].obstacle) {
                color = FG_DARK_GREY;
            }
            if (Nodes[i].visited) {
                color = FG_BLUE;
            }
            if (&Nodes[i] == StartNode) {
                color = FG_GREEN;
            }
            if (&Nodes[i] == EndNode) {
                color = FG_RED;
            }
            int x = MarginLeft + (int)(Nodes[i].x * (CellSize + CellSpace));
            int y = MarginTop + (int)(Nodes[i].y * (CellSize + CellSpace));
            Fill(x, y, x + CellSize, y + CellSize, PIXEL_SOLID, color);
        }

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