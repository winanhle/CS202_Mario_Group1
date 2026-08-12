#pragma once
#include "IPlayerForm.h"

/**
 * @class NormalForm
 * @brief Dạng nhỏ mặc định của player.
 *
 * Nhận CharacterFormOffsets từ subclass PlayerManager để dùng đúng
 * pixel offset của sprite sheet cho từng nhân vật (Mario / Luigi / ...).
 */
class NormalForm : public IPlayerForm {
    FormFrames                  m_frames;
    const CharacterFormOffsets* m_offsets; // owned by static const in Mario/Luigi
public:
    explicit NormalForm(const CharacterFormOffsets& offsets)
        : m_frames(offsets.normalForm), m_offsets(&offsets) {}

    sf::IntRect  getWalkFrame1() const override { return m_frames.walkFrame1; }
    sf::IntRect  getWalkFrame2() const override { return m_frames.walkFrame2; }
    sf::IntRect  getJumpFrame()  const override { return m_frames.jumpFrame;  }
    sf::Vector2f getHitboxSize() const override { return m_frames.hitboxSize; }

    std::unique_ptr<IPlayerForm> takeDamage() const override;
    std::unique_ptr<IPlayerForm> evolve(PowerUpType type) const override;
};
