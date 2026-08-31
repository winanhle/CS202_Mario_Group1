#pragma once

#include "../entities/map/MapData.h"
#include <vector>

class IPlayerManager;

namespace sf {
class RenderWindow;
}

/**
 * @interface IFireBarManager
 * @brief Interface for the FireBar obstacle manager.
 *
 * FireBars are rotating arms of fireballs anchored to blocks.
 * The manager owns all FireBar instances, updates their rotation,
 * renders their fireballs, and resolves collision against active players.
 */
class IFireBarManager
{
public:
    virtual ~IFireBarManager() = default;

    /** Load textures. Called once by GameWorld::initialize(). */
    virtual void initialize() = 0;

    /**
     * @brief Advance rotation animation and resolve collision with player(s).
     * @param deltaTime Elapsed frame time in seconds
     */
    virtual void update(float deltaTime) = 0;

    /** Render all active FireBars. */
    virtual void render(sf::RenderWindow& window) const = 0;

    /** Inject Player 1 (always present). */
    virtual void setPlayerManager(IPlayerManager* player) = 0;

    /** Inject Player 2 (2-player mode only; may be nullptr). */
    virtual void setPlayerManager2(IPlayerManager* player) = 0;

    /**
     * @brief Clear existing firebars and spawn new ones from map data.
     * Called by GameWorld::loadCurrentLevel().
     */
    virtual void spawnFromMapData(const std::vector<FireBarSpawnData>& spawns) = 0;
};
