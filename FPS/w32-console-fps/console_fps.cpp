#include "windows.h"

using namespace std;

const int nScreenWidth = 120;
const int nScreenHeight = 40;

int main()
{
    wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];

    for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
        screen[i] = L'.';
    }

    // Set console window size
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT windowSize = { 0, 0, nScreenWidth - 1, nScreenHeight - 1 };
    SetConsoleWindowInfo(hStdOut, TRUE, &windowSize);

    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    DWORD nWritten = 0;
    WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth*nScreenHeight, { 0, 0 }, &nWritten);

    while (1) {

    }

    return 0;
}