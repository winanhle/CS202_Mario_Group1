#pragma once
#include "IPlayerForm.h"

/**
 * @class FireForm
 * @brief Dạng lửa (sau khi ăn FireFlower).
 */
class FireForm : public IPlayerForm {
    FormFrames                  m_frames;
    const CharacterFormOffsets* m_offsets;

    // ── Shoot Cooldown ────────────────────────────────────────
    float m_shootCooldown    = 0.f;
    static constexpr float SHOOT_COOLDOWN_MAX = 0.5f; // giây

public:
    explicit FireForm(const CharacterFormOffsets& offsets)
        : m_frames(offsets.fireForm), m_offsets(&offsets) {}

    sf::IntRect  getWalkFrame1() const override { return m_frames.walkFrame1; }
    sf::IntRect  getWalkFrame2() const override { return m_frames.walkFrame2; }
    sf::IntRect  getJumpFrame()  const override { return m_frames.jumpFrame;  }
    sf::Vector2f getHitboxSize() const override { return m_frames.hitboxSize; }

    std::unique_ptr<IPlayerForm> takeDamage() const override;
    std::unique_ptr<IPlayerForm> evolve(PowerUpType type) const override;

    // ── Shoot Cooldown ────────────────────────────────────────
    // Gọi mỗi frame để tick down cooldown
    void update(float deltaTime) override { updateCooldown(deltaTime); }
    void updateCooldown(float dt)
    {
        if (m_shootCooldown > 0.f)
            m_shootCooldown -= dt;
    }

    // Trả về true nếu có thể bắn
    bool canShoot() const { return m_shootCooldown <= 0.f; }

    // Gọi sau khi bắn để reset cooldown
    void triggerShootCooldown() { m_shootCooldown = SHOOT_COOLDOWN_MAX; }
};
