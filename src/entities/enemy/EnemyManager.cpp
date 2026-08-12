#include "EnemyManager.h"
#include "enemies/Goomba.h"
#include "../../interfaces/IPlayerManager.h"
#include <SFML/Graphics.hpp>

EnemyManager::EnemyManager() = default;

void EnemyManager::initialize() {
    m_enemies.clear();
    if (!m_goombaTextures[0].loadFromFile("assets/texture/enemy/Goomba1.PNG")) {
        throw std::runtime_error("Failed to load Goomba1.PNG");
    }

    if (!m_goombaTextures[1].loadFromFile("assets/texture/enemy/Goomba2.PNG")) {
        throw std::runtime_error("Failed to load Goomba2.PNG");
    }

    if (!m_goombaDeadTexture.loadFromFile("assets/texture/enemy/GoombaDead.PNG")) {
        throw std::runtime_error("Failed to load GoombaDead.PNG");
    }
    std::array<sf::Texture*, 2> goombaFrames{&m_goombaTextures[0], &m_goombaTextures[1]};
    m_enemies.push_back(std::make_unique<Goomba>(300.f, 100.f, goombaFrames, m_goombaDeadTexture));
}

void EnemyManager::update(float deltaTime) {
    for (auto& enemy : m_enemies) {
        if (enemy->isDead())
            continue;

        enemy->update(deltaTime);
        //gravity
        enemy->applyGravity(deltaTime);

        if (!m_mapManager)
        {
            enemy->move(deltaTime);
            continue;
        }
        //horizontal collision
        sf::FloatRect hitbox = enemy->getHitbox();

        float checkX;

        if (enemy->getDirection() < 0)
        {
            // Enemy đang đi LEFT
            checkX = hitbox.position.x - 1.f;
        }
        else
        {
            // Enemy đang đi RIGHT
            checkX =
                hitbox.position.x +
                hitbox.size.x +
                1.f;
        }

        // Check 2 điểm ở phía trước enemy
        float checkTop =
            hitbox.position.y + 2.f;

        float checkBottom =
            hitbox.position.y +
            hitbox.size.y - 2.f;

        bool hitWall =
            m_mapManager->isSolid(checkX, checkTop) ||
            m_mapManager->isSolid(checkX, checkBottom);

        if (hitWall)
        {
            enemy->reverseDirection();
        }

        enemy->move(deltaTime);
        //ground collision
        sf::FloatRect newHitbox = enemy->getHitbox();

        float feetY =
            newHitbox.position.y +
            newHitbox.size.y;

        float checkGroundY =
            feetY + 1.f;

        // Check cả chân trái và chân phải
        float leftFoot =
            newHitbox.position.x + 2.f;

        float rightFoot =
            newHitbox.position.x +
            newHitbox.size.x - 2.f;

        bool onGround =
            m_mapManager->isSolid(leftFoot, checkGroundY) ||
            m_mapManager->isSolid(rightFoot, checkGroundY);
        // resolve ground collision
        if (onGround && enemy->getVelocity().y >= 0.f)
        {
            int tileSize =
                m_mapManager->getTileSize();

            // Tile mà chân enemy đang chạm vào
            float groundY =
                static_cast<float>(
                    static_cast<int>(checkGroundY / tileSize)
                    * tileSize
                );

            // Đặt enemy chính xác lên trên tile
            enemy->setPositionY(groundY - newHitbox.size.y);

            // Stop falling
            enemy->setVelocityY(0.f);
        }

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
