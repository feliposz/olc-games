#include "olcConsoleGameEngine.h"

const int MAX_SOLDIERS = 20;

enum {
    SS_IDLE = 0,
    SS_SELECTED = 1,
    SS_MOVING = 2
};

struct Soldier
{
    float m_x, m_y;
    float m_destX, m_destY;
    float m_deltaX, m_deltaY;
    int m_state;
};

class Game : public olcConsoleGameEngine
{
private:
    float m_fPlayerX, m_fPlayerY, m_fSpeed;
    int m_startX, m_startY;
    Soldier m_soldiers[MAX_SOLDIERS];

    float Distance(float x1, float y1, float x2, float y2)
    {
        return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    }

public:
    Game()
    {

    }

    // Inherited via olcConsoleGameEngine
    virtual bool OnUserCreate() override
    {
        m_fSpeed = 20.0f;

        for (auto &s : m_soldiers) {
            s.m_x = rand() % m_nScreenWidth;
            s.m_y = rand() % m_nScreenHeight;
            s.m_destX = s.m_x;
            s.m_destY = s.m_y;
            s.m_deltaX = 0;
            s.m_deltaY = 0;
            s.m_state = SS_IDLE;
        }        

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {

        Fill(0, 0, m_nScreenWidth, m_nScreenHeight, L' ', 0);

        for (auto &s : m_soldiers) {
            int color = FG_GREEN;

            // Move units
            if (s.m_state == SS_MOVING) {
                s.m_x += s.m_deltaX * m_fSpeed * fElapsedTime;
                s.m_y += s.m_deltaY * m_fSpeed * fElapsedTime;
                // Check if destination reached
                if (Distance(s.m_destX, s.m_destY, s.m_x, s.m_y) < 1) {
                    s.m_state = SS_IDLE;
                    s.m_deltaX = 0;
                    s.m_deltaY = 0;
                    s.m_destX = s.m_x;
                    s.m_destY = s.m_y;
                }
            }

            switch (s.m_state) {
                case SS_IDLE: color = FG_GREEN; break;
                case SS_SELECTED: color = FG_YELLOW; break;
                case SS_MOVING: color = FG_RED; break;
            }
           
            Draw(s.m_x, s.m_y-1, 'o', color);
            Draw(s.m_x-1, s.m_y, '-', color);
            Draw(s.m_x, s.m_y, '+', color);
            Draw(s.m_x+1, s.m_y, '-', color);
            Draw(s.m_x-1, s.m_y+1, '/', color);
            Draw(s.m_x+1, s.m_y+1, '\\', color);
        }
        
        // Started dragging mouse
        if (m_mouse[0].bPressed) {
            m_startX = m_mousePosX;
            m_startY = m_mousePosY;
        }

        // Proper corners
        int left = m_startX < m_mousePosX ? m_startX : m_mousePosX;
        int right = m_startX > m_mousePosX ? m_startX : m_mousePosX;
        int top = m_startY < m_mousePosY ? m_startY : m_mousePosY;
        int bottom = m_startY > m_mousePosY ? m_startY : m_mousePosY;

        if (m_mouse[0].bReleased) {

            // Check for click first!
            if (m_startX == m_mousePosX && m_startY == m_mousePosY) {
                // Get the middle (average) point of selected units
                float avgX = 0, avgY = 0;
                int count = 0;
                for (auto &s : m_soldiers) {
                    if (s.m_state == SS_SELECTED) {
                        count++;
                        avgX += s.m_x;
                        avgY += s.m_y;
                    }
                }
                if (count > 0) {
                    avgX /= count;
                    avgY /= count;
                    // Set destination point relative to clicked destination and mid-point
                    for (auto &s : m_soldiers) {
                        if (s.m_state == SS_SELECTED) {
                            s.m_destX = s.m_x + m_mousePosX - avgX;
                            s.m_destY = s.m_y + m_mousePosY - avgY;
                            // Normalize movement vector
                            float distance = Distance(s.m_x, s.m_y, s.m_destX, s.m_destY);
                            s.m_deltaX = (s.m_destX - s.m_x) / distance;
                            s.m_deltaY = (s.m_destY - s.m_y) / distance;
                            s.m_state = SS_MOVING;
                        }
                    }
                }
            }

            for (auto &s : m_soldiers) {
                if (s.m_x >= left && s.m_x <= right && s.m_y >= top && s.m_y <= bottom) {
                    s.m_state = SS_SELECTED;
                }
                else if (s.m_state == SS_SELECTED) {
                    s.m_state = SS_IDLE;
                }
            }
        }

        // Selection rectangle
        if (m_mouse[0].bHeld) {
            Fill(left, top, right + 1, top + 1, PIXEL_SOLID, FG_WHITE);
            Fill(left, bottom, right + 1, bottom + 1, PIXEL_SOLID, FG_WHITE);
            Fill(left, top, left + 1, bottom + 1, PIXEL_SOLID, FG_WHITE);
            Fill(right, top, right + 1, bottom + 1, PIXEL_SOLID, FG_WHITE);
        }

        // Cursor
        Draw(m_mousePosX - 1, m_mousePosY, PIXEL_SOLID, FG_YELLOW);
        Draw(m_mousePosX + 1, m_mousePosY, PIXEL_SOLID, FG_YELLOW);
        Draw(m_mousePosX, m_mousePosY - 1, PIXEL_SOLID, FG_YELLOW);
        Draw(m_mousePosX, m_mousePosY + 1, PIXEL_SOLID, FG_YELLOW);
        Draw(m_mousePosX, m_mousePosY, PIXEL_SOLID, FG_YELLOW);

        return true;
    }

};

int main()
{
    srand(time(0));
    Game game;
    game.ConstructConsole(160, 120, 8, 8);

    game.Start();

    return 0;
}