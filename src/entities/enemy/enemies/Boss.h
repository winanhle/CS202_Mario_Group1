#pragma once

#include <array>
#include <functional>
#include <stdexcept>
#include <string>

#include "../EnemySprite.h"
#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

class Boss : public Enemy
{
public:
    enum class State
    {
        Walking,     // patrolling, disguised as a boss
        Attacking,   // mouth opening/closing, animated, about to spit a fireball
        ShellIdle,   // unmasked - retreated into shell, not moving
        ShellMoving  // kicked shell, sliding and dangerous
    };

    using FireballSpawnFn = std::function<void(float, float, int)>;

    explicit Boss(float x, float y, FireballSpawnFn fireballSpawnFn = nullptr)
        : Enemy(x, y, *walkFrames(-1)[0]) // starts facing left, matches default m_direction = -1
        , m_fireballSpawnFn(std::move(fireballSpawnFn))
    {
        m_moveSpeed = m_walkSpeed;
        setBossScale();
    }

    void update(float dt) override
    {
        switch (m_state)
        {
            case State::Walking:
                m_moveSpeed = m_walkSpeed;
                updateAnimation(dt, m_walkAnimInterval, walkFrames(m_direction));
                updateAttackTimer(dt);
                break;

            case State::Attacking:
                m_moveSpeed = 0.f; // stands still while opening its mouth
                updateAnimation(dt, m_attackAnimInterval, attackFrames(m_direction));
                m_attackPoseTimer -= dt;
                if (m_attackPoseTimer <= 0.f)
                {
                    fireFireball();
                    m_state = State::Walking;
                    m_frameIndex = 0;
                    m_animTimer = 0.f;

                    setBossScale();
                    setSpriteTexture(*walkFrames(m_direction)[0]);
                }
                break;

            case State::ShellIdle:
                m_moveSpeed = 0.f; 
                break;

            case State::ShellMoving:
                m_moveSpeed = m_shellSpeed;
                // Static pose (KoopaShell2) - same as above.
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
            case State::Attacking:
                player->takeDamage();
                break;

            case State::ShellIdle: {
                // Player touches resting shell → kick shell
                m_state = State::ShellMoving;

                setShellScale();
                setSpriteTexture(*shellFrames()[1]);

                break;
            }

            case State::ShellMoving:
                // Getting hit by the sliding shell hurts
                player->takeDamage();
                break;
        }
    }

    void onStomp() override
    {
        switch (m_state)
        {
            case State::Walking:
            case State::Attacking:
            {
                // Boss gets stomped → hide inside shell
                m_state = State::ShellIdle;

                m_frameIndex = 0;
                m_animTimer = 0.f;

                setShellScale();
                setSpriteTexture(*shellFrames()[0]);

                break;
            }

            case State::ShellIdle:
            {
                // Stomp resting shell → kick it
                m_state = State::ShellMoving;

                setShellScale();
                setSpriteTexture(*shellFrames()[1]);

                break;
            }

            case State::ShellMoving:
            {
                // Stomp moving shell → stop it
                m_state = State::ShellIdle;

                setShellScale();
                setSpriteTexture(*shellFrames()[0]);

                break;
            }
        }
    }
private:

    void setBossScale() {
        m_sprite.setScale({1.f, 1.f});
    }

    void setShellScale() {
        m_sprite.setScale({0.5f, 0.5f});
    }

    static std::array<sf::Texture*, 2> walkFrames(int direction)
    {
        if (direction < 0) return { &walkLeft1(), &walkLeft2() };
        return { &walkRight1(), &walkRight2() };
    }

    static std::array<sf::Texture*, 2> attackFrames(int direction)
    {
        if (direction < 0) return { &attackLeft1(), &attackLeft2() };
        return { &attackRight1(), &attackRight2() };
    }

    static std::array<sf::Texture*, 2> shellFrames()
    {
        return { &shell1(), &shell2() };
    }

    void updateAnimation(float dt, float interval, const std::array<sf::Texture*, 2>& frames)
    {
        m_animTimer += dt;
        if (m_animTimer >= interval)
        {
            m_animTimer = 0.f;
            m_frameIndex = (m_frameIndex + 1) % 2;
            setSpriteTexture(*frames[m_frameIndex]);
        }
    }

    void updateAttackTimer(float dt)
    {
        m_attackCooldownTimer -= dt;
        if (m_attackCooldownTimer <= 0.f)
        {
            m_state = State::Attacking;
            m_attackPoseTimer = m_attackPoseDuration;
            m_frameIndex = 0;
            setSpriteTexture(*attackFrames(m_direction)[0]);
        }
    }

    void fireFireball()
    {
        if (m_fireballSpawnFn)
        {
            sf::FloatRect bounds = getHitbox();
            float spawnX = bounds.position.x + bounds.size.x / 2.f;
            float spawnY = bounds.position.y + bounds.size.y / 2.f;
            m_fireballSpawnFn(spawnX, spawnY, m_direction);
        }
        m_attackCooldownTimer = m_attackCooldownDuration;
    }

public:
    static sf::Texture& walkLeft1()    { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossWalkingLeft1);    if (!ok) throw std::runtime_error("Boss: failed to load BossWalkingLeft1");    return t; }
    static sf::Texture& walkLeft2()    { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossWalkingLeft2);    if (!ok) throw std::runtime_error("Boss: failed to load BossWalkingLeft2");    return t; }
    static sf::Texture& walkRight1()   { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossWalkingRight1);   if (!ok) throw std::runtime_error("Boss: failed to load BossWalkingRight1");   return t; }
    static sf::Texture& walkRight2()   { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossWalkingRight2);   if (!ok) throw std::runtime_error("Boss: failed to load BossWalkingRight2");   return t; }
    static sf::Texture& attackLeft1()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossAttackingLeft1);  if (!ok) throw std::runtime_error("Boss: failed to load BossAttackingLeft1");  return t; }
    static sf::Texture& attackLeft2()  { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossAttackingLeft2);  if (!ok) throw std::runtime_error("Boss: failed to load BossAttackingLeft2");  return t; }
    static sf::Texture& attackRight1() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossAttackingRight1); if (!ok) throw std::runtime_error("Boss: failed to load BossAttackingRight1"); return t; }
    static sf::Texture& attackRight2() { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::BossAttackingRight2); if (!ok) throw std::runtime_error("Boss: failed to load BossAttackingRight2"); return t; }
    static sf::Texture& shell1()       { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::KoopaShell1);         if (!ok) throw std::runtime_error("Boss: failed to load KoopaShell1");         return t; }
    static sf::Texture& shell2()       { static sf::Texture t; static bool ok = t.loadFromFile(EnemySprite::KoopaShell2);         if (!ok) throw std::runtime_error("Boss: failed to load KoopaShell2");         return t; }

private:

    FireballSpawnFn m_fireballSpawnFn;

    State m_state = State::Walking;

    int m_frameIndex = 0;
    float m_animTimer = 0.f;
    float m_walkAnimInterval = 0.2f;
    float m_attackAnimInterval = 0.15f;

    float m_walkSpeed = 30.f;   // a bit slower/heavier-feeling than a regular Koopa
    float m_shellSpeed = 220.f;

    float m_attackCooldownDuration = 3.f; // seconds between fireballs while walking
    float m_attackCooldownTimer = m_attackCooldownDuration;
    float m_attackPoseDuration = 0.5f;    // covers both attack frames before firing
    float m_attackPoseTimer = 0.f;
};