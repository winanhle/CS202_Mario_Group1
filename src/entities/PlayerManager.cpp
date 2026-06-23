#include "PlayerManager.h"
#include <SFML/Graphics.hpp>

PlayerManager::PlayerManager()
    : m_score(0), m_lives(3), m_positionX(100.0f), m_positionY(400.0f), m_isAlive(true)
{
}

void PlayerManager::initialize()
{
    // TODO: Le Tran - Initialize player character
    // - Load sprite
    // - Set initial position
    // - Initialize animation state
}

void PlayerManager::update(float deltaTime)
{
    // TODO: Le Tran - Update player logic
    // - Apply gravity
    // - Handle movement
    // - Check collisions with enemies and items
    // - Update animation
}

void PlayerManager::render(sf::RenderWindow& window) const
{
    // TODO: Le Tran - Render player
    // For now, draw a placeholder rectangle
    sf::RectangleShape player({ 32.0f, 48.0f });
    player.setFillColor(sf::Color::Red);
    player.setPosition({ m_positionX, m_positionY });
    window.draw(player);
}

void PlayerManager::handleInput(const sf::Event& event)
{
    // TODO: Le Tran - Handle player input
    // - Left/Right movement
    // - Jump
    // - Action buttons
}

bool PlayerManager::isAlive() const
{
    return m_isAlive;
}

int PlayerManager::getScore() const
{
    return m_score;
}

int PlayerManager::getLives() const
{
    return m_lives;
}

float PlayerManager::getPositionX() const
{
    return m_positionX;
}

float PlayerManager::getPositionY() const
{
    return m_positionY;
}
