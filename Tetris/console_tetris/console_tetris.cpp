#include "stdafx.h"
#include "windows.h"
#include "conio.h"
#include <thread>
#include <string>
#include <iostream>

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
    WORD *attributes = new WORD[nScreenWidth * nScreenHeight];
    int *field = new int[nFieldWidth * nFieldHeight];
    int nCurrentShape = 0;
    int nCurrentX = 0;
    int nCurrentY = 0;
    int nCurrentRot = 0;

    InitShapes();

    // Set console window size
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT windowSize = { 0, 0, nScreenWidth - 1, nScreenHeight - 1 };
    SetConsoleWindowInfo(hStdOut, TRUE, &windowSize);

    // Init screen buffer
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    // Hide cursor
    CONSOLE_CURSOR_INFO CursorInfo;
    CursorInfo.bVisible = false;
    CursorInfo.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &CursorInfo);

    // Init game screen buffer
    for (int i = 0; i < nScreenWidth*nScreenHeight; i++)
    {
        screen[i] = L' ';
        attributes[i] = 0x07;
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
    int nLevel = 0;
    int nScore = 0;
    bool bGameOver = false;
    bool bRotHold = false;
    bool bForceDown = false;
    bool bClearLines = false;
    const wchar_t *display = L" ABCDEFG=#";
    const WORD attribute[10] = { 0x0F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x09, 0xF4, 0x8F };

    nCurrentX = nFieldWidth / 2 - 2;

    while (!bGameOver) {

        // Timing
        this_thread::sleep_for(50ms);
        nCounter++;

        if (nCounter > (20 - nLevel))
        {
            bForceDown = true;
            nCounter = 0;
        }

        // Input        
        const SHORT KeyMask = 0x8000;
        bool bLeft = GetAsyncKeyState(VK_LEFT) & KeyMask;
        bool bRight = GetAsyncKeyState(VK_RIGHT) & KeyMask;
        bool bDown = GetAsyncKeyState(VK_DOWN) & KeyMask;
        bool bQuit = GetAsyncKeyState(VK_ESCAPE) & KeyMask;
        bool bRot = GetAsyncKeyState('Z') & KeyMask;

        // Game Logic

        if (bQuit) {
            bGameOver = true;
        }

        if (bRot) {
            if (!bRotHold) {
                int nNewRot = (nCurrentRot + 1) % 4;
                if (DoesShapeFit(nCurrentShape, nCurrentX, nCurrentY, nNewRot, field)) {
                    nCurrentRot = nNewRot;
                }
                bRotHold = true;
            }
        }
        else {
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

        if (bForceDown)
        {
            if (DoesShapeFit(nCurrentShape, nCurrentX, nCurrentY + 1, nCurrentRot, field))
            {
                nCurrentY++;
            }
            else
            {
                // Place current piece on field
                for (int y = 0; y < 4; y++)
                {
                    for (int x = 0; x < 4; x++)
                    {
                        bool bSolid = RotateShape(nCurrentShape, x, y, nCurrentRot) == L'X';
                        if (bSolid) {
                            field[(y + nCurrentY)*nFieldWidth + (x + nCurrentX)] = nCurrentShape + 1;
                        }
                    }
                }

                // Check for lines
                for (int y = nCurrentY; y < nCurrentY + 4 && y < nFieldHeight - 1; y++) {
                    bool bFullLine = true;
                    for (int x = 1; x < nFieldWidth - 1; x++) {
                        if (field[y*nFieldWidth + x] == 0) {
                            bFullLine = false;
                            break;
                        }
                    }
                    if (bFullLine) {
                        bClearLines = true;
                        for (int x = 1; x < nFieldWidth - 1; x++) {
                            field[y*nFieldWidth + x] = 8;
                        }
                    }
                }

                // Check for game over
                if (nCurrentY == 0) {
                    bGameOver = true;
                }

                // Select next piece
                nCurrentShape = rand() % 7;
                nCurrentX = nFieldWidth / 2 - 2;
                nCurrentY = 0;
                nCurrentRot = 0;
            }
            bForceDown = false;
        }

        // Rendering

        // Draw field to game buffer        
        for (int y = 0; y < nFieldHeight; y++)
        {
            for (int x = 0; x < nFieldWidth; x++)
            {
                int id = field[y*nFieldWidth + x];
                screen[(2 + y)*nScreenWidth + (2 + x)] = display[id];
                attributes[(2 + y)*nScreenWidth + (2 + x)] = attribute[id];
            }
        }

        if (!bClearLines) {
            // Draw current piece to game buffer
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    bool bSolid = RotateShape(nCurrentShape, x, y, nCurrentRot) == L'X';
                    if (bSolid) {
                        screen[(2 + y + nCurrentY)*nScreenWidth + (2 + x + nCurrentX)] = display[nCurrentShape + 1];
                        attributes[(2 + y + nCurrentY)*nScreenWidth + (2 + x + nCurrentX)] = attribute[nCurrentShape + 1];
                    }
                }
            }
        }

        // Draw score
        swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 26, L"SCORE: %8d LEVEL: %2d", nScore, nLevel);
        // Remove NULL character from string left by swprintf...
        screen[2 * nScreenWidth + nFieldWidth + 6 + 25] = L' ';

        // Draw buffer to screen
        DWORD dwWritten;
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth*nScreenHeight, { 0, 0 }, &dwWritten);
        WriteConsoleOutputAttribute(hConsole, (const WORD*)attributes, nScreenWidth*nScreenHeight, { 0, 0 }, &dwWritten);

        int nLinesCleared = 0;
        if (bClearLines) {
            // scan lines from bottom to top
            for (int scanY = nFieldHeight - 1; scanY > 0; scanY--) {
                // check for cleared line
                if (field[scanY * nFieldWidth + 1] == 8) {
                    for (int y = scanY; y > 0; y--) {
                        for (int x = 1; x < nFieldWidth - 1; x++) {
                            // move pieces down
                            field[y * nFieldWidth + x] = field[(y - 1) * nFieldWidth + x];
                        }
                    }
                    scanY++; // line cleared, retry
                    nLinesCleared++;
                }
            }
            bClearLines = false;

            // Pause a little after line clearing
            this_thread::sleep_for(400ms);

            // Compute score
            int scores[5] = { 0, 100, 250, 400, 600 };
            nScore += scores[nLinesCleared];

            // Adjust speed
            nLevel = nScore / 1000 + 1;
        }
    }

    SetConsoleActiveScreenBuffer(hStdOut);
    CloseHandle(hConsole);

    // Display score
    std::cout << "Game over! Score: " << nScore << endl;
    std::system("pause");

    return 0;
}

