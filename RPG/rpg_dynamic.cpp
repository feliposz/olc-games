#include "rpg_dynamic.h"
#include "rpg_assets.h"
#include "game_util.h"

namespace Rpg {

    Dynamic::Dynamic(std::string name)
    {
        px = 0;
        py = 0;
        vx = 0;
        vy = 0;
        SolidMap = true;
        SolidDynamic = true;
        Friendly = true;
        Name = name;
    }

    Dynamic::~Dynamic()
    {
    }

    Dynamic_Creature::Dynamic_Creature(std::string name, olc::Sprite *sprite) : Dynamic(name)
    {
        Health = 10;
        MaxHealth = 10;
        m_sprite = sprite;
        m_frame = 0;
        m_timer = 0;
    }

    void Dynamic_Creature::Update(float elapsed, Dynamic *player)
    {
        m_timer += elapsed;
        if (m_timer > 0.2f) {
            m_timer -= 0.2f;
            m_frame = (m_frame + 1) % 2;
        }

        if (vx < -0.1f) {
            m_direction = WEST;
        }
        if (vx > 0.1f) {
            m_direction = EAST;
        }
        if (vy < -0.1f) {
            m_direction = NORTH;
        }
        if (vy > 0.1f) {
            m_direction = SOUTH;
        }

        if (Health <= 0) {
            Health = 0;
            m_state = DEAD;
        }
        else if (fabs(vx) > 0.1f || fabs(vy) > 0.1f) {
            m_state = WALKING;
        }
        else {
            m_state = STANDING;
        }

        Behavior(elapsed, player);
    }

    void Dynamic_Creature::Draw(olc::PixelGameEngine *engine, float ox, float oy)
    {
        int spriteSheetX = 0;
        int spriteSheetY = 0;
        switch (m_state) {
        case STANDING:
            spriteSheetX = m_direction * Assets::TileWidth;
            spriteSheetY = 0 * Assets::TileHeight;
            break;
        case WALKING:
            spriteSheetX = m_direction * Assets::TileWidth;
            spriteSheetY = m_frame * Assets::TileHeight;
            break;
        case CELEBRATING:
            spriteSheetX = 4 * Assets::TileWidth;
            spriteSheetY = 0 * Assets::TileHeight;
            break;
        case DEAD:
            spriteSheetX = 4 * Assets::TileWidth;
            spriteSheetY = 1 * Assets::TileHeight;
            break;
        }
        engine->DrawPartialSprite((px - ox) * Assets::TileWidth, (py - oy) * Assets::TileHeight, m_sprite, spriteSheetX, spriteSheetY, Assets::TileWidth, Assets::TileHeight);
    }

    Dynamic_Creature_Skelly::Dynamic_Creature_Skelly() : Dynamic_Creature("skelly", Assets::GetInstance().GetSprite("skelly"))
    {
        Health = 5;
        MaxHealth = 5;
        m_stateTick = 2.0f;
    }

    void Dynamic_Creature_Skelly::Behavior(float elapsed, Dynamic *player)
    {
        m_stateTick -= elapsed;
        if (m_stateTick <= 0.0f) {
            m_stateTick += 1.0f;
            float dist = GameUtil::Distance(player->px, player->py, px, py);

            if (dist < 6.0f) {
                if (player->px > px) {
                    vx = 1.5f;
                }
                else if (player->px < px) {
                    vx = -1.5f;
                }
                if (player->py > py) {
                    vy = 1.5f;
                }
                else if (player->py < py) {
                    vy = -1.5f;
                }
            }
        }
    }

    Dynamic_Teleport::Dynamic_Teleport(float x, float y, std::string map, float tx, float ty) : Dynamic("teleport")
    {
        px = x;
        py = y;
        TargetMap = map;
        TargetX = tx;
        TargetY = ty;
        SolidMap = false;
        SolidDynamic = false;
    }

    void Dynamic_Teleport::Draw(olc::PixelGameEngine * engine, float ox, float oy)
    {
        engine->DrawCircle((px + 0.5f - ox) * Assets::TileWidth, (py + 0.5f - oy) * Assets::TileHeight, Assets::TileWidth * 0.5f);
    }

}