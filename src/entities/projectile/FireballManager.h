#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Fireball.h"
#include "../../interfaces/IMapManager.h"
#include "../../interfaces/IEnemyManager.h"

/**
 * @class FireballManager
 * @brief Sở hữu texture và quản lý vòng đời cầu lửa.
 *
 * - Quản lý trực tiếp std::vector<Fireball> (by value), tránh heap allocation
 *   thừa và giảm indirection.
 * - Phụ thuộc interface IMapManager & IEnemyManager (DIP), inject qua
 *   setMapManager() / setEnemyManager().
 */
class FireballManager
{
    std::vector<Fireball> m_fireballs;        // by value, không dùng pointer
    sf::Texture           m_fireballTexture;  // owns texture, truyền ref cho Fireball
    IMapManager*          m_mapManager   = nullptr;
    IEnemyManager*        m_enemyManager = nullptr;

public:
    void initialize();   // load texture từ FireballSprite::Sheet
    void update(float dt);
    void render(sf::RenderWindow& window) const;

    void spawnFireball(float x, float y, int direction /* -1 hoặc +1 */);
    void clear() { m_fireballs.clear(); }

    void setMapManager(IMapManager* map)        { m_mapManager   = map; }
    void setEnemyManager(IEnemyManager* enemies) { m_enemyManager = enemies; }

    int getActiveCount() const { return static_cast<int>(m_fireballs.size()); }
};