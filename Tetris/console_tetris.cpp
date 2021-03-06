#include "stdafx.h"
#include "windows.h"
#include "conio.h"
#include <thread>
#include <string>
#include <iostream>
#include "console_tetris.h"

using namespace std;

int nScreenWidth = 80;
int nScreenHeight = 30;
int nFieldWidth = 12;
int nFieldHeight = 18;
wstring tetraminos[7][4];

void InitShapes()
{
    // I

    tetraminos[0][0].append(L"..X.");
    tetraminos[0][0].append(L"..X.");
    tetraminos[0][0].append(L"..X.");
    tetraminos[0][0].append(L"..X.");

    tetraminos[0][1].append(L"....");
    tetraminos[0][1].append(L"XXXX");
    tetraminos[0][1].append(L"....");
    tetraminos[0][1].append(L"....");

    tetraminos[0][2].append(L"..X.");
    tetraminos[0][2].append(L"..X.");
    tetraminos[0][2].append(L"..X.");
    tetraminos[0][2].append(L"..X.");

    tetraminos[0][3].append(L"....");
    tetraminos[0][3].append(L"XXXX");
    tetraminos[0][3].append(L"....");
    tetraminos[0][3].append(L"....");

    // Z

    tetraminos[1][0].append(L"..X.");
    tetraminos[1][0].append(L".XX.");
    tetraminos[1][0].append(L".X..");
    tetraminos[1][0].append(L"....");

    tetraminos[1][1].append(L"....");
    tetraminos[1][1].append(L"XX..");
    tetraminos[1][1].append(L".XX.");
    tetraminos[1][1].append(L"....");

    tetraminos[1][2].append(L"..X.");
    tetraminos[1][2].append(L".XX.");
    tetraminos[1][2].append(L".X..");
    tetraminos[1][2].append(L"....");

    tetraminos[1][3].append(L"....");
    tetraminos[1][3].append(L"XX..");
    tetraminos[1][3].append(L".XX.");
    tetraminos[1][3].append(L"....");

    // S

    tetraminos[2][0].append(L".X..");
    tetraminos[2][0].append(L".XX.");
    tetraminos[2][0].append(L"..X.");
    tetraminos[2][0].append(L"....");

    tetraminos[2][1].append(L"....");
    tetraminos[2][1].append(L".XX.");
    tetraminos[2][1].append(L"XX..");
    tetraminos[2][1].append(L"....");

    tetraminos[2][2].append(L".X..");
    tetraminos[2][2].append(L".XX.");
    tetraminos[2][2].append(L"..X.");
    tetraminos[2][2].append(L"....");

    tetraminos[2][3].append(L"....");
    tetraminos[2][3].append(L".XX.");
    tetraminos[2][3].append(L"XX..");
    tetraminos[2][3].append(L"....");

    // T

    tetraminos[3][0].append(L".X..");
    tetraminos[3][0].append(L"XX..");
    tetraminos[3][0].append(L".X..");
    tetraminos[3][0].append(L"....");

    tetraminos[3][1].append(L".X..");
    tetraminos[3][1].append(L"XXX.");
    tetraminos[3][1].append(L"....");
    tetraminos[3][1].append(L"....");

    tetraminos[3][2].append(L".X..");
    tetraminos[3][2].append(L".XX.");
    tetraminos[3][2].append(L".X..");
    tetraminos[3][2].append(L"....");

    tetraminos[3][3].append(L"....");
    tetraminos[3][3].append(L"XXX.");
    tetraminos[3][3].append(L".X..");
    tetraminos[3][3].append(L"....");

    // O

    tetraminos[4][0].append(L"....");
    tetraminos[4][0].append(L".XX.");
    tetraminos[4][0].append(L".XX.");
    tetraminos[4][0].append(L"....");

    tetraminos[4][1].append(L"....");
    tetraminos[4][1].append(L".XX.");
    tetraminos[4][1].append(L".XX.");
    tetraminos[4][1].append(L"....");

    tetraminos[4][2].append(L"....");
    tetraminos[4][2].append(L".XX.");
    tetraminos[4][2].append(L".XX.");
    tetraminos[4][2].append(L"....");

    tetraminos[4][3].append(L"....");
    tetraminos[4][3].append(L".XX.");
    tetraminos[4][3].append(L".XX.");
    tetraminos[4][3].append(L"....");

    // L

    tetraminos[5][0].append(L".X..");
    tetraminos[5][0].append(L".X..");
    tetraminos[5][0].append(L".XX.");
    tetraminos[5][0].append(L"....");

    tetraminos[5][1].append(L"....");
    tetraminos[5][1].append(L"XXX.");
    tetraminos[5][1].append(L"X...");
    tetraminos[5][1].append(L"....");

    tetraminos[5][2].append(L"XX..");
    tetraminos[5][2].append(L".X..");
    tetraminos[5][2].append(L".X..");
    tetraminos[5][2].append(L".....");

    tetraminos[5][3].append(L"..X.");
    tetraminos[5][3].append(L"XXX.");
    tetraminos[5][3].append(L"....");
    tetraminos[5][3].append(L"....");

    // J

    tetraminos[6][0].append(L".X..");
    tetraminos[6][0].append(L".X..");
    tetraminos[6][0].append(L"XX..");
    tetraminos[6][0].append(L"....");

    tetraminos[6][1].append(L"X...");
    tetraminos[6][1].append(L"XXX.");
    tetraminos[6][1].append(L"....");
    tetraminos[6][1].append(L"....");

    tetraminos[6][2].append(L".XX.");
    tetraminos[6][2].append(L".X..");
    tetraminos[6][2].append(L".X..");
    tetraminos[6][2].append(L"....");

    tetraminos[6][3].append(L"....");
    tetraminos[6][3].append(L"XXX.");
    tetraminos[6][3].append(L"..X.");
    tetraminos[6][3].append(L"....");

}

int RotateShape(int shape, int x, int y, int rotation)
{
    int pos = pos = y * 4 + x;
    return tetraminos[shape][rotation][pos];
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

struct SPiece {
    int x, y, shape, rotation;
};

SPiece GetRandomPiece()
{
    SPiece p;
    p.x = nFieldWidth / 2 - 1;
    p.y = 0;
    p.shape = rand() % 7;
    p.rotation = rand() % 4;
    return p;
}

int MainGame()
{
    // Init screen buffer
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    // Hide cursor
    CONSOLE_CURSOR_INFO CursorInfo;
    CursorInfo.bVisible = false;
    CursorInfo.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &CursorInfo);

    wchar_t *screenText = new wchar_t[nScreenWidth * nScreenHeight];
    WORD *screenAttr = new WORD[nScreenWidth * nScreenHeight];
    int *field = new int[nFieldWidth * nFieldHeight];

    // Init game screen buffer
    for (int i = 0; i < nScreenWidth*nScreenHeight; i++)
    {
        screenText[i] = L' ';
        screenAttr[i] = 0x07;
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
    bool bSleep = true;
    const wchar_t *displayText = L" ABCDEFG=#";
    const WORD displayAttr[10] = { 0x0F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x09, 0xF4, 0x8F };

    SPiece pieceCurr, pieceNext;

    pieceCurr = GetRandomPiece();
    pieceNext = GetRandomPiece();

    while (!bGameOver) {

        // Timing
        if (bSleep) {
            this_thread::sleep_for(50ms);
        }
        bSleep = true;
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

        // Handle input/movement

        if (bQuit) {
            bGameOver = true;
        }

        if (bRot) {
            if (!bRotHold) {
                int nNewRot = (pieceCurr.rotation + 1) % 4;
                if (DoesShapeFit(pieceCurr.shape, pieceCurr.x, pieceCurr.y, nNewRot, field)) {
                    pieceCurr.rotation = nNewRot;
                    Beep(330, 100);
                }
                else if (DoesShapeFit(pieceCurr.shape, pieceCurr.x - 1, pieceCurr.y, nNewRot, field)) {
                    pieceCurr.x--;
                    pieceCurr.rotation = nNewRot;
                    Beep(330, 100);
                }
                else if (DoesShapeFit(pieceCurr.shape, pieceCurr.x + 1, pieceCurr.y, nNewRot, field)) {
                    pieceCurr.x++;
                    pieceCurr.rotation = nNewRot;
                    Beep(330, 100);
                }

                bRotHold = true;
            }
        }
        else {
            bRotHold = false;
        }

        if (bLeft && DoesShapeFit(pieceCurr.shape, pieceCurr.x - 1, pieceCurr.y, pieceCurr.rotation, field))
        {
            pieceCurr.x--;
        }

        if (bRight && DoesShapeFit(pieceCurr.shape, pieceCurr.x + 1, pieceCurr.y, pieceCurr.rotation, field))
        {
            pieceCurr.x++;
        }

        if (bDown && DoesShapeFit(pieceCurr.shape, pieceCurr.x, pieceCurr.y + 1, pieceCurr.rotation, field))
        {
            pieceCurr.y++;
            Beep(220, 100);
            bSleep = false;
        }

        // Make pieces fall, handle events

        if (bForceDown)
        {
            if (DoesShapeFit(pieceCurr.shape, pieceCurr.x, pieceCurr.y + 1, pieceCurr.rotation, field))
            {
                // Already moved due to player
                if (!bDown) {
                    pieceCurr.y++;
                    Beep(220, 100);
                    bSleep = false;
                }
            }
            else
            {
                // Place current piece on field
                for (int y = 0; y < 4; y++)
                {
                    for (int x = 0; x < 4; x++)
                    {
                        bool bSolid = RotateShape(pieceCurr.shape, x, y, pieceCurr.rotation) == L'X';
                        if (bSolid) {
                            field[(y + pieceCurr.y)*nFieldWidth + (x + pieceCurr.x)] = pieceCurr.shape + 1;
                        }
                    }
                }

                // Check for lines
                for (int y = pieceCurr.y; y < pieceCurr.y + 4 && y < nFieldHeight - 1; y++) {
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
                if (pieceCurr.y == 0) {
                    bGameOver = true;
                }

                // Select next piece
                pieceCurr = pieceNext;
                pieceNext = GetRandomPiece();
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
                screenText[(2 + y)*nScreenWidth + (2 + x)] = displayText[id];
                screenAttr[(2 + y)*nScreenWidth + (2 + x)] = displayAttr[id];
            }
        }

        if (!bClearLines) {
            // Draw current piece to game buffer
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    bool bSolid = RotateShape(pieceCurr.shape, x, y, pieceCurr.rotation) == L'X';
                    if (bSolid) {
                        int pos = (2 + y + pieceCurr.y)*nScreenWidth + (2 + x + pieceCurr.x);
                        screenText[pos] = displayText[pieceCurr.shape + 1];
                        screenAttr[pos] = displayAttr[pieceCurr.shape + 1];
                    }
                }
            }

            // Draw next piece outside
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    bool bSolid = RotateShape(pieceNext.shape, x, y, pieceNext.rotation) == L'X';
                    int pos = (6 + y + pieceNext.y)*nScreenWidth + (6 + x + nFieldWidth);
                    screenText[pos] = bSolid ? displayText[pieceNext.shape + 1] : L' ';
                    screenAttr[pos] = displayAttr[pieceNext.shape + 1];
                }
            }
        }

        // Draw score
        swprintf_s(&screenText[2 * nScreenWidth + nFieldWidth + 6], 26, L"SCORE: %8d LEVEL: %2d", nScore, nLevel);
        // Remove NULL character from string left by swprintf...
        screenText[2 * nScreenWidth + nFieldWidth + 6 + 25] = L' ';

        swprintf(&screenText[4 * nScreenWidth + nFieldWidth + 6], 6, L"NEXT:");
        screenText[4 * nScreenWidth + nFieldWidth + 6 + 5] = L' '; // TODO: Find a way to remove the need for this!

        // Draw buffer to screen
        DWORD dwWritten;
        WriteConsoleOutputCharacter(hConsole, screenText, nScreenWidth*nScreenHeight, { 0, 0 }, &dwWritten);
        WriteConsoleOutputAttribute(hConsole, (const WORD*)screenAttr, nScreenWidth*nScreenHeight, { 0, 0 }, &dwWritten);

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

            int scores[5] = { 0, 100, 250, 400, 600 };

            // Pause a little after line clearing
            for (int i = 0; i < nLinesCleared; i++) {
                Beep(scores[i+1]*2+440, 400 / nLinesCleared);
            }

            // Compute score
            nScore += scores[nLinesCleared];

            // Adjust speed
            nLevel = nScore / 1000 + 1;
        }
    }

    delete screenText;
    delete screenAttr;
    delete field;

    CloseHandle(hConsole);

    return nScore;
}

int main()
{
    
    srand(time(NULL));

    InitShapes();

    // Set console window size
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT windowSize = { 0, 0, nScreenWidth - 1, nScreenHeight - 1 };
    SetConsoleWindowInfo(hStdOut, TRUE, &windowSize);

    bool bContinue = true;
    int nScore = -1;
    while (bContinue) {
        int option;
        system("cls");
        if (nScore >= 0) {
            std::cout << "Game over! Score: " << nScore << endl << endl;
        }
        cout << "Tetris console" << endl << endl << "1) New Game" << endl << "2) Quit" << endl << endl << "Choice: ";
        cin >> option;
        switch (option) {
        case 1:
            nScore = MainGame();
            break;
        default:
            bContinue = false;
            break;
        }
    }

    SetConsoleActiveScreenBuffer(hStdOut);

    // Display score    
    std::system("pause");

    return 0;
}

