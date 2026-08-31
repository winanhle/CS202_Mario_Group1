#pragma once

#include "GameState.h"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <set>
#include <vector>
#include <SFML/Graphics/RectangleShape.hpp>

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
 *
 * Uses a stack of states so overlays (e.g. pause) can sit on top of the
 * game world. Transitions are queued and applied after the current state's
 * input/update call returns, so a state can safely pop or replace itself.
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

    // Add a state on top, keeping the one below (play -> pause)
    void pushState(GameState::Ptr newState);

    // Remove the top state, revealing the one below (pause -> play)
    void popState();

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
    enum class PendingOp
    {
        Change,
        Push,
        Pop
    };

    struct PendingTransition
    {
        PendingOp op;
        GameState::Ptr state; // used by Change and Push
    };

    GameState* currentState() const;
    void applyPending();

    std::vector<GameState::Ptr> m_stateStack;
    std::set<sf::Keyboard::Key> m_heldKeys; // keys currently down, used to skip OS auto-repeat
    std::vector<PendingTransition> m_pendingTransitions;

    // Fade-to-black transition state
    enum class FadePhase { None, FadeOut, FadeIn };
    FadePhase m_fadePhase = FadePhase::None;
    float m_fadeTimer = 0.f;
    static constexpr float FADE_DURATION = 0.3f;
    GameState::Ptr m_pendingFadeState;
    mutable sf::RectangleShape m_fadeRect;
};