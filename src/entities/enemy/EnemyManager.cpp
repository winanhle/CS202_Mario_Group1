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

        if (m_mapManager)
        {
            sf::FloatRect hitbox = enemy->getHitbox();
            float checkX;
            if (enemy->getDirection() < 0)
            {
                checkX = hitbox.position.x - 1.f;
            }
            else
            {
                checkX = hitbox.position.x
                       + hitbox.size.x
                       + 1.f;
            }

            float checkTop =
                hitbox.position.y + 2.f;

            float checkBottom =
                hitbox.position.y
                + hitbox.size.y
                - 2.f;

            bool hitWall =
                m_mapManager->isSolid(checkX, checkTop) ||
                m_mapManager->isSolid(checkX, checkBottom);

            if (hitWall)
            {
                enemy->reverseDirection();
            }
            else
            {
                enemy->move(deltaTime);
            }
        }
        else enemy->move(deltaTime);
    

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

bool EnemyManager::takeDamageFromFireball(const sf::FloatRect& fireballHitbox)
{
    for (auto& enemy : m_enemies)
    {
        if (enemy->isDead())
            continue;

        if (enemy->getHitbox().findIntersection(fireballHitbox))
        {
            enemy->onStomp(); // bị cầu lửa tiêu diệt (giống bị stomp)
            return true;
        }
    }
    return false;
}
