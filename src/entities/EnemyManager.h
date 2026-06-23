#pragma once

#include "../interfaces/IEnemyManager.h"

namespace sf {
class RenderWindow;
}

/**
 * @class EnemyManager
 * @brief Stub implementation of enemy management
 * 
 * Developer: Dinh Anh
 * Status: STUB - Replace with full implementation
 * 
 * This stub provides minimal functionality to keep the project compilable.
 * Dinh Anh will implement full enemy AI and spawning logic here.
 */
class EnemyManager : public IEnemyManager
{
public:
    EnemyManager();
    ~EnemyManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getEnemyCount() const override;

private:
    int m_enemyCount;
};
