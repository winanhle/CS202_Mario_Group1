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
    BACKGROUND = 12,  // trang trí (bụi cỏ, đám mây) — xử lý giống hệt EMPTY
    FIRE_BAR = 13
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
 * @struct LiftSpawnData
 * @brief Describes one Lift platform spawn point parsed from a TMX <objectgroup>.
 *
 * Lift objects in Tiled use name/type = "Lift" with these custom properties:
 *   motionType = "updown" | "leftright"
 *   holes      = 4 | 6
 *   range      = float  (pixels of oscillation half-amplitude, default 80)
 *   speed      = float  (pixels per second of oscillation, default 60)
 */
struct LiftSpawnData {
    std::string motionType = "updown"; // "updown" or "leftright"
    int         holes      = 4;        // 4 or 6 tile-holes wide
    float       x          = 0.f;
    float       y          = 0.f;
    float       range      = 80.f;     // half-amplitude of oscillation in pixels
    float       speed      = 60.f;     // pixels per second (converted to angular vel)
};

/**
 * @struct FireBarSpawnData
 * @brief Describes one FireBar rotating obstacle spawn point.
 */
struct FireBarSpawnData {
    float x = 0.f;            // Center X in world pixels
    float y = 0.f;            // Center Y in world pixels
    int fireCount = 6;        // Number of fireballs (default 6)
    float speed = 2.0f;       // Angular velocity (rad/s)
    bool clockwise = true;    // Rotation direction (true = clockwise, false = counter-clockwise)
    float initialAngle = 0.f; // Starting angle in radians
};

/**
 * @struct MapObjectData
 * @brief Aggregated entity data parsed from TMX <objectgroup> layers.
 *
 * OOP: DIP — consumers depend on this data contract, not on MapManager internals.
 */
struct MapObjectData {
    std::vector<EntitySpawnData>  enemySpawns;
    PlayerSpawnData               playerSpawn;
    // Chỉ có Star được spawn dưới dạng item object (type/name "Star" trong Tiled).
    std::vector<EntitySpawnData>  itemSpawns;
    std::vector<LiftSpawnData>    liftSpawns;
    std::vector<FireBarSpawnData> fireBarSpawns;
};
