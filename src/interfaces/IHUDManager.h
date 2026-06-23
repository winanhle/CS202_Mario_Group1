#pragma once

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @interface IHUDManager
 * @brief Interface for HUD/UI management module
 * 
 * Implemented by: Nguyen Phuc
 * Responsible for: UI rendering, HUD display, score display, menus
 * 
 * Extension Point:
 * - Nguyen Phuc should create HUDManager implementing this interface
 * - No other modules should directly depend on HUDManager
 * - Communication happens through this interface via GameWorld
 */
class IHUDManager
{
public:
    virtual ~IHUDManager() = default;

    /**
     * @brief Initialize the HUD manager
     */
    virtual void initialize() = 0;

    /**
     * @brief Update HUD logic (e.g., animations, state changes)
     * @param deltaTime Time since last frame
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Render HUD to screen
     * @param window SFML window
     */
    virtual void render(sf::RenderWindow& window) const = 0;

    /**
     * @brief Handle input events (e.g., menu selections)
     * @param event SFML event
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * @brief Update HUD with player score
     * @param score The score to display
     */
    virtual void updateScore(int score) = 0;

    /**
     * @brief Update HUD with player lives
     * @param lives Number of lives to display
     */
    virtual void updateLives(int lives) = 0;

    /**
     * @brief Update HUD with enemy count
     * @param count Number of enemies
     */
    virtual void updateEnemyCount(int count) = 0;
};
