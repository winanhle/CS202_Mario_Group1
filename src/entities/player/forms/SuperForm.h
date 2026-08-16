#pragma once
#include "IPlayerForm.h"

/**
 * @class SuperForm
 * @brief Dạng lớn (sau khi ăn Mushroom).
 */
class SuperForm : public IPlayerForm {
    FormFrames                  m_frames;
    const CharacterFormOffsets* m_offsets;
public:
    explicit SuperForm(const CharacterFormOffsets& offsets)
        : m_frames(offsets.superForm), m_offsets(&offsets) {}

    sf::IntRect  getWalkFrame1() const override { return m_frames.walkFrame1; }
    sf::IntRect  getWalkFrame2() const override { return m_frames.walkFrame2; }
    sf::IntRect  getJumpFrame()  const override { return m_frames.jumpFrame;  }
    sf::Vector2f getHitboxSize() const override { return m_frames.hitboxSize; }

    std::unique_ptr<IPlayerForm> takeDamage() const override;
    std::unique_ptr<IPlayerForm> evolve(PowerUpType type) const override;
};
