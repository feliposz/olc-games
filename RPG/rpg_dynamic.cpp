#include "rpg_dynamic.h"
#include "rpg_assets.h"
#include "rpg_engine.h"
#include "game_util.h"

namespace Rpg {

    GameEngine *Dynamic::Engine = nullptr;

    Dynamic::Dynamic(std::string name)
    {
        px = 0;
        py = 0;
        vx = 0;
        vy = 0;
        SolidMap = true;
        SolidDynamic = true;
        Friendly = true;
        IsProjectile = false;
        IsAttackable = false;
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
        IsAttackable = true;
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
        Friendly = false;
        EquipedWeapon = (Weapon*)Assets::GetInstance().GetItem("sword");
        m_stateTick = 2.0f;
    }

    void Dynamic_Creature_Skelly::Behavior(float elapsed, Dynamic *player)
    {
        if (m_state == DEAD) {
            vx = 0;
            vy = 0;
            IsAttackable = false;
            SolidDynamic = false;
            return;
        }
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

            if (dist < 1.5f) {
                PerformAttack();
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

    Dynamic_Item::Dynamic_Item(float x, float y, Item * item) : Dynamic(item->Name)
    {
        px = x;
        py = y;
        PItem = item;
        SolidMap = false;
        SolidDynamic = false;
        Collected = false;
    }

    void Dynamic_Item::Draw(olc::PixelGameEngine * engine, float ox, float oy)
    {
        if (Collected)
            return;
        engine->DrawPartialSprite((px - ox) * Assets::TileWidth, (py - oy) * Assets::TileHeight, PItem->Sprite, 0, 0, Assets::TileWidth, Assets::TileHeight);
    }

    void Dynamic_Item::OnInteract(Dynamic * player)
    {
        if (Collected)
            return;
        if (PItem->OnInteract(player)) {
            Engine->GiveItem(PItem);
        }
        Collected = true;
    }

    Dynamic_Projectile::Dynamic_Projectile(float px, float py, bool friendly, float vx, float vy, float duration, olc::Sprite * sprite, int tx, int ty) : Dynamic("projectile")
    {
        this->px = px;
        this->py = py;
        this->vx = vx;
        this->vy = vy;
        Friendly = friendly;
        m_duration = duration;
        m_sprite = sprite;
        m_tileX = tx;
        m_tileY = ty;
    }

    void Dynamic_Projectile::Update(float elapsed, Dynamic * player)
    {
        m_duration -= elapsed;
        if (m_duration < 0) {
            m_duration = 0;
            Redundant = true;
        }
    }

    void Dynamic_Projectile::Draw(olc::PixelGameEngine * engine, float ox, float oy)
    {
        if (!Redundant) {
            engine->DrawPartialSprite((px - ox) * Assets::TileWidth, (py - oy) * Assets::TileHeight, m_sprite, m_tileX * Assets::TileWidth, m_tileY * Assets::TileHeight, Assets::TileWidth, Assets::TileHeight);
        }
    }

    Dynamic_Creature_Player::Dynamic_Creature_Player() : Dynamic_Creature("Player", Assets::GetInstance().GetSprite("player"))
    {
        Health = 5;
        EquipedWeapon = (Weapon*)Assets::GetInstance().GetItem("sword");
    }

    void Dynamic_Creature::PerformAttack()
    {
        if (EquipedWeapon != nullptr) {
            EquipedWeapon->OnUse(this);
        }
    }

}