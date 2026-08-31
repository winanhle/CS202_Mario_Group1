#pragma once

#include <array>
#include <stdexcept>

#include "../EnemySprite.h"
#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

class KoopaTroopa : public Enemy
{
public:
    enum class State
    {
        Walking,     // normal patrol
        ShellIdle,   // retreated into shell, not moving
        ShellMoving  // kicked shell, sliding and dangerous
    };

    static sf::Texture& left1()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::Koopa1); if (!ok) throw std::runtime_error("Koopa: failed to load Koopa1"); return t; }
    static sf::Texture& left2()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::Koopa2); if (!ok) throw std::runtime_error("Koopa: failed to load Koopa2"); return t; }
    static sf::Texture& right1() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::Koopa3); if (!ok) throw std::runtime_error("Koopa: failed to load Koopa3"); return t; }
    static sf::Texture& right2() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::Koopa4); if (!ok) throw std::runtime_error("Koopa: failed to load Koopa4"); return t; }
    static sf::Texture& shell1() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::KoopaShell1); if (!ok) throw std::runtime_error("Koopa: failed to load KoopaShell1"); return t; }
    static sf::Texture& shell2() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::KoopaShell2); if (!ok) throw std::runtime_error("Koopa: failed to load KoopaShell2"); return t; }

    explicit KoopaTroopa(float x, float y)
        : Enemy(x, y, left1())
        , m_leftFrames({ &left1(), &left2() })
        , m_rightFrames({ &right1(), &right2() })
        , m_shellFrames({ &shell1(), &shell2() })
    {}

    // leftFrames  = {Koopa1, Koopa2}          -> walking left
    // rightFrames = {Koopa3, Koopa4}          -> walking right
    // shellFrames = {KoopaShell1, KoopaShell2} -> shell spin, idle and moving
    KoopaTroopa(float x, float y,
                std::array<sf::Texture*, 2> leftFrames,
                std::array<sf::Texture*, 2> rightFrames,
                std::array<sf::Texture*, 2> shellFrames)
        : Enemy(x, y, *leftFrames[0]) // starts facing left, matches default m_direction = -1
        , m_leftFrames(leftFrames)
        , m_rightFrames(rightFrames)
        , m_shellFrames(shellFrames)
    {}

    void update(float dt) override
    {
        // NOTE: EnemyManager::update() already calls applyGravity() and move()
        // for every enemy after this update(). Do NOT call them again here,
        // or gravity/movement gets applied twice per frame and enemies will
        // tunnel through the floor/walls. Only set m_moveSpeed and animate.
        switch (m_state)
        {
            case State::Walking:
                m_moveSpeed = m_walkSpeed;
                updateAnimation(dt, m_walkAnimInterval);
                break;

            case State::ShellIdle:
                m_moveSpeed = 0.f; // stays put, but still falls via EnemyManager's gravity
                updateAnimation(dt, m_shellIdleAnimInterval);
                break;

            case State::ShellMoving:
                m_moveSpeed = m_shellSpeed;
                updateAnimation(dt, m_shellMovingAnimInterval);
                break;
        }
    }

    void onPlayerCollision(IPlayerManager* player) override
    {
        if (!player)
            return;

        switch (m_state)
        {
            case State::Walking:
                // Normal side-touch damage
                player->takeDamage();
                break;

            case State::ShellIdle:
                // Side contact with a resting shell kicks it off
                m_state = State::ShellMoving;
                break;

            case State::ShellMoving:
                // Getting hit by a sliding shell hurts
                player->takeDamage();
                break;
        }
    }

    void onStomp() override
    {
        switch (m_state)
        {
            case State::Walking:
                // First stomp: retreat into the shell instead of dying
                m_state = State::ShellIdle;
                m_velocity.x = 0.f;
                break;

            case State::ShellIdle:
                // Stomping a resting shell sends it sliding
                m_state = State::ShellMoving;
                break;

            case State::ShellMoving:
                // Stomping a moving shell stops it back to idle
                m_state = State::ShellIdle;
                m_velocity.x = 0.f;
                break;
        }
    }

private:
    void updateAnimation(float dt, float interval)
    {
        m_animTimer += dt;
        if (m_animTimer >= interval)
        {
            m_animTimer = 0.f;
            m_frameIndex = (m_frameIndex + 1) % 2;
        }

        if (m_state == State::Walking)
        {
            const auto& frames = (m_direction < 0) ? m_leftFrames : m_rightFrames;
            setSpriteTexture(*frames[m_frameIndex]);
        }
        else
        {
            setSpriteTexture(*m_shellFrames[m_frameIndex]);
        }
    }

    std::array<sf::Texture*, 2> m_leftFrames;
    std::array<sf::Texture*, 2> m_rightFrames;
    std::array<sf::Texture*, 2> m_shellFrames;

    int m_frameIndex = 0;
    float m_animTimer = 0.f;
    float m_walkAnimInterval = 0.2f;        // seconds between walk frames
    float m_shellIdleAnimInterval = 0.5f;   // slow flicker while resting
    float m_shellMovingAnimInterval = 0.08f; // fast spin while sliding

    State m_state = State::Walking;
    float m_walkSpeed = 40.f;
    float m_shellSpeed = 200.f;
};