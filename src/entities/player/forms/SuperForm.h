#pragma once
#include "IPlayerForm.h"

class SuperForm : public IPlayerForm {
public:
    sf::IntRect getWalkFrame1() const override { return {{1, 31}, {15, 26}}; }
    sf::IntRect getWalkFrame2() const override { return {{18, 31}, {15, 26}}; }
    sf::IntRect getJumpFrame() const override  { return {{52, 31}, {15, 26}}; }
    sf::Vector2f getHitboxSize() const override { return {15.f, 26.f}; }
    std::unique_ptr<IPlayerForm> takeDamage() const override;
    std::unique_ptr<IPlayerForm> evolve(PowerUpType type) const override;
};
