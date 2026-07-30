#pragma once
#include "IPlayerForm.h"

class NormalForm : public IPlayerForm {
public:
    sf::IntRect getWalkFrame1() const override { return {{1, 6}, {15, 18}}; }
    sf::IntRect getWalkFrame2() const override { return {{18, 6}, {15, 18}}; }
    sf::IntRect getJumpFrame() const override  { return {{52, 6}, {15, 18}}; }
    sf::Vector2f getHitboxSize() const override { return {15.f, 18.f}; }
    std::unique_ptr<IPlayerForm> takeDamage() const override;
    std::unique_ptr<IPlayerForm> evolve(PowerUpType type) const override;
};
