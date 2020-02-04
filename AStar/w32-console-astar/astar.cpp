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

int main()
{
    return 0;
}