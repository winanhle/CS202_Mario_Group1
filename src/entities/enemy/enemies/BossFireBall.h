#pragma once

#include <array>
#include <stdexcept>

#include "../EnemySprite.h"
#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

// The projectile Boss::fireFireball() spawns. Deliberately an Enemy
// subclass, not a separate entity type - that lets it drop straight into
// EnemyManager's existing m_enemies vector and get gravity, movement, wall
// bounce, and player-collision handling for free, with zero new manager
// code required. It just adds its own visuals and a couple of extra rules:
// hurts the player on touch (and is consumed by the hit), can be stomped
// out like a normal enemy, and expires on its own after a short lifetime so
// it doesn't fly across the level forever.
//
// Sprites are preset paths from EnemySprite.h, loaded lazily once and
// shared across every fireball instance - no texture arguments needed.
class BossFireball : public Enemy
{
public:
    // direction matches Enemy's convention: -1 = left, 1 = right.
    BossFireball(float x, float y, int direction)
        : Enemy(x, y, *frames(direction)[0])
    {
        m_direction = direction;
        m_moveSpeed = m_flySpeed;
    }

    void update(float dt) override
    {
        // NOTE: EnemyManager::update() already calls applyGravity() and
        // move() after this update() - don't call them again here.
        m_lifetime -= dt;
        if (m_lifetime <= 0.f)
        {
            m_dead = true; // quietly expires, no death animation needed
            return;
        }

        m_animTimer += dt;
        if (m_animTimer >= m_animInterval)
        {
            m_animTimer = 0.f;
            m_frameIndex = (m_frameIndex + 1) % 2;
            setSpriteTexture(*frames(m_direction)[m_frameIndex]);
        }
    }

    void onPlayerCollision(IPlayerManager* player) override
    {
        if (!player)
            return;

        player->takeDamage();
        m_dead = true; // consumed on hit, like a real fireball would be
    }

    void onStomp() override
    {
        // Stompable like a regular enemy for a forgiving feel. If you'd
        // rather fireballs be immune to stomping (closer to the original
        // arcade game), just make this a no-op instead.
        Enemy::onStomp();
    }

private:
    static std::array<sf::Texture*, 2> frames(int direction)
    {
        if (direction < 0) return { &left1(), &left2() };
        return { &right1(), &right2() };
    }

    // See Boss.h for why each texture needs its own dedicated static
    // accessor rather than one shared generic loader.
    static sf::Texture& left1()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossFireLeft1);  if (!ok) throw std::runtime_error("BossFireball: failed to load BossFireLeft1");  return t; }
    static sf::Texture& left2()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossFireLeft2);  if (!ok) throw std::runtime_error("BossFireball: failed to load BossFireLeft2");  return t; }
    static sf::Texture& right1() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossFireRight1); if (!ok) throw std::runtime_error("BossFireball: failed to load BossFireRight1"); return t; }
    static sf::Texture& right2() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossFireRight2); if (!ok) throw std::runtime_error("BossFireball: failed to load BossFireRight2"); return t; }

    int m_frameIndex = 0;
    float m_animTimer = 0.f;
    float m_animInterval = 0.1f; // fast flicker, reads as "on fire"

    float m_flySpeed = 150.f;
    float m_lifetime = 4.f; // seconds before it despawns on its own
};