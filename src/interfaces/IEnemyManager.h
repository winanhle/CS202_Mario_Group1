#pragma once
#include "IMapManager.h"
#include <SFML/Graphics/Rect.hpp>
#include <vector>
class IPlayerManager;
class ISoundManager;
struct EntitySpawnData;

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
    virtual void setPlayerManager2(IPlayerManager* player) = 0;
    virtual void setMapManager(IMapManager* map) = 0;
    virtual void setSoundManager(ISoundManager* sound) = 0;

    /**
     * @brief Enemy bị cầu lửa của player trúng.
     * @param fireballHitbox Hitbox cầu lửa hiện tại.
     * @return true nếu có ít nhất 1 enemy bị tiêu diệt (cầu lửa nên nổ).
     */
    virtual bool takeDamageFromFireball(const sf::FloatRect& fireballHitbox, class IPlayerManager* owner = nullptr) = 0;

    /**
     * @brief Spawn enemies from parsed TMX map object data.
     * Clears existing enemies and creates new ones based on the spawn data.
     * @param spawns Vector of EntitySpawnData parsed from the map's object layer
     */
    virtual void spawnFromMapData(const std::vector<EntitySpawnData>& spawns) = 0;

    /**
     * @brief Stomp-kill all enemies standing on (or overlapping) the tile
     *        row directly above (gy - 1). Called when a BRICK_NORMAL is
     *        broken by a Super or Fire player.
     * @param gx  Grid column of the broken tile.
     * @param gy  Grid row of the broken tile.
     */
    virtual void killEnemiesAboveTile(int gx, int gy) = 0;
};
