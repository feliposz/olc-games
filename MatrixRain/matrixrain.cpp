#include "olcConsoleGameEngineCustom.h"
#include <string>
#include <stdlib.h>

class MatrixRain : public olcConsoleGameEngine
{
private:
    struct Streamer
    {
        int column;
        float position;
        float speed;
        std::wstring text;
    };

    const int maxStreamers = 75;
    std::list<Streamer> listStreamers;

    void InitializeStreamer(Streamer &s)
    {
        s.column = rand() % ScreenWidth();
        s.position = 0;
        s.speed = (float)(rand() % 35 + 5);
        int size = rand() % 30 + 10;
        for (int i = 0; i < size; i++)
        {
            s.text.append(1, RandomChar());
        }
    }

    wchar_t RandomChar()
    {
        wchar_t min = 0;
        wchar_t max = 0;
        switch (rand() % 5)
        {
            case 0:
                min = L'A';
                max = L'Z';
                break;
            case 1:
                min = L'0';
                max = L'9';
                break;
            default:
                // half-width katakana
                min = 0xFF67;
                max = 0xFF9D;
                break;
        }
        wchar_t range = max - min;
        return rand() % range + min;
    }

public:
    virtual bool OnUserCreate() override
    {
        for (int i = 0; i < maxStreamers; i++)
        {
            Streamer s;
            InitializeStreamer(s);
            listStreamers.push_back(s);
        }
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ', FG_BLACK);

        for (auto &s : listStreamers)
        {
            for (int i = 0; i < s.text.size(); i++)
            {
                int row = (int)s.position - i;
                if (row >= 0 && row < ScreenHeight())
                {
                    int j = row % s.text.size();
                    short color = (s.speed > 25.0f) ? FG_GREEN : FG_DARK_GREEN;
                    if (i == 0)
                    {
                        color = FG_WHITE;
                    }
                    else if (i < 4)
                    {
                        color = FG_GREY;
                    }
                    DrawString(s.column, row, s.text.substr(j, 1), color);
                }
            }
            s.position += s.speed * fElapsedTime;
            if (s.position - s.text.size() >= ScreenHeight())
            {
                InitializeStreamer(s);
            }
        }
        return true;
    }
};

int main()
{
    MatrixRain game;
    game.ConstructConsole(120, 50, 10, 15, L"MS Gothic");
    game.Start();
    return 0;
}
