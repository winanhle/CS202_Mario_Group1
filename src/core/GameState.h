#pragma once

#include <memory>
#include <SFML/Graphics/Text.hpp>

namespace sf {
class RenderWindow;
class Event;
}

inline void centerOrigin(sf::Text& text)
{
    const auto bounds = text.getLocalBounds();
    text.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                     bounds.position.y + bounds.size.y / 2.0f });
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

    // Set by the StateManager when the state is activated, so the state can
    // request transitions (changeState / pushState / popState).
    void setStateManager(StateManager* manager) { m_stateManager = manager; }

protected:
    GameState() = default;

    /** @brief Get the owning StateManager (for state transitions) */
    StateManager* getStateManager() const { return m_stateManager; }
private:
    /** Pointer to the owning StateManager (for state transitions) */
    StateManager* m_stateManager = nullptr;
};
