#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include "../entities/map/MapData.h"

class IItemManager;   // forward declaration
class IEnemyManager;  // forward declaration
class IPlayerManager;

namespace sf {
class RenderWindow;
class Event;
}

class IMapManager {
public:
    virtual ~IMapManager() = default;
    virtual void initialize() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) const = 0;

    virtual bool isSolid(float x, float y) const = 0;

    /**
     * @brief Direction-aware solidity cho trường hợp player đập tile từ dưới lên.
     * Mọi block thường trả cùng kết quả với isSolid(); riêng HIDDEN_BLOCK trả true
     * (bump được) dù isSolid() là false (Mario xuyên qua từ trái/phải/trên).
     */
    virtual bool isSolidFromBelow(float x, float y) const = 0;

    virtual int getTileSize() const = 0;
    virtual sf::Vector2u getMapPixelSize() const = 0;

    /**
     * @brief Query the logical tile type at a world pixel coordinate.
     * Out-of-bounds pixels are treated as GROUND.
     */
    virtual TileType getTileType(float x, float y) const = 0;

    /**
     * @brief Query the logical tile type at grid coordinates (for editor use).
     * Out-of-bounds grid coords are treated as GROUND.
     */
    virtual TileType getTileTypeAt(int gx, int gy) const = 0;

    /**
     * @brief Called by PlayerManager when the player bumps a tile from below
     *        (upward velocity collides with the bottom face of a tile).
     * @param tileGridX  Grid column of the hit tile
     * @param tileGridY  Grid row of the hit tile
     * @param player     Current player instance
     */
    virtual void onHitFromBelow(int tileGridX, int tileGridY, IPlayerManager* player) = 0;

    /**
     * @brief Inject ItemManager so MapManager can spawn coins/power-ups.
     * Called by GameWorld::injectDependencies().
     */
    virtual void setItemManager(IItemManager* itemManager) = 0;

    /**
     * @brief Inject EnemyManager so MapManager can kill enemies above broken bricks.
     * Called by GameWorld::injectDependencies().
     */
    virtual void setEnemyManager(IEnemyManager* enemyManager) = 0;

    /**
     * @brief Load a map from a TMX file path.
     * Replaces any previously loaded map data.
     * @param tmxPath Path to the .tmx file
     */
    virtual void loadMap(const std::string& tmxPath) = 0;

    /**
     * @brief Get parsed object layer data (enemy spawns, player spawn)
     * from the last loaded map.
     * @return Reference to the parsed MapObjectData
     */
    virtual const MapObjectData& getMapObjectData() const = 0;

    /**
     * @brief Kích hoạt animation cờ trượt xuống cột khi người chơi chạm cột cờ.
     */
    virtual void triggerFlagSlide(int poleGridX) = 0;
    virtual bool isFlagSliding() const = 0;
    virtual bool hasFlagSlideFinished() const = 0;

    // ── Map Editor API ──────────────────────────────────────────────────────

    /**
     * @brief Set tile at grid coordinates and record an undo step.
     *        Use this for all editor operations (not game-logic tile changes).
     */
    virtual void editTile(int gx, int gy, TileType newType) = 0;

    /** Undo the last editTile() call. Returns false if the undo stack is empty. */
    virtual bool undoEdit() = 0;

    /** Redo the last undone editTile() call. Returns false if stack is empty. */
    virtual bool redoEdit() = 0;

    /**
     * @brief Serialize the current tile grid + object data back to TMX (CSV encoding).
     * @param path  Output .tmx file path.
     * @return true on success.
     */
    virtual bool saveToTMX(const std::string& path) const = 0;

    /** Append an enemy spawn entry (editor use). */
    virtual void addEnemySpawn(const EntitySpawnData& spawn) = 0;

    /** Remove enemy spawn entry by index (editor use). */
    virtual void removeEnemySpawn(int index) = 0;
};