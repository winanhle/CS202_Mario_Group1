#pragma once

#include <array>
#include <functional>
#include <stdexcept>
#include <string>

#include "../EnemySprite.h"
#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

// A Koopa Troopa in disguise: bigger/slower boss that spits fireballs
// periodically while walking. The first stomp unmasks it - it retreats into
// a normal Koopa shell (reuses KoopaShell1/2 art, since it really was a
// Koopa underneath), then cycles ShellIdle/ShellMoving forever just like
// KoopaTroopa - it never truly dies.
//
// All sprites are preset paths from EnemySprite.h and are loaded lazily,
// exactly once total (shared across every Boss instance), so the
// constructor needs no texture arguments at all.
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

    // Called when the boss should spawn a fireball.
    // Params: (spawnX, spawnY, direction) - direction matches Enemy's
    // convention (-1 = left, 1 = right). Typically wired to push a
    // BossFireball into whatever container drives your enemies (see
    // BossFireball.h - it's an Enemy subclass, so EnemyManager's existing
    // m_enemies vector can hold it directly with no extra plumbing).
    using FireballSpawnFn = std::function<void(float, float, int)>;

    explicit Boss(float x, float y, FireballSpawnFn fireballSpawnFn = nullptr)
        : Enemy(x, y, *walkFrames(-1)[0]) // starts facing left, matches default m_direction = -1
        , m_fireballSpawnFn(std::move(fireballSpawnFn))
    {
        m_moveSpeed = m_walkSpeed;
    }

    void update(float dt) override
    {
        // NOTE: EnemyManager::update() already calls applyGravity() and
        // move() for every enemy after this update(). Do NOT call them again
        // here, or gravity/movement doubles up and the boss tunnels through
        // the floor/walls (see BuzzyBeetle/KoopaTroopa for the same fix).
        // Only set m_moveSpeed and handle animation/timers here.
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
                }
                break;

            case State::ShellIdle:
                m_moveSpeed = 0.f; // stays put, but still falls via EnemyManager's gravity
                // Static pose (KoopaShell1) - set once when entering this
                // state (see onStomp()/onPlayerCollision()), nothing to
                // animate here - it's a distinct pose, not an anim frame.
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
                // Normal side-touch damage
                player->takeDamage();
                break;

            case State::ShellIdle:
                // Side contact with the resting shell kicks it off
                m_state = State::ShellMoving;
                setSpriteTexture(*shellFrames()[1]); // KoopaShell2 - running
                break;

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
                // First stomp unmasks it: it retreats into its shell instead
                // of dying outright - it was a Koopa Troopa all along.
                m_state = State::ShellIdle;
                m_frameIndex = 0;
                setSpriteTexture(*shellFrames()[0]);
                break;

            case State::ShellIdle:
                // Stomping a resting shell sends it sliding
                m_state = State::ShellMoving;
                setSpriteTexture(*shellFrames()[1]); // KoopaShell2 - running
                break;

            case State::ShellMoving:
                // Stomping a moving shell stops it back to idle
                m_state = State::ShellIdle;
                setSpriteTexture(*shellFrames()[0]); // KoopaShell1 - resting
                break;
        }
    }

private:
    // ── Animation frame lookups ────────────────────────────────────────────
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