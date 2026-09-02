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
    float dt = std::min(deltaTime, 0.033f);

    for (auto& enemy : m_enemies) {
        if (enemy->isDead())
            continue;

        enemy->update(dt);

        // Physics: Wall & Ground Collision
        if (enemy->usesPhysics()) {
            float tileSize = m_mapManager ? static_cast<float>(m_mapManager->getTileSize()) : 16.f;

            // 1. Horizontal movement & Wall Collision
            sf::FloatRect hitbox = enemy->getHitbox();
            float curX = hitbox.position.x;
            float curY = hitbox.position.y;
            float width = hitbox.size.x;
            float height = hitbox.size.y;

            if (m_mapManager) {
                float checkX = (enemy->getDirection() < 0) ? (curX - 1.f) : (curX + width + 1.f);
                // Probe strictly above the feet: from top + 2px down to bottom - 4px
                // to ensure we never mistake the floor tile beneath the enemy for a wall.
                float checkTop = curY + 2.f;
                float checkBottom = curY + height - 4.f;
                if (checkBottom < checkTop) checkBottom = checkTop;

                bool hitWall = m_mapManager->isSolid(checkX, checkTop) ||
                               m_mapManager->isSolid(checkX, checkBottom);

                if (hitWall) {
                    enemy->reverseDirection();
                }
            }

            enemy->move(dt);

            // 2. Vertical movement (Gravity) & Ground Collision
            enemy->applyGravity(dt);

            hitbox = enemy->getHitbox();
            curX = hitbox.position.x;
            curY = hitbox.position.y;
            width = hitbox.size.x;
            height = hitbox.size.y;

            if (m_mapManager) {
                float vy = enemy->getVelocity().y;
                if (vy >= 0.f) {
                    float leftFoot = curX + 2.f;
                    float rightFoot = curX + width - 2.f;

                    int gy_start = static_cast<int>((curY + height - 6.f) / tileSize);
                    if (gy_start < 0) gy_start = 0;
                    int gy_end = static_cast<int>((curY + height + 2.f) / tileSize);

                    for (int gy = gy_start; gy <= gy_end; ++gy) {
                        float probeY = static_cast<float>(gy) * tileSize + 1.f;
                        if (m_mapManager->isSolid(leftFoot, probeY) || m_mapManager->isSolid(rightFoot, probeY)) {
                            float topOfTile = static_cast<float>(gy) * tileSize;
                            enemy->setPositionY(topOfTile - height);
                            enemy->setVelocityY(0.f);
                            break;
                        }
                    }
                }
            }
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
    m_pendingSpawns.clear();

    if (!m_enemyFactory) {
        m_enemyFactory = std::make_unique<EnemyFactory>();
    }

    float tileSize = m_mapManager ? static_cast<float>(m_mapManager->getTileSize()) : 16.f;

    for (const auto& spawnData : spawns) {
        auto enemy = m_enemyFactory->createEnemy(
            spawnData,
            [this](float fx, float fy, int dir) {
                m_pendingSpawns.push_back(std::make_unique<BossFireball>(fx, fy, dir));
            }
        );

        if (enemy) {
            // Align spawned enemy perfectly to the ground beneath or at its spawn point
            if (m_mapManager) {
                float height = enemy->getHitbox().size.y;
                float width = enemy->getHitbox().size.x;
                float spawnX = spawnData.x;
                float spawnY = spawnData.y;

                int gy_min = static_cast<int>(spawnY / tileSize);
                if (gy_min < 0) gy_min = 0;
                int gy_max = static_cast<int>((spawnY + height + tileSize * 2.f) / tileSize);

                for (int gy = gy_min; gy <= gy_max; ++gy) {
                    float probeY = static_cast<float>(gy) * tileSize + 1.f;
                    float leftFoot = spawnX + 2.f;
                    float rightFoot = spawnX + width - 2.f;
                    if (m_mapManager->isSolid(leftFoot, probeY) || m_mapManager->isSolid(rightFoot, probeY)) {
                        float topOfTile = static_cast<float>(gy) * tileSize;
                        if (spawnY + height >= topOfTile - tileSize && spawnY <= topOfTile + 4.f) {
                            enemy->setPositionY(topOfTile - height);
                            enemy->setVelocityY(0.f);
                            break;
                        }
                    }
                }
            }

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
            if (m_soundManager)
                m_soundManager->playStomp();
        }
    }
}