#include "FireBarManager.h"
#include <iostream>

void FireBarManager::initialize()
{
    m_firebars.clear();

    if (!m_fireballTexture.loadFromFile(FireballSprite::Sheet))
    {
        std::cerr << "[FireBarManager] Failed to load fireball sprite sheet: "
                  << FireballSprite::Sheet << std::endl;
    }
    else
    {
        m_fireballTexture.setSmooth(false);
        std::cout << "[FireBarManager] Fireball sprite sheet loaded." << std::endl;
    }
}

void FireBarManager::spawnFromMapData(const std::vector<FireBarSpawnData>& spawns)
{
    m_firebars.clear();

    if (m_fireballTexture.getSize().x == 0)
    {
        if (m_fireballTexture.loadFromFile(FireballSprite::Sheet))
        {
            m_fireballTexture.setSmooth(false);
            std::cout << "[FireBarManager] Fireball sprite sheet loaded (lazy)." << std::endl;
        }
        else
        {
            std::cerr << "[FireBarManager] Failed to load fireball sprite sheet: "
                      << FireballSprite::Sheet << std::endl;
        }
    }

    for (const auto& s : spawns)
    {
        m_firebars.push_back(std::make_unique<FireBar>(
            s.x, s.y,
            m_fireballTexture,
            s.fireCount,
            s.speed,
            s.clockwise,
            s.initialAngle
        ));
        std::cout << "  -> FireBar at (" << s.x << ", " << s.y
                  << ") fireCount=" << s.fireCount << " speed=" << s.speed << std::endl;
    }

    std::cout << "[FireBarManager] Spawned " << m_firebars.size()
              << " FireBar(s) from map data." << std::endl;
}

void FireBarManager::update(float deltaTime)
{
    for (auto& fb : m_firebars)
    {
        fb->update(deltaTime);
    }

    if (m_player)
        resolvePlayerCollision(m_player);

    if (m_player2)
        resolvePlayerCollision(m_player2);
}

void FireBarManager::resolvePlayerCollision(IPlayerManager* player)
{
    if (!player || !player->isAlive())
        return;

    // Bất tử trong i-frames hoặc StarState → không bị sát thương
    if (player->isInvincible() || player->isStarActive())
        return;

    const sf::FloatRect playerBox = player->getHitbox();

    for (const auto& fb : m_firebars)
    {
        if (fb->checkCollision(playerBox))
        {
            player->takeDamage();
            break; // Tránh multi-damage trong cùng 1 frame
        }
    }
}

void FireBarManager::render(sf::RenderWindow& window) const
{
    for (const auto& fb : m_firebars)
    {
        fb->render(window);
    }
}
