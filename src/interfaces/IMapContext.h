#pragma once

#include "../entities/map/MapData.h"

/**
 * @interface IMapContext
 * @brief Narrow context interface passed to IBlockBehavior::onHitFromBelow.
 *
 * Decouples block behaviors from the concrete MapManager class (DIP).
 * MapManager implements this interface; behaviors only see this surface.
 *
 * SOLID: Dependency-Inversion — IBlockBehavior depends on an abstraction,
 *        not on the concrete MapManager.
 */
class IMapContext
{
public:
    virtual ~IMapContext() = default;

    /** Atomically update the logical type and visual GID of a tile. */
    virtual void setTile(int gx, int gy, TileType type) = 0;

    /** Spawn 4 quarter-brick debris particles flying outward from tile (gx, gy). */
    virtual void spawnBrickDebris(int gx, int gy) = 0;

    /**
     * Spawn a coin-pop animation above tile (gx, gy).
     * Score / coin award is handled by the calling behavior via IPlayerManager.
     */
    virtual void spawnCoinPop(int gx, int gy) = 0;

    /**
     * Spawn Mushroom (formType == 0) or FireFlower (formType > 0)
     * one tile above (gx, gy).
     */
    virtual void spawnItemForFormType(int gx, int gy, int formType) = 0;

    /**
     * Start (or refresh) the 3.5 s MULTI_COIN countdown for tile (gx, gy).
     * When the countdown expires, MapManager converts the tile to QUESTION_USED.
     */
    virtual void setMultiCoinActive(int gx, int gy) = 0;

    /**
     * Stomp-kill all enemies whose hitbox overlaps the tile row directly
     * above (gy - 1). Called when a brick is broken by a Super/Fire player.
     */
    virtual void killEnemiesAboveTile(int gx, int gy) = 0;
};
