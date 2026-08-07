#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

enum class PowerUpType { Mushroom, FireFlower, Star };

class IPlayerForm {
public:
    virtual ~IPlayerForm() = default;
    virtual sf::IntRect getWalkFrame1() const = 0;
    virtual sf::IntRect getWalkFrame2() const = 0;
    virtual sf::IntRect getJumpFrame() const = 0;
    virtual sf::Vector2f getHitboxSize() const = 0;
    virtual std::unique_ptr<IPlayerForm> takeDamage() const = 0;
    virtual std::unique_ptr<IPlayerForm> evolve(PowerUpType type) const = 0;
};
