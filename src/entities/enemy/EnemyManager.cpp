#include "EnemyManager.h"
#include "enemies/Goomba.h"
#include "enemies/BuzzyBeetle.h"
#include "enemies/KoopaTroopa.h"
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

    static constexpr std::array<const char*, 5> buzzyBeetlePaths{
        "assets/texture/enemy/BuzzyBeetle1.PNG",
        "assets/texture/enemy/BuzzyBeetle2.PNG",
        "assets/texture/enemy/BuzzyBeetle3.PNG",
        "assets/texture/enemy/BuzzyBeetle4.PNG",
        "assets/texture/enemy/BuzzyBeetle5.PNG"
    };
    for (size_t i = 0; i < buzzyBeetlePaths.size(); ++i)
    {
        if (!m_buzzyBeetleTextures[i].loadFromFile(buzzyBeetlePaths[i]))
            throw std::runtime_error(std::string("Failed to load ") + buzzyBeetlePaths[i]);
    }

    std::array<sf::Texture*, 2> buzzyLeftFrames{&m_buzzyBeetleTextures[0], &m_buzzyBeetleTextures[1]};
    std::array<sf::Texture*, 2> buzzyRightFrames{&m_buzzyBeetleTextures[3], &m_buzzyBeetleTextures[4]};
    m_enemies.push_back(std::make_unique<BuzzyBeetle>(
        400.f, 100.f, buzzyLeftFrames, buzzyRightFrames, m_buzzyBeetleTextures[2]));

    static constexpr std::array<const char*, 6> koopaTroopaPaths{
        "assets/texture/enemy/Koopa1.PNG",
        "assets/texture/enemy/Koopa2.PNG",
        "assets/texture/enemy/Koopa3.PNG",
        "assets/texture/enemy/Koopa4.PNG",
        "assets/texture/enemy/KoopaShell1.PNG",
        "assets/texture/enemy/KoopaShell2.PNG"
    };
    for (size_t i = 0; i < koopaTroopaPaths.size(); ++i)
    {
        if (!m_koopaTroopaTextures[i].loadFromFile(koopaTroopaPaths[i]))
            throw std::runtime_error(std::string("Failed to load ") + koopaTroopaPaths[i]);
    }

    std::array<sf::Texture*, 2> koopaLeftFrames{&m_koopaTroopaTextures[0], &m_koopaTroopaTextures[1]};
    std::array<sf::Texture*, 2> koopaRightFrames{&m_koopaTroopaTextures[2], &m_koopaTroopaTextures[3]};
    std::array<sf::Texture*, 2> koopaShellFrames{&m_koopaTroopaTextures[4], &m_koopaTroopaTextures[5]};
    m_enemies.push_back(std::make_unique<KoopaTroopa>(
        500.f, 100.f, koopaLeftFrames, koopaRightFrames, koopaShellFrames));
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

        if (m_player)
            resolvePlayerCollision(*enemy, m_player, 0);

        if (m_player2)
            resolvePlayerCollision(*enemy, m_player2, 1);
    }
}

void EnemyManager::resolvePlayerCollision(Enemy& enemy, IPlayerManager* player, int playerIndex)
{
    sf::FloatRect playerBox = player->getHitbox();
    sf::FloatRect enemyBox = enemy.getHitbox();

    bool isOverlapping = playerBox.findIntersection(enemyBox).has_value();
    bool wasOverlapping = enemy.wasPlayerOverlapping(playerIndex);

    if (isOverlapping && !wasOverlapping)
    {
        float playerBottom = playerBox.position.y + playerBox.size.y;
        float enemyTop = enemyBox.position.y;

        // Stomp: player's feet land in the top slice of the enemy's hitbox.
        // Tweak the 0.5f fraction if stomps feel too easy/hard to land.
        float stompZone = enemyBox.size.y * 0.5f;
        bool isStomp = playerBottom <= enemyTop + stompZone;

        if (isStomp)
            enemy.onStomp();
        else
            enemy.onPlayerCollision(player);
    }

    enemy.setPlayerOverlapping(playerIndex, isOverlapping);
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