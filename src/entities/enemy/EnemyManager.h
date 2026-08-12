#pragma once

#include "../../interfaces/IEnemyManager.h"
#include "../../interfaces/IMapManager.h"
#include "Enemy.h"
#include <SFML/Graphics/Rect.hpp>
#include <vector>

class IPlayerManager;

namespace sf {
class RenderWindow;
}

class EnemyManager : public IEnemyManager
{
    IPlayerManager* m_player = nullptr;
    IMapManager* m_mapManager = nullptr;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    sf::Texture m_goombaTexture;
public:
    EnemyManager();
    ~EnemyManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getEnemyCount() const override;

    void setPlayerManager(IPlayerManager* player) override { m_player = player; }

    void setMapManager(IMapManager* map) override { m_mapManager = map; }

    bool takeDamageFromFireball(const sf::FloatRect& fireballHitbox) override;
};
