#include "EnemyManager.h"
#include "enemies/Goomba.h"
#include "../../interfaces/IPlayerManager.h"
#include <SFML/Graphics.hpp>

EnemyManager::EnemyManager() = default;

void EnemyManager::initialize() {
    m_enemies.clear();
    if (!m_goombaTexture.loadFromFile(EnemySprite::Goomba1))
    {
        throw std::runtime_error(
            "Failed to load Goomba1.PNG"
        );
    }
    m_enemies.push_back(
        std::make_unique<Goomba>(
            200.f,
            160.f,
            m_goombaTexture
        )
    );
}

void EnemyManager::update(float deltaTime) {
    for (auto& enemy : m_enemies) {
        if (enemy->isDead())
            continue;

        enemy->update(deltaTime);

        if (m_player && enemy->getHitbox().findIntersection(m_player->getHitbox())) {
            enemy->onPlayerCollision(m_player);
        }
    }
}

void EnemyManager::render(sf::RenderWindow& window) const {
    for (const auto& enemy : m_enemies) {
        enemy->render(window);
    }
    
}

int EnemyManager::getEnemyCount() const
{
    return static_cast<int>(m_enemies.size());
}
