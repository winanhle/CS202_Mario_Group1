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

    /**
     * @brief Spawn a vertical bounce animation for block at (gx, gy)
     *        moving up by half a tile (8px) and returning, while setting finalType.
     */
    virtual void spawnBlockBump(int gx, int gy, TileType finalType) = 0;

    /**
     * @brief Yêu cầu chuyển map (warp) đến mapPath tại tọa độ đích (targetX, targetY).
     * @param targetMap Đường dẫn map đích (ví dụ "assets/map/stage1_hidden.tmx").
     * @param targetX Tọa độ X đích (-1 = lấy PlayerSpawn từ map đích).
     * @param targetY Tọa độ Y đích (-1 = lấy PlayerSpawn từ map đích).
     */
    virtual void requestWarp(const std::string& targetMap, float targetX = -1.f, float targetY = -1.f) = 0;
};
