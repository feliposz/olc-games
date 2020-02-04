#include <vector>
#include <algorithm>
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
    float local;
    float global;
    vector<Node *> neighbors;
    Node *parent;
};

class AstarGame : public olcConsoleGameEngine
{
    Node *Nodes = nullptr;
    Node *StartNode = nullptr;
    Node *EndNode = nullptr;

    virtual bool OnUserCreate() override
    {
        InitNodes();
        
        //srand(time(0));

        // Place random obstacles
        for (int i = 0; i < GridSize * GridSize; i++) {
            Nodes[i].obstacle = rand() % 10 < 3;
        }

        // Select random start/end
        StartNode = &Nodes[rand() % (GridSize*GridSize)];
        EndNode = &Nodes[rand() % (GridSize*GridSize)];

        StartNode->obstacle = false;
        EndNode->obstacle = false;

        UpdateNeighbors();
        AstarPath();

        return true;
    }

    void InitNodes()
    {
        Nodes = new Node[GridSize * GridSize];

        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                Node *n = &Nodes[y * GridSize + x];
                n->x = x;
                n->y = y;
                n->obstacle = false;
            }
        }
    }


    void ResetNodes()
    {
        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                Node *n = &Nodes[y * GridSize + x];
                n->global = INT32_MAX;
                n->local = INT32_MAX;
                n->visited = false;
                n->parent = nullptr;
            }
        }
    }

    void UpdateNeighbors()
    {
        for (int x = 0; x < GridSize; x++) {
            for (int y = 0; y < GridSize; y++) {
                Node *current = &Nodes[y * GridSize + x];
                current->neighbors.clear();
                if (current->obstacle) {
                    continue;
                }
                Node *left = &Nodes[y * GridSize + x - 1];
                Node *right = &Nodes[y * GridSize + x + 1];
                Node *up = &Nodes[(y - 1) * GridSize + x];
                Node *down = &Nodes[(y + 1) * GridSize + x];                
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

    float Distance(Node *a, Node *b)
    {
        return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
    }

    void AstarPath()
    {
        ResetNodes();

        vector<Node *> toVisit;

        StartNode->global = Distance(StartNode, EndNode);
        StartNode->local = 0;

        toVisit.push_back(StartNode);

        while (!toVisit.empty()) {
            Node *current = toVisit.back();
            toVisit.pop_back();
            current->visited = true;
            for (auto n : current->neighbors) {
                float testLocal = current->local + Distance(current, n);
                if (testLocal < n->local) {
                    n->parent = current;
                    n->local = testLocal;
                    n->global = testLocal + Distance(n, EndNode);
                }
                if (!n->visited) {
                    if (find(toVisit.begin(), toVisit.end(), n) == toVisit.end()) {
                        toVisit.push_back(n);
                    }
                }
            }
            
            sort(toVisit.begin(), toVisit.end(), [&](Node *a, Node *b) { return a->global > b->global; });
        }
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

        if (m_mouse[0].bReleased) {
            int nodeX = (m_mousePosX - MarginTop) / (CellSize + CellSpace);
            int nodeY = (m_mousePosY - MarginTop) / (CellSize + CellSpace);
            if (nodeX >= 0 && nodeX < GridSize && nodeY >= 0 && nodeY <= GridSize) {
                Node *n = &Nodes[nodeY * GridSize + nodeX];
                if (m_keys[VK_SHIFT].bHeld && !n->obstacle) {
                    StartNode = n;
                    AstarPath();
                }
                else if (m_keys[VK_CONTROL].bHeld && !n->obstacle) {
                    EndNode = n;
                    AstarPath();
                }
                else {
                    n->obstacle = !n->obstacle;
                    UpdateNeighbors();
                    AstarPath();
                }
            }
        }

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
            Fill(x, y, x + CellSize, y + CellSize, Nodes[i].visited ? PIXEL_SOLID : PIXEL_HALF, color);
        }
        
        Node *Path = EndNode;
        while (Path->parent) {

            if (Path->parent) {
                int x1 = MarginLeft + (int)((Path->x) * (CellSize + CellSpace) + CellSize / 2);
                int y1 = MarginTop + (int)((Path->y) * (CellSize + CellSpace) + CellSize / 2);
                int x2 = MarginLeft + (int)((Path->parent->x) * (CellSize + CellSpace) + CellSize / 2);
                int y2 = MarginTop + (int)((Path->parent->y) * (CellSize + CellSpace) + CellSize / 2);
                DrawLine(x1, y1, x2, y2, PIXEL_SOLID, FG_YELLOW);
            }

            Path = Path->parent;
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