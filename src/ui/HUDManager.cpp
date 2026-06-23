#include "HUDManager.h"
#include <SFML/Graphics.hpp>

HUDManager::HUDManager()
    : m_displayScore(0), m_displayLives(0), m_displayEnemyCount(0)
{
}

void HUDManager::initialize()
{
    // TODO: Nguyen Phuc - Initialize HUD manager
    // - Load fonts
    // - Set up UI elements
    // - Initialize display values
}

void HUDManager::update(float deltaTime)
{
    // TODO: Nguyen Phuc - Update HUD logic
    // - Animate HUD elements
    // - Handle UI state changes
}

void HUDManager::render(sf::RenderWindow& window) const
{
    // TODO: Nguyen Phuc - Render HUD
    // For now, draw placeholder rectangles for debug info

    // Score display area (top-left)
    sf::RectangleShape scoreBox({ 200.0f, 40.0f });
    scoreBox.setFillColor(sf::Color::Cyan);
    scoreBox.setPosition({ 10.0f, 10.0f });
    window.draw(scoreBox);

    // Lives display area (top-center)
    sf::RectangleShape livesBox({ 200.0f, 40.0f });
    livesBox.setFillColor(sf::Color::Green);
    livesBox.setPosition({ 300.0f, 10.0f });
    window.draw(livesBox);

    // Enemy count display area (top-right)
    sf::RectangleShape enemyBox({ 200.0f, 40.0f });
    enemyBox.setFillColor(sf::Color::White);
    enemyBox.setPosition({ 590.0f, 10.0f });
    window.draw(enemyBox);
}

void HUDManager::handleInput(const sf::Event& event)
{
    // TODO: Nguyen Phuc - Handle HUD input
    // - Menu navigation
    // - Button clicks
}

void HUDManager::updateScore(int score)
{
    m_displayScore = score;
}

void HUDManager::updateLives(int lives)
{
    m_displayLives = lives;
}

void HUDManager::updateEnemyCount(int count)
{
    m_displayEnemyCount = count;
}
