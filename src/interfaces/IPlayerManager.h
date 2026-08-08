#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "IMapManager.h"

class ISettingsManager;

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
     * @param settings Optional settings manager; when provided, the player
     *        builds its key bindings from the saved settings instead of
     *        hardcoded defaults
     */
    virtual void initialize(ISettingsManager* settings = nullptr) = 0;

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

    /**
     * @brief Restore a previously saved player state (score, lives, position)
     * @param score Saved score
     * @param lives Saved lives
     * @param posX Saved X position
     * @param posY Saved Y position
     */
    virtual void restoreState(int score, int lives, float posX, float posY) = 0;

    // ─── API CHO ENEMY/ITEM MANAGER ───
    virtual sf::FloatRect getHitbox() const = 0;

    // ─── BEHAVIOR ───
    virtual void setMapManager(IMapManager* map) = 0;
    virtual void takeDamage() = 0;
    virtual void bounce() = 0;
    virtual void collectCoin(int amount) = 0;
    virtual void collectPowerUp(int type) = 0;
};
