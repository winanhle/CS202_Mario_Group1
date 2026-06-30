#include "EnemyManager.h"
#include "../interfaces/IPlayerManager.h"
#include <SFML/Graphics.hpp>

EnemyManager::EnemyManager()
{
}

void EnemyManager::initialize()
{
    // TODO: Dinh Anh - Initialize enemy manager
    // - Load enemy sprites
    // - Spawn initial enemies
    // - Set up AI parameters
}

void EnemyManager::update(float deltaTime)
{
    if (!m_player) return;
    sf::FloatRect playerBox = m_player->getHitbox();

    for (auto& enemy : m_enemies) {
        if (!enemy.alive) continue;

        enemy.x += enemy.vx * deltaTime;
        enemy.y += enemy.vy * deltaTime;

        if (auto overlap = playerBox.findIntersection(enemy.getHitbox())) {
            float playerBottom = playerBox.position.y + playerBox.size.y;
            float enemyMidY    = enemy.y + enemy.getHitbox().size.y / 2.f;

            if (playerBottom < enemyMidY) {
                enemy.die();
                m_player->bounce();
            } else {
                m_player->takeDamage();
            }
        }
    }
}

void EnemyManager::render(sf::RenderWindow& window) const
{
    // TODO: Dinh Anh - Render all enemies
    // For now, draw placeholder rectangles
    for (const auto& enemy : m_enemies) {
        if (!enemy.alive) continue;
        sf::RectangleShape shape({16.f, 16.f});
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition({enemy.x, enemy.y});
        window.draw(shape);
    }
}

int EnemyManager::getEnemyCount() const
{
    return static_cast<int>(m_enemies.size());
}
