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

    static constexpr float FRAME_DURATION = 0.15f;

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
        if (isDead()) return;

        m_animationTimer += deltaTime;

        while (m_animationTimer >= FRAME_DURATION)
        {
            m_animationTimer -= FRAME_DURATION;
            m_currentFrame = (m_currentFrame + 1) % m_walkFrames.size();
            m_sprite.setTexture(*m_walkFrames[m_currentFrame], true);
        }
    }

    void onPlayerCollision(IPlayerManager* player) override
    {
        if (!player)return;
        player->takeDamage();
    }

    void onStomp() override
    {
        m_dead = true;
        if (m_deadTexture) m_sprite.setTexture(*m_deadTexture, true);
    }
};