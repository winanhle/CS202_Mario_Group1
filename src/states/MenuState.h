#pragma once

#include "../core/GameState.h"
#include <string>

namespace sf {
class Event;
class RenderWindow;
}

/**
 * @class MenuState
 * @brief Main menu state
 * 
 * Displays the main menu to the player.
 * Placeholder implementation for now.
 */
class MenuState : public GameState
{
public:
    MenuState();
    ~MenuState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    std::string m_displayText;
};
