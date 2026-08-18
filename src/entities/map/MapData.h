#pragma once

#include <string>
#include <vector>

/**
 * @enum TileType
 * @brief Logical tile classification used for gameplay logic.
 * Values map 1:1 to the `type` string property set on tiles in Tiled.
 */
enum class TileType {
    EMPTY = 0,
    GROUND = 1,
    PIPE = 2,
    BRICK_NORMAL = 3,
    QUESTION_COIN = 4,
    QUESTION_POWERUP = 5,
    MULTI_COIN = 6,
    HIDDEN_BLOCK = 7,
    DEATH_ZONE = 8,
    FLAGPOLE = 9,
    COIN = 10,
    SOLID_BRICK = 11,
    BACKGROUND = 12  // trang trí (bụi cỏ, đám mây) — xử lý giống hệt EMPTY
};

/**
 * @struct EntitySpawnData
 * @brief Describes one entity spawn point parsed from a TMX <objectgroup>.
 *
 * OOP: Pure data struct — SRP (no logic, just data contract).
 */
struct EntitySpawnData {
    std::string type;        // "Goomba", "KoopaTroopa", "BuzzyBeetle", etc.
    float x = 0.f;           // world pixel X
    float y = 0.f;           // world pixel Y
    // Optional properties parsed from <properties> on the object
    std::string direction = "left";   // "left" or "right"
    float moveSpeed = -1.f;           // -1 means use enemy's default
};

/**
 * @struct PlayerSpawnData
 * @brief Describes the player's spawn point parsed from the TMX map.
 */
struct PlayerSpawnData {
    float x = 32.f;          // default fallback position
    float y = 100.f;
    bool found = false;      // true if the map had a PlayerSpawn object
};

/**
 * @struct MapObjectData
 * @brief Aggregated entity data parsed from TMX <objectgroup> layers.
 *
 * OOP: DIP — consumers depend on this data contract, not on MapManager internals.
 */
struct MapObjectData {
    std::vector<EntitySpawnData> enemySpawns;
    PlayerSpawnData playerSpawn;
    // Chỉ có Star được spawn dưới dạng item object (type/name "Star" trong Tiled).
    std::vector<EntitySpawnData> itemSpawns;
};
