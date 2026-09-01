#include "EnemyManager.h"
#include "EnemyFactory.h"
#include "enemies/BossFireBall.h"
#include "../../interfaces/IPlayerManager.h"
#include "../../interfaces/ISoundManager.h"
#include <iostream>
#include <SFML/Graphics.hpp>

EnemyManager::EnemyManager()
    : m_enemyFactory(std::make_unique<EnemyFactory>())
{
}

EnemyManager::EnemyManager(std::unique_ptr<IEnemyFactory> factory)
    : m_enemyFactory(std::move(factory))
{
    if (!m_enemyFactory) {
        m_enemyFactory = std::make_unique<EnemyFactory>();
    }
}

void EnemyManager::initialize() {
    m_enemies.clear();
    m_pendingSpawns.clear();

    if (m_enemyFactory) {
        m_enemyFactory->preloadTextures();
    }
}

void EnemyManager::update(float deltaTime) {
    for (auto& enemy : m_enemies) {
        if (enemy->isDead())
            continue;

        enemy->update(deltaTime);
        //gravity
        if (enemy->usesPhysics()) {
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
        }
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

        if (m_player)
            resolvePlayerCollision(*enemy, m_player, 0);

        if (m_player2)
            resolvePlayerCollision(*enemy, m_player2, 1);
    }

    if (!m_pendingSpawns.empty())
    {
        for (auto& spawned : m_pendingSpawns)
            m_enemies.push_back(std::move(spawned));
        m_pendingSpawns.clear();
    }
}

void EnemyManager::resolvePlayerCollision(Enemy& enemy, IPlayerManager* player, int playerIndex)
{
    sf::FloatRect playerBox = player->getHitbox();
    sf::FloatRect enemyBox = enemy.getHitbox();

    bool isOverlapping = playerBox.findIntersection(enemyBox).has_value();
    bool wasOverlapping = enemy.wasPlayerOverlapping(playerIndex);

    if (isOverlapping)
    {
        float playerBottom = playerBox.position.y + playerBox.size.y;
        float enemyTop = enemyBox.position.y;

        // Stomp: player's feet land in the top slice of the enemy's hitbox.
        float stompZone = enemyBox.size.y * 0.5f;
        bool inStompZone = playerBottom <= enemyTop + stompZone;

        // Enemy rất thấp (Goomba/BuzzyBeetle 16px). Khi player rơi nhanh, frame
        // đầu tiên chân có thể đã xuyên sâu hơn nửa thân → vẫn phải tính stomp,
        // miễn là đang rơi xuống và chân chưa xuống quá đáy enemy.
        bool fallingOnto = player->getVelocityY() > 0.f &&
                           playerBottom <= enemyTop + enemyBox.size.y;

        bool isStomp = (inStompZone || fallingOnto) || player->isStarActive();

        if (isStomp && !wasOverlapping)
        {
            enemy.onStomp();
            player->addScore(100);
            player->bounce();
            if (m_soundManager)
                m_soundManager->playStomp();
        }
        else if (!wasOverlapping || !player->isInvincible())
        {
            // Đang i-frames → takeDamage() bên trong player là no-op, nhưng KHÔNG
            // đánh dấu "đã va chạm": hết i-frames mà vẫn chạm enemy thì sát thương
            // vẫn được áp (Mario không được "xuyên thủng" Goomba một cách tự do).
            enemy.onPlayerCollision(player);
        }
    }

    // Trong lúc i-frames, cố tình để cờ overlap = false (xem dòng trên).
    enemy.setPlayerOverlapping(playerIndex, isOverlapping && !player->isInvincible());
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

bool EnemyManager::takeDamageFromFireball(const sf::FloatRect& fireballHitbox, IPlayerManager* owner)
{
    for (auto& enemy : m_enemies)
    {
        if (enemy->isDead())
            continue;

        if (enemy->getHitbox().findIntersection(fireballHitbox))
        {
            enemy->onStomp();
            if (owner) owner->addScore(100);
            if (m_soundManager)
                m_soundManager->playStomp();
            return true;
        }
    }
    return false;
}

void EnemyManager::spawnFromMapData(const std::vector<EntitySpawnData>& spawns) {
    m_enemies.clear();

    if (!m_enemyFactory) {
        m_enemyFactory = std::make_unique<EnemyFactory>();
    }

    for (const auto& spawnData : spawns) {
        auto enemy = m_enemyFactory->createEnemy(
            spawnData,
            [this](float fx, float fy, int dir) {
                m_pendingSpawns.push_back(std::make_unique<BossFireball>(fx, fy, dir));
            }
        );

        if (enemy) {
            m_enemies.push_back(std::move(enemy));
        }
    }

    std::cout << "[EnemyManager] Spawned " << m_enemies.size()
              << " enemies from map data using EnemyFactory." << std::endl;
}

void EnemyManager::killEnemiesAboveTile(int gx, int gy)
{
    if (!m_mapManager) return;

    // World bounds of the tile directly above the broken/bumped block (gy - 1).
    // We use a full-tile rectangle so an enemy standing partly on the edge is
    // still caught.
    const float ts   = static_cast<float>(m_mapManager->getTileSize());
    const float left = static_cast<float>(gx) * ts;
    const float top  = static_cast<float>(gy - 1) * ts;
    sf::FloatRect aboveRect({ left, top }, { ts, ts });

    for (auto& enemy : m_enemies) {
        if (enemy->isDead()) continue;
        if (enemy->getHitbox().findIntersection(aboveRect).has_value()) {
            enemy->onStomp(); // treat like a stomp kill (plays death anim, removes enemy)
        }
    }
}