#pragma once
#include "IMapManager.h"
class IPlayerManager;

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

    // ─── NHẬN DEPENDENCY ───
    virtual void setPlayerManager(IPlayerManager* player) = 0;

    /**
     * @brief Gán Player 2 (chỉ trong chế độ 2P).
     *        Item sẽ check collision với cả 2 player qua abstract interface.
     */
    virtual void setPlayerManager2(IPlayerManager* player) = 0;

    // ─── SPAWN API (called by MapManager on tile interactions) ───

    /**
     * @brief Spawn a coin-pop animation at the given world position
     *        and immediately award the player 1 coin via collectCoin(1).
     * @param worldX  Pixel X of the tile's left edge
     * @param worldY  Pixel Y of the tile's top edge
     */
    virtual void spawnCoinPop(float worldX, float worldY) = 0;

    /**
     * @brief Spawn a Mushroom item above the given world position.
     */
    virtual void spawnMushroom(float worldX, float worldY) = 0;

    /**
     * @brief Spawn a FireFlower item above the given world position.
     */
    virtual void spawnFireFlower(float worldX, float worldY) = 0;
    virtual void spawnStar(float worldX, float worldY) = 0;
    virtual void setMapManager(IMapManager* map) = 0;
};
