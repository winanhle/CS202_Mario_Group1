#pragma once

#include "../../interfaces/IEnemyManager.h"
#include "../../interfaces/IMapManager.h"
#include "Enemy.h"
#include "EnemyFactory.h"
#include "../map/MapData.h"
#include <SFML/Graphics/Rect.hpp>
#include <array>
#include <memory>
#include <vector>

class IPlayerManager;

namespace sf {
class RenderWindow;
}

class EnemyManager : public IEnemyManager
{
    IPlayerManager* m_player = nullptr;
    IPlayerManager* m_player2 = nullptr;
    IMapManager* m_mapManager = nullptr;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::unique_ptr<IEnemyFactory> m_enemyFactory;

    std::vector<std::unique_ptr<Enemy>> m_pendingSpawns;

    void resolvePlayerCollision(Enemy& enemy, IPlayerManager* player, int playerIndex);
public:
    EnemyManager();
    explicit EnemyManager(std::unique_ptr<IEnemyFactory> factory);
    ~EnemyManager() override = default;

    void setEnemyFactory(std::unique_ptr<IEnemyFactory> factory) {
        m_enemyFactory = std::move(factory);
    }

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getEnemyCount() const override;

    void setPlayerManager(IPlayerManager* player) override { m_player = player; }

    void setPlayerManager2(IPlayerManager* player) override { m_player2 = player; }

    void setMapManager(IMapManager* map) override { m_mapManager = map; }

    bool takeDamageFromFireball(const sf::FloatRect& fireballHitbox) override;

    void spawnFromMapData(const std::vector<EntitySpawnData>& spawns) override;

    /**
     * @brief Stomp-kill all enemies whose hitbox overlaps the tile row
     *        directly above (gy - 1). Called when a brick breaks.
     */
    void killEnemiesAboveTile(int gx, int gy) override;
};