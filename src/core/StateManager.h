#pragma once

#include "GameState.h"
#include <memory>

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @class StateManager
 * @brief Manages the current game state and state transitions
 * 
 * Responsible for:
 * - Storing the active state
 * - Changing to a new state
 * - Delegating input, update, and render calls to the active state
 */
class StateManager
{
public:
    StateManager();
    ~StateManager() = default;

    /**
     * @brief Change to a new game state
     * @param newState The new state to activate
     */
    void changeState(GameState::Ptr newState);

    /**
     * @brief Handle input for the current state
     * @param event The SFML event to handle
     */
    void handleInput(const sf::Event& event);

    /**
     * @brief Update the current state
     * @param deltaTime Time elapsed since last update in seconds
     */
    void update(float deltaTime);

    /**
     * @brief Render the current state
     * @param window The SFML render window to draw to
     */
    void render(sf::RenderWindow& window) const;

    /**
     * @brief Check if a state is currently active
     * @return True if a state is set, false otherwise
     */
    bool hasActiveState() const;

private:
    GameState::Ptr m_currentState;
};
