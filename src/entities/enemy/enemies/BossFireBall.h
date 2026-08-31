#pragma once

#include <array>
#include <stdexcept>

#include "../EnemySprite.h"
#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

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
        m_lifetime -= dt;
        if (m_lifetime <= 0.f)
        {
            m_dead = true; // quietly expires, no death animation needed
            return;
        }

        m_sprite.move({m_flySpeed * static_cast<float>(m_direction) * dt, 0.f});

        m_animTimer += dt;
        if (m_animTimer >= m_animInterval)
        {
            m_animTimer = 0.f;
            m_frameIndex = (m_frameIndex + 1) % 2;
            setSpriteTexture(*frames(m_direction)[m_frameIndex]);
        }
    }

    bool usesPhysics() const override
    {
        return false;
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
        Enemy::onStomp();
    }

public:
    // See Boss.h for why each texture needs its own dedicated static
    // accessor rather than one shared generic loader.
    static sf::Texture& left1()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossFireLeft1);  if (!ok) throw std::runtime_error("BossFireball: failed to load BossFireLeft1");  return t; }
    static sf::Texture& left2()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossFireLeft2);  if (!ok) throw std::runtime_error("BossFireball: failed to load BossFireLeft2");  return t; }
    static sf::Texture& right1() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossFireRight1); if (!ok) throw std::runtime_error("BossFireball: failed to load BossFireRight1"); return t; }
    static sf::Texture& right2() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossFireRight2); if (!ok) throw std::runtime_error("BossFireball: failed to load BossFireRight2"); return t; }

private:
    static std::array<sf::Texture*, 2> frames(int direction)
    {
        if (direction < 0) return { &left1(), &left2() };
        return { &right1(), &right2() };
    }

    int m_frameIndex = 0;
    float m_animTimer = 0.f;
    float m_animInterval = 0.1f; // fast flicker, reads as "on fire"

    float m_flySpeed = 150.f;
    float m_lifetime = 4.f; // seconds before it despawns on its own
};