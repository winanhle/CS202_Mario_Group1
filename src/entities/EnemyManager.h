#pragma once

#include "../interfaces/IEnemyManager.h"
#include <SFML/Graphics/Rect.hpp>
#include <vector>

class IPlayerManager;

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
    IPlayerManager* m_player = nullptr;

    struct Enemy {
        float x, y;
        float vx, vy;
        bool alive = true;
        sf::FloatRect getHitbox() const { return {{x, y}, {16.f, 16.f}}; }
        void die() { alive = false; }
    };
    std::vector<Enemy> m_enemies;

public:
    EnemyManager();
    ~EnemyManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getEnemyCount() const override;

    // ─── NHẬN DEPENDENCY ───
    void setPlayerManager(IPlayerManager* player) override { m_player = player; }
};
