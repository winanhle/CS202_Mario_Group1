#pragma once

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @interface IPlayerManager
 * @brief Interface for player management module
 * 
 * Implemented by: Le Tran
 * Responsible for: Mario character, player input, player state
 * 
 * Extension Point:
 * - Le Tran should create PlayerManager implementing this interface
 * - No other modules should directly depend on PlayerManager
 * - Communication happens through this interface via GameWorld
 */
class IPlayerManager
{
public:
    virtual ~IPlayerManager() = default;

    /**
     * @brief Initialize the player manager
     */
    virtual void initialize() = 0;

    /**
     * @brief Update player logic
     * @param deltaTime Time since last frame
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Render player to screen
     * @param window SFML window
     */
    virtual void render(sf::RenderWindow& window) const = 0;

    /**
     * @brief Handle input events
     * @param event SFML event
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * @brief Check if player is alive
     * @return True if player is alive
     */
    virtual bool isAlive() const = 0;

    /**
     * @brief Get player score
     * @return Current score
     */
    virtual int getScore() const = 0;

    /**
     * @brief Get remaining lives
     * @return Number of lives
     */
    virtual int getLives() const = 0;

    /**
     * @brief Get player position for queries
     * @return X coordinate of player
     */
    virtual float getPositionX() const = 0;

    /**
     * @brief Get player position for queries
     * @return Y coordinate of player
     */
    virtual float getPositionY() const = 0;
};
