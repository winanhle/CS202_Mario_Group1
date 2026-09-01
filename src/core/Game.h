#pragma once

#include "StateManager.h"
#include <memory>
#include <SFML/Graphics.hpp>

class ISettingsManager;
class ISaveManager;
class ISoundManager;

/**
 * @class Game
 * @brief Main game class that manages the window, game loop, and state manager
 * 
 * Responsibilities:
 * - Owns the SFML render window
 * - Owns the StateManager
 * - Owns and injects shared managers (Settings, SaveManager, SoundManager)
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

    // Shared settings instance, injected into every state that needs it
    std::shared_ptr<ISettingsManager> m_settings;
    // Shared save manager instance, injected into every state that needs it
    std::shared_ptr<ISaveManager> m_saveManager;
    // Shared sound manager instance, injected into every state that needs it
    std::shared_ptr<ISoundManager> m_soundManager;
};
