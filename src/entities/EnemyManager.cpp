#include "EnemyManager.h"
#include <SFML/Graphics.hpp>

EnemyManager::EnemyManager()
    : m_enemyCount(0)
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
    // TODO: Dinh Anh - Update all enemies
    // - Apply AI logic
    // - Move enemies
    // - Check boundaries
    // - Handle spawning/despawning
}

void EnemyManager::render(sf::RenderWindow& window) const
{
    // TODO: Dinh Anh - Render all enemies
    // For now, draw placeholder rectangles
    for (int i = 0; i < m_enemyCount; ++i)
    {
        sf::RectangleShape enemy({ 32.0f, 32.0f });
        enemy.setFillColor(sf::Color::Yellow);
        enemy.setPosition({ 100.0f + i * 50.0f, 300.0f });
        window.draw(enemy);
    }
}

int EnemyManager::getEnemyCount() const
{
    return m_enemyCount;
}
