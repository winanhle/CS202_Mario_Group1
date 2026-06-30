#pragma once
#include <SFML/Window/Event.hpp>
class PlayerManager;

// State Pattern
class PlayerState {
public:
    virtual ~PlayerState() = default;
    virtual void handleInput(PlayerManager& player, const sf::Event& event) = 0;
    virtual void update(PlayerManager& player, float deltaTime) = 0;
    virtual void enter(PlayerManager& player) = 0;
};