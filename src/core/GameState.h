#pragma once

#include <memory>

namespace sf {
class RenderWindow;
class Event;
}

class StateManager;

/**
 * @class GameState
 * @brief Abstract base class for all game states (Menu, Play, Pause, GameOver)
 * 
 * Defines the interface that all concrete states must implement.
 * States handle their own input, updates, and rendering.
 */
class GameState
{
public:
    using Ptr = std::unique_ptr<GameState>;

    virtual ~GameState() = default;

    /**
     * @brief Handle input events
     * @param event The SFML event to handle
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * @brief Update game state logic
     * @param deltaTime Time elapsed since last update in seconds
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Render the state
     * @param window The SFML render window to draw to
     */
    virtual void render(sf::RenderWindow& window) const = 0;

protected:
    GameState() = default;
};
