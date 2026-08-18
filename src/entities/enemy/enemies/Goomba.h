#pragma once

#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

#include <array>

class Goomba : public Enemy
{
private:
    std::array<sf::Texture*, 2> m_walkFrames;
    sf::Texture* m_deadTexture = nullptr;
    std::size_t m_currentFrame = 0;
    float m_animationTimer = 0.f;

    bool m_squished = false;
    float m_deathTimer = 0.f;

    static constexpr float FRAME_DURATION = 0.15f;
    static constexpr float DEATH_DISPLAY_DURATION = 0.6f; // seconds to show the squished sprite before it vanishes

public:
    Goomba(
        float x,
        float y,
        std::array<sf::Texture*, 2> walkFrames,
        sf::Texture& deadTexture
    )
        : Enemy(x, y, *walkFrames[0]),
          m_walkFrames(walkFrames),
          m_deadTexture(&deadTexture)
    {
    }

    void update(float deltaTime) override
    {
        if (m_squished)
        {
            // Corpse stays visible and inert while this counts down.
            m_deathTimer -= deltaTime;
            if (m_deathTimer <= 0.f)
                m_dead = true; // now render()/EnemyManager treat it as fully gone
            return;
        }

        m_animationTimer += deltaTime;

        while (m_animationTimer >= FRAME_DURATION)
        {
            m_animationTimer -= FRAME_DURATION;
            m_currentFrame = (m_currentFrame + 1) % m_walkFrames.size();
            setSpriteTexture(*m_walkFrames[m_currentFrame]);
        }
    }

    void onPlayerCollision(IPlayerManager* player) override
    {
        // A squished corpse can't hurt the player anymore
        if (!player || m_squished)
            return;

        player->takeDamage();
    }

    void onStomp() override
    {
        if (m_squished)
            return; // already stomped/killed, ignore repeat hits (e.g. a fireball)

        m_squished = true;
        m_deathTimer = DEATH_DISPLAY_DURATION;
        m_moveSpeed = 0.f; // stop walking, but EnemyManager keeps calling update() so this timer still runs
        if (m_deadTexture) setSpriteTexture(*m_deadTexture);
    }
};