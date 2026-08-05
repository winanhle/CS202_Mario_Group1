#pragma once

#include "../../interfaces/IEnemyManager.h"
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
};
