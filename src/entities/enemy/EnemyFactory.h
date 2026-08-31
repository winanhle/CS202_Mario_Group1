#pragma once

#include "Enemy.h"
#include "../map/MapData.h"
#include <functional>
#include <memory>
#include <string>

/**
 * @class IEnemyFactory
 * @brief Abstract Factory Interface for creating Enemy entities.
 *
 * Adheres to OOP principles:
 * - SRP: Isolates enemy creation logic from lifecycle/physics management in EnemyManager.
 * - OCP: Allows extension of enemy creation without modifying EnemyManager.
 * - DIP: High-level modules depend on this abstraction rather than concrete Enemy subclasses.
 */
class IEnemyFactory {
public:
    using FireballSpawnCallback = std::function<void(float, float, int)>;

    virtual ~IEnemyFactory() = default;

    /**
     * @brief Preloads all enemy textures into memory to avoid frame hitches.
     */
    virtual void preloadTextures() = 0;

    /**
     * @brief Creates a concrete Enemy instance based on EntitySpawnData.
     * @param spawnData The spawn configuration parsed from map data.
     * @param onSpawnFireball Optional callback for enemies that spawn projectiles (e.g. Boss).
     * @return std::unique_ptr<Enemy> or nullptr if enemy type is unknown.
     */
    virtual std::unique_ptr<Enemy> createEnemy(
        const EntitySpawnData& spawnData,
        FireballSpawnCallback onSpawnFireball = nullptr
    ) = 0;
};

/**
 * @class EnemyFactory
 * @brief Default factory implementation supporting Goomba, KoopaTroopa, BuzzyBeetle, Boss, etc.
 */
class EnemyFactory : public IEnemyFactory {
public:
    EnemyFactory() = default;
    ~EnemyFactory() override = default;

    void preloadTextures() override;

    std::unique_ptr<Enemy> createEnemy(
        const EntitySpawnData& spawnData,
        FireballSpawnCallback onSpawnFireball = nullptr
    ) override;
};
