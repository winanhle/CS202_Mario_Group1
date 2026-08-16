#include "FireballManager.h"
#include "FireballSprite.h"
#include <algorithm>
#include <stdexcept>
#include <string>

void FireballManager::initialize()
{
    if (!m_fireballTexture.loadFromFile(FireballSprite::Sheet))
    {
        throw std::runtime_error(
            "Failed to load fireball sprite sheet: " + std::string(FireballSprite::Sheet));
    }
}

void FireballManager::update(float dt)
{
    // Chỉ update khi dependency đã được inject (sau GameWorld::injectDependencies)
    if (m_mapManager && m_enemyManager)
    {
        for (auto& fb : m_fireballs)
            fb.update(dt, *m_mapManager, *m_enemyManager);
    }

    // Xóa các cầu lửa đã chết (Erase-Remove Idiom).
    // std::erase_if chỉ có ở C++20 — dự án đang dùng C++17 nên dùng remove_if.
    m_fireballs.erase(
        std::remove_if(m_fireballs.begin(), m_fireballs.end(),
            [](const Fireball& fb) { return fb.isDead(); }),
        m_fireballs.end());
}

void FireballManager::render(sf::RenderWindow& window) const
{
    for (const auto& fb : m_fireballs)
        fb.render(window);
}

void FireballManager::spawnFireball(float x, float y, int direction)
{
    // emplace_back construct Fireball in-place trong vector, tránh copy thừa.
    m_fireballs.emplace_back(x, y, direction, m_fireballTexture);
}