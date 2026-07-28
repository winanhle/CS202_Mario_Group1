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

    /**
     * @brief Set the owning StateManager
     * @param manager Pointer to the StateManager that owns this state
     * 
     * Called automatically by StateManager::changeState().
     * States can use this to trigger state transitions.
     */
    void setStateManager(StateManager* manager) { m_stateManager = manager; }

protected:
    GameState() = default;

    /** @brief Get the owning StateManager (for state transitions) */
    StateManager* getStateManager() const { return m_stateManager; }

private:
    /** Pointer to the owning StateManager (for state transitions) */
    StateManager* m_stateManager = nullptr;
};
