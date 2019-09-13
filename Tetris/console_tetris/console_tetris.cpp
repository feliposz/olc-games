// console_tetris.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include <thread>
#include <string>

using namespace std;

int nScreenWidth = 80;
int nScreenHeight = 30;
int nFieldWidth = 12;
int nFieldHeight = 18;
wstring tetraminos[7];

void InitShapes()
{
    tetraminos[0].append(L"..X.");
    tetraminos[0].append(L"..X.");
    tetraminos[0].append(L"..X.");
    tetraminos[0].append(L"..X.");

    tetraminos[1].append(L"..X.");
    tetraminos[1].append(L".XX.");
    tetraminos[1].append(L".X..");
    tetraminos[1].append(L"....");

    tetraminos[2].append(L".X..");
    tetraminos[2].append(L".XX.");
    tetraminos[2].append(L"..X.");
    tetraminos[2].append(L"....");

    tetraminos[3].append(L"..X.");
    tetraminos[3].append(L".XX.");
    tetraminos[3].append(L"..X.");
    tetraminos[3].append(L"....");

    tetraminos[4].append(L"....");
    tetraminos[4].append(L".XX.");
    tetraminos[4].append(L".XX.");
    tetraminos[4].append(L"....");

    tetraminos[5].append(L".X..");
    tetraminos[5].append(L".X..");
    tetraminos[5].append(L".XX.");
    tetraminos[5].append(L"....");

    tetraminos[6].append(L"..X.");
    tetraminos[6].append(L"..X.");
    tetraminos[6].append(L".XX.");
    tetraminos[6].append(L"....");
}

int RotateShape(int shape, int x, int y, int rotation)
{
    int pos = 0;
    switch (rotation)
    {
    case 0:
        pos = y * 4 + x;
        break;
    case 1:
        pos = 12 + y - 4 * x;
        break;
    case 2:
        pos = 15 - y * 4 - x;
        break;
    case 3:
        pos = 3 - y + 4 * x;
        break;
    }
    return tetraminos[shape][pos];
}

bool DoesShapeFit(int shape, int px, int py, int rotation, int *field)
{
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            bool bSolid = L'X' == RotateShape(shape, x, y, rotation);
            int pos = (y + py) * nFieldWidth + (x + px);
            if (bSolid && field[pos] != 0)
            {
                return false;
            }
        }
    }
    return true;
}

int main()
{
    wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
    int *field = new int[nFieldWidth * nFieldHeight];
    int nCurrentShape = 2;
    int nCurrentX = 0;
    int nCurrentY = 0;
    int nCurrentRot = 3;

    InitShapes();

    // Set console window size
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT windowSize = { 0, 0, nScreenWidth - 1, nScreenHeight - 1 };
    SetConsoleWindowInfo(hStdOut, TRUE, &windowSize);

    // Init screen buffer
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    // Init game screen buffer
    for (int i = 0; i < nScreenWidth*nScreenHeight; i++)
    {
        screen[i] = L' ';
    }

    // Init game field
    for (int y = 0; y < nFieldHeight; y++)
    {
        for (int x = 0; x < nFieldWidth; x++)
        {
            int id = 0;
            if (y == nFieldHeight - 1 || x == 0 || x == nFieldWidth - 1)
            {
                id = 9;
            }
            field[y*nFieldWidth + x] = id;
        }
    }

    // Game loop

    int nCounter = 0;
    bool bGameOver = false;
    bool bRotHold = false;

    nCurrentX = nFieldWidth / 2 - 2;

    while (!bGameOver) {

        // Timing
        this_thread::sleep_for(50ms);
        nCounter++;

        // Input        
        const SHORT KeyMask = 0x8000;
        bool bLeft = GetAsyncKeyState(VK_LEFT) & KeyMask;
        bool bRight = GetAsyncKeyState(VK_RIGHT) & KeyMask;
        bool bDown = GetAsyncKeyState(VK_DOWN) & KeyMask;
        bool bQuit = GetAsyncKeyState(VK_ESCAPE) & KeyMask;
        bool bRot = GetAsyncKeyState('Z') & KeyMask;

        // Game Logic

        if (bQuit)
        {
            bGameOver = true;
        }

        if (bRot)
        {
            if (!bRotHold && DoesShapeFit(nCurrentShape, nCurrentX, nCurrentY, (nCurrentRot + 1) % 4, field))
            {
                nCurrentRot = (nCurrentRot + 1) % 4;
                bRotHold = true;
            }
        }
        else
        {
            bRotHold = false;
        }

        if (bLeft && DoesShapeFit(nCurrentShape, nCurrentX - 1, nCurrentY, nCurrentRot, field))
        {
            nCurrentX--;
        }

        if (bRight && DoesShapeFit(nCurrentShape, nCurrentX + 1, nCurrentY, nCurrentRot, field))
        {
            nCurrentX++;
        }

        if (bDown && DoesShapeFit(nCurrentShape, nCurrentX, nCurrentY + 1, nCurrentRot, field))
        {
            nCurrentY++;
        }

        // Rendering

        // Draw field to game buffer
        const wchar_t *display = L" ABCDEFG=#";
        for (int y = 0; y < nFieldHeight; y++)
        {
            for (int x = 0; x < nFieldWidth; x++)
            {
                int id = field[y*nFieldWidth + x];
                screen[(2 + y)*nScreenWidth + (2 + x)] = display[id];
            }
        }

        // Draw current piece to game buffer
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                bool bSolid = RotateShape(nCurrentShape, x, y, nCurrentRot) == L'X';
                if (bSolid) {
                    screen[(2 + y + nCurrentY)*nScreenWidth + (2 + x + nCurrentX)] = display[nCurrentShape];
                }
            }
        }

        // Draw buffer to screen
        COORD cPos;
        cPos.X = 0;
        cPos.Y = 0;
        DWORD nWritten;
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth*nScreenHeight, cPos, &nWritten);

    }

    SetConsoleActiveScreenBuffer(hStdOut);
    CloseHandle(hConsole);

    return 0;
}

