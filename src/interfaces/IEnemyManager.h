#pragma once

class IPlayerManager;

namespace sf {
class RenderWindow;
}

/**
 * @interface IEnemyManager
 * @brief Interface for enemy management module
 * 
 * Implemented by: Dinh Anh
 * Responsible for: Enemy AI, enemy spawning, enemy state, enemy collision
 * 
 * Extension Point:
 * - Dinh Anh should create EnemyManager implementing this interface
 * - No other modules should directly depend on EnemyManager
 * - Communication happens through this interface via GameWorld
 */
class IEnemyManager
{
public:
    virtual ~IEnemyManager() = default;

    /**
     * @brief Initialize the enemy manager
     */
    virtual void initialize() = 0;

    /**
     * @brief Update enemy logic
     * @param deltaTime Time since last frame
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Render all enemies
     * @param window SFML window
     */
    virtual void render(sf::RenderWindow& window) const = 0;

    /**
     * @brief Get number of active enemies
     * @return Count of enemies
     */
    virtual int getEnemyCount() const = 0;

    // ─── NHẬN DEPENDENCY ───
    virtual void setPlayerManager(IPlayerManager* player) = 0;
};
