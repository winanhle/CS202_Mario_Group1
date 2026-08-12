#pragma once
#include <SFML/Graphics.hpp>
#include "FireballState.h"

class IMapManager;
class IEnemyManager;

/**
 * @class Fireball
 * @brief Cầu lửa (concrete entity) chạy State Machine:
 *
 *         Flying ──[va chạm Y + bounce < 2]──► Flying (bounce)
 *         Flying ──[va chạm Y + bounce == 2]──► Exploding
 *         Flying ──[va chạm X / trúng enemy]──► Exploding
 *         Flying ──[lifetime hết / ra khỏi map]──► Dead
 *         Exploding ──[animation xong]──► Dead
 *
 * Fireball chỉ giữ NON-OWNING pointer tới texture của FireballManager
 * (manager owns texture + vector<Fireball> by value).
 */
class Fireball
{
    // ── Physics ──────────────────────────────────────────────
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;       // vx = SPEED * direction, vy = 0 ban đầu
    static constexpr float SPEED       = 250.f;
    static constexpr float GRAVITY     = 600.f;
    static constexpr float BOUNCE_VY   = -200.f;
    static constexpr int   MAX_BOUNCES = 2;

    // ── State ─────────────────────────────────────────────────
    FireballState m_state       = FireballState::Flying;
    int           m_bounceCount = 0;
    float         m_lifetime    = 0.f;

    // ── Animation ─────────────────────────────────────────────
    const sf::Texture* m_texture      = nullptr;
    sf::Sprite         m_sprite;
    int                m_currentFrame = 0;
    float              m_animTimer    = 0.f;
    static constexpr float FLY_FRAME_TIME     = 0.08f; // 4 frame fly
    static constexpr float EXPLODE_FRAME_TIME = 0.07f; // 3 frame explode
    static constexpr float MAX_LIFETIME       = 4.f;

public:
    // Constructor nhận texture từ FireballManager
    Fireball(float x, float y, int direction, const sf::Texture& texture);

    void update(float dt, const IMapManager& map, IEnemyManager& enemies);
    void render(sf::RenderWindow& window) const;

    bool isDead()       const { return m_state == FireballState::Dead; }
    sf::FloatRect getHitbox() const;

private:
    void applyGravity(float dt);
    void handleTileCollision(const IMapManager& map, float dt);
    void handleEnemyCollision(IEnemyManager& enemies);
    void updateFlyAnimation(float dt);
    void updateExplodeAnimation(float dt);
    bool collidesWithMap(const IMapManager& map, float x, float y) const;
};