#pragma once

namespace sf {
class RenderWindow;
}

/**
 * @interface IItemManager
 * @brief Interface for item management module
 * 
 * Implemented by: Dinh Anh
 * Responsible for: Items, coins, power-ups, item spawning
 * 
 * Extension Point:
 * - Dinh Anh should create ItemManager implementing this interface
 * - No other modules should directly depend on ItemManager
 * - Communication happens through this interface via GameWorld
 */
class IItemManager
{
public:
    virtual ~IItemManager() = default;

    /**
     * @brief Initialize the item manager
     */
    virtual void initialize() = 0;

    /**
     * @brief Update item logic
     * @param deltaTime Time since last frame
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Render all items
     * @param window SFML window
     */
    virtual void render(sf::RenderWindow& window) const = 0;

    /**
     * @brief Get number of items in the world
     * @return Count of items
     */
    virtual int getItemCount() const = 0;
};
