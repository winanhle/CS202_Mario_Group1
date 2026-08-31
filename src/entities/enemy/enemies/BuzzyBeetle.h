#pragma once

#include <array>
#include <stdexcept>

#include "../EnemySprite.h"
#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

class BuzzyBeetle : public Enemy
{
public:
    static sf::Texture& left1()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BuzzyBeetle1); if (!ok) throw std::runtime_error("BuzzyBeetle: failed to load BuzzyBeetle1"); return t; }
    static sf::Texture& left2()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BuzzyBeetle2); if (!ok) throw std::runtime_error("BuzzyBeetle: failed to load BuzzyBeetle2"); return t; }
    static sf::Texture& shell()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BuzzyBeetle3); if (!ok) throw std::runtime_error("BuzzyBeetle: failed to load BuzzyBeetle3"); return t; }
    static sf::Texture& right1() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BuzzyBeetle4); if (!ok) throw std::runtime_error("BuzzyBeetle: failed to load BuzzyBeetle4"); return t; }
    static sf::Texture& right2() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BuzzyBeetle5); if (!ok) throw std::runtime_error("BuzzyBeetle: failed to load BuzzyBeetle5"); return t; }

    explicit BuzzyBeetle(float x, float y)
        : Enemy(x, y, left1())
        , m_leftFrames({ &left1(), &left2() })
        , m_rightFrames({ &right1(), &right2() })
        , m_shellTexture(&shell())
    {}

    // leftFrames  = {BuzzyBeetle1, BuzzyBeetle2}  -> walking left
    // rightFrames = {BuzzyBeetle4, BuzzyBeetle5}  -> walking right
    // shellTexture = BuzzyBeetle3                 -> flipped/shell state
    BuzzyBeetle(float x, float y,
                std::array<sf::Texture*, 2> leftFrames,
                std::array<sf::Texture*, 2> rightFrames,
                sf::Texture& shellTexture)
        : Enemy(x, y, *leftFrames[0]) // starts facing left, matches default m_direction = -1
        , m_leftFrames(leftFrames)
        , m_rightFrames(rightFrames)
        , m_shellTexture(&shellTexture)
    {}

    void update(float dt) override
    {
        // NOTE: EnemyManager::update() already calls applyGravity() and move()
        // for every enemy after this update(). Do NOT call them again here,
        // or gravity/movement gets applied twice per frame and enemies will
        // tunnel through the floor/walls. Only set m_moveSpeed and animate.
        if (m_flipped)
        {
            m_moveSpeed = 0.f; // stays put, but still falls via EnemyManager's gravity

            m_flipTimer -= dt;
            if (m_flipTimer <= 0.f)
                m_flipped = false; // rights itself and resumes walking
        }
        else
        {
            m_moveSpeed = m_walkSpeed;
            updateWalkAnimation(dt);
        }
    }

    void onPlayerCollision(IPlayerManager* player) override
    {
        if (!player)
            return;

        if (m_flipped)
        {
            // Just brushing against the resting shell: harmless, but resets
            // the stand-up timer so the player lingering nearby keeps it down.
            m_flipTimer = m_flipDuration;
            return;
        }

        player->takeDamage();
    }

    void onStomp() override
    {
        if (!m_flipped)
        {
            // First stomp knocks it onto its back instead of killing it
            m_flipped = true;
            m_flipTimer = m_flipDuration;
            m_velocity.x = 0.f;
            setSpriteTexture(*m_shellTexture);
        }
        else
        {
            // Stomping it again while flipped finishes it off
            Enemy::onStomp();
        }
    }

private:
    void updateWalkAnimation(float dt)
    {
        m_animTimer += dt;
        if (m_animTimer >= m_animInterval)
        {
            m_animTimer = 0.f;
            m_frameIndex = (m_frameIndex + 1) % 2;
        }

        const auto& frames = (m_direction < 0) ? m_leftFrames : m_rightFrames;
        setSpriteTexture(*frames[m_frameIndex]);
    }

    std::array<sf::Texture*, 2> m_leftFrames;
    std::array<sf::Texture*, 2> m_rightFrames;
    sf::Texture* m_shellTexture;

    int m_frameIndex = 0;
    float m_animTimer = 0.f;
    float m_animInterval = 0.2f; // seconds between walk frames

    bool m_flipped = false;
    float m_flipTimer = 0.f;
    float m_flipDuration = 5.f; // seconds before it flips back onto its feet
    float m_walkSpeed = 40.f;
};