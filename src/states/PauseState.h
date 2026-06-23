#pragma once

#include "../core/GameState.h"
#include <string>

namespace sf {
class Event;
class RenderWindow;
}

/**
 * @class PauseState
 * @brief Game pause state
 * 
 * Displays a pause menu overlay.
 * Placeholder implementation for now.
 */
class PauseState : public GameState
{
public:
    PauseState();
    ~PauseState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    std::string m_displayText;
};
