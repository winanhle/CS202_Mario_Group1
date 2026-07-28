#include "HUDManager.h"
#include <SFML/Graphics.hpp>

HUDManager::HUDManager()
    : m_displayScore(0), m_displayLives(3), m_displayEnemyCount(0), m_fontLoaded(false)
{
}

void HUDManager::initialize()
{
    // Load the Mario font (SFML 3.x uses openFromFile)
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    // Configure HUD background bar (created once, drawn every frame)
    m_hudBar.setSize({ 800.0f, 75.0f });
    m_hudBar.setFillColor(sf::Color(0, 0, 0, 150));
    m_hudBar.setPosition({ 0.0f, 0.0f });

    // Score Label (top-left)
    m_scoreLabel.setFont(m_font);
    m_scoreLabel.setString("SCORE");
    m_scoreLabel.setCharacterSize(24);
    m_scoreLabel.setFillColor(sf::Color::White);
    m_scoreLabel.setPosition({ 20.0f, 10.0f });

    m_scoreValue.setFont(m_font);
    m_scoreValue.setString("0");
    m_scoreValue.setCharacterSize(28);
    m_scoreValue.setFillColor(sf::Color::White);
    m_scoreValue.setPosition({ 20.0f, 38.0f });

    // Lives Label (center-left)
    m_livesLabel.setFont(m_font);
    m_livesLabel.setString("LIVES");
    m_livesLabel.setCharacterSize(24);
    m_livesLabel.setFillColor(sf::Color::White);
    m_livesLabel.setPosition({ 250.0f, 10.0f });

    m_livesValue.setFont(m_font);
    m_livesValue.setString("x 3");
    m_livesValue.setCharacterSize(28);
    m_livesValue.setFillColor(sf::Color::White);
    m_livesValue.setPosition({ 250.0f, 38.0f });

    // Enemy Label (top-right)
    m_enemyLabel.setFont(m_font);
    m_enemyLabel.setString("ENEMIES");
    m_enemyLabel.setCharacterSize(24);
    m_enemyLabel.setFillColor(sf::Color::White);
    m_enemyLabel.setPosition({ 580.0f, 10.0f });

    m_enemyValue.setFont(m_font);
    m_enemyValue.setString("0");
    m_enemyValue.setCharacterSize(28);
    m_enemyValue.setFillColor(sf::Color::White);
    m_enemyValue.setPosition({ 580.0f, 38.0f });
}

void HUDManager::update(float deltaTime)
{
    // Update displayed text strings to match current values
    updateTextStrings();
}

void HUDManager::render(sf::RenderWindow& window) const
{
    if (!m_fontLoaded)
        return;

    // Draw semi-transparent HUD background bar
    window.draw(m_hudBar);

    // Draw all HUD text
    window.draw(m_scoreLabel);
    window.draw(m_scoreValue);
    window.draw(m_livesLabel);
    window.draw(m_livesValue);
    window.draw(m_enemyLabel);
    window.draw(m_enemyValue);
}

void HUDManager::handleInput(const sf::Event& event)
{
    // HUD input handling (for future menu interactions)
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

void HUDManager::updateTextStrings()
{
    m_scoreValue.setString(std::to_string(m_displayScore));
    m_livesValue.setString("x " + std::to_string(m_displayLives));
    m_enemyValue.setString(std::to_string(m_displayEnemyCount));
}
