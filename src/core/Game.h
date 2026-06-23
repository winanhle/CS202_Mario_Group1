#pragma once

#include "StateManager.h"
#include <memory>
#include <SFML/Graphics.hpp>

/**
 * @class Game
 * @brief Main game class that manages the window, game loop, and state manager
 * 
 * Responsibilities:
 * - Owns the SFML render window
 * - Owns the StateManager
 * - Implements the main game loop
 * - Handles frame timing
 */
class Game
{
public:
    Game();
    ~Game() = default;

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    /**
     * @brief Run the main game loop
     */
    void run();

private:
    /**
     * @brief Process all pending events
     */
    void handleEvents();

    /**
     * @brief Update game logic
     */
    void update();

    /**
     * @brief Render current frame
     */
    void render();

    static constexpr float FRAME_RATE = 60.0f;
    static constexpr float FRAME_TIME = 1.0f / FRAME_RATE;

    std::unique_ptr<sf::RenderWindow> m_window;
    StateManager m_stateManager;
    sf::Clock m_frameClock;
};
