#define OLC_PGE_APPLICATION
#include "../olcPixelGameEngine.h"

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

class Game : public olc::PixelGameEngine
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
            s.m_x = rand() % ScreenWidth();
            s.m_y = rand() % ScreenHeight();
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

        Clear(olc::BLACK);

        for (auto &s : m_soldiers) {
            olc::Pixel color = olc::GREEN;

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
                case SS_IDLE: color = olc::GREEN; break;
                case SS_SELECTED: color = olc::YELLOW; break;
                case SS_MOVING: color = olc::RED; break;
            }
           
            DrawCircle(s.m_x, s.m_y - 4, 2, color); // head
            DrawLine(s.m_x, s.m_y - 2, s.m_x, s.m_y + 2, color); // body
            DrawLine(s.m_x - 3, s.m_y, s.m_x + 3, s.m_y, color); // armgs
            DrawLine(s.m_x, s.m_y + 2, s.m_x - 3, s.m_y + 5, color); // left leg
            DrawLine(s.m_x, s.m_y + 2, s.m_x + 3, s.m_y + 5, color); // right leg

        }
        
        // Started dragging mouse
        if (GetMouse(0).bPressed) {
            m_startX = GetMouseX();
            m_startY = GetMouseY();
        }

        // Proper corners
        int left = m_startX < GetMouseX() ? m_startX : GetMouseX();
        int right = m_startX > GetMouseX() ? m_startX : GetMouseX();
        int top = m_startY < GetMouseY() ? m_startY : GetMouseY();
        int bottom = m_startY > GetMouseY() ? m_startY : GetMouseY();

        if (GetMouse(0).bReleased) {

            // Check for click first!
            if (m_startX == GetMouseX() && m_startY == GetMouseY()) {
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
                            s.m_destX = s.m_x + GetMouseX() - avgX;
                            s.m_destY = s.m_y + GetMouseY() - avgY;
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
        if (GetMouse(0).bHeld) {
            DrawRect(left, top, right - left, bottom - top, olc::WHITE);
        }

        // Cursor
        DrawLine(GetMouseX() - 2, GetMouseY(), GetMouseX() + 2, GetMouseY(), olc::YELLOW);
        DrawLine(GetMouseX(), GetMouseY() - 2, GetMouseX(), GetMouseY() + 2, olc::YELLOW);

        return true;
    }

};

int main()
{
    srand(time(0));
    Game game;
    if (game.Construct(160, 120, 8, 8))
    {
        game.Start();
    }

    return 0;
}