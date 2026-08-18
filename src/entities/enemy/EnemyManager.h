#pragma once

#include "../../interfaces/IEnemyManager.h"
#include "../../interfaces/IMapManager.h"
#include "Enemy.h"
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

    std::array<sf::Texture, 2> m_goombaTextures;
    sf::Texture m_goombaDeadTexture;

    std::array<sf::Texture, 5> m_buzzyBeetleTextures; // 1,2 = left; 3 = shell; 4,5 = right
    std::array<sf::Texture, 6> m_koopaTroopaTextures; // 1,2 = left; 3,4 = right; shell1,2 = shell
    void resolvePlayerCollision(Enemy& enemy, IPlayerManager* player, int playerIndex);
public:
    EnemyManager();
    ~EnemyManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getEnemyCount() const override;

    void setPlayerManager(IPlayerManager* player) override { m_player = player; }

    void setPlayerManager2(IPlayerManager* player) override { m_player2 = player; }

    void setMapManager(IMapManager* map) override { m_mapManager = map; }

    bool takeDamageFromFireball(const sf::FloatRect& fireballHitbox) override;

    void spawnFromMapData(const std::vector<EntitySpawnData>& spawns) override;
};