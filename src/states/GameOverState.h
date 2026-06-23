#pragma once

#include "../core/GameState.h"
#include <string>

namespace sf {
class Event;
class RenderWindow;
}

/**
 * @class GameOverState
 * @brief Game over state
 * 
 * Displays a game over screen.
 * Placeholder implementation for now.
 */
class GameOverState : public GameState
{
public:
    GameOverState();
    ~GameOverState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    std::string m_displayText;
};
