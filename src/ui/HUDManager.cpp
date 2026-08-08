#include "HUDManager.h"
#include <SFML/Graphics.hpp>
#include <cmath>

HUDManager::HUDManager()
    : m_renderedScore(-1)
    , m_renderedLives(-1)
    , m_renderedItemCount(-1)
    , m_renderedTime(-1)
    , m_timeLeft(TIME_START)
    , m_blinkTimer(0.0f)
    , m_showTimerText(true)
    , m_scorePopTimer(0.0f)
    , m_scorePopActive(false)
    , m_fontLoaded(false)
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
    // Anchor the pop animation at the left edge, centered vertically
    sf::FloatRect scoreBounds = m_scoreValue.getLocalBounds();
    m_scoreValue.setOrigin({ 0.0f, scoreBounds.position.y + scoreBounds.size.y / 2.0f });
    m_scoreValue.setPosition({ 20.0f, 38.0f + scoreBounds.size.y / 2.0f });

    // Lives Label
    m_livesLabel.setFont(m_font);
    m_livesLabel.setString("LIVES");
    m_livesLabel.setCharacterSize(24);
    m_livesLabel.setFillColor(sf::Color::White);
    m_livesLabel.setPosition({ 200.0f, 10.0f });

    m_livesValue.setFont(m_font);
    m_livesValue.setString("x 3");
    m_livesValue.setCharacterSize(28);
    m_livesValue.setFillColor(sf::Color::White);
    m_livesValue.setPosition({ 200.0f, 38.0f });

    // Item count Label
    m_itemsLabel.setFont(m_font);
    m_itemsLabel.setString("ITEMS");
    m_itemsLabel.setCharacterSize(24);
    m_itemsLabel.setFillColor(sf::Color::White);
    m_itemsLabel.setPosition({ 380.0f, 10.0f });

    m_itemsValue.setFont(m_font);
    m_itemsValue.setString("0");
    m_itemsValue.setCharacterSize(28);
    m_itemsValue.setFillColor(sf::Color::White);
    m_itemsValue.setPosition({ 380.0f, 38.0f });

    // World / level Label
    m_worldLabel.setFont(m_font);
    m_worldLabel.setString("WORLD");
    m_worldLabel.setCharacterSize(24);
    m_worldLabel.setFillColor(sf::Color::White);
    m_worldLabel.setPosition({ 560.0f, 10.0f });

    m_worldValue.setFont(m_font);
    m_worldValue.setString("1-1"); // static for now; set once, never rebuilt
    m_worldValue.setCharacterSize(28);
    m_worldValue.setFillColor(sf::Color::White);
    m_worldValue.setPosition({ 560.0f, 38.0f });

    // Timer Label (top-right)
    m_timeLabel.setFont(m_font);
    m_timeLabel.setString("TIME");
    m_timeLabel.setCharacterSize(24);
    m_timeLabel.setFillColor(sf::Color::White);
    m_timeLabel.setPosition({ 700.0f, 10.0f });

    m_timeValue.setFont(m_font);
    m_timeValue.setString("300");
    m_timeValue.setCharacterSize(28);
    m_timeValue.setFillColor(sf::Color::White);
    m_timeValue.setPosition({ 700.0f, 38.0f });
}

void HUDManager::update(float deltaTime)
{
    updateTimer(deltaTime);
    updateScorePop(deltaTime);
}

void HUDManager::updateTimer(float deltaTime)
{
    if (m_timeLeft > 0.0f)
    {
        m_timeLeft -= deltaTime;
        if (m_timeLeft < 0.0f)
            m_timeLeft = 0.0f;
    }

    // Rebuild the string only when the displayed second changes
    int seconds = static_cast<int>(std::ceil(m_timeLeft));
    if (seconds != m_renderedTime)
    {
        m_renderedTime = seconds;
        m_timeValue.setString(std::to_string(seconds));
    }

    // Blink the timer when it gets low
    if (m_timeLeft <= TIME_LOW)
    {
        m_blinkTimer += deltaTime;
        if (m_blinkTimer >= BLINK_INTERVAL)
        {
            m_blinkTimer = 0.0f;
            m_showTimerText = !m_showTimerText;
        }
    }
    else
    {
        m_showTimerText = true;
    }
}

void HUDManager::updateScorePop(float deltaTime)
{
    if (!m_scorePopActive)
        return;

    if (m_scorePopTimer > 0.0f)
    {
        m_scorePopTimer -= deltaTime;
        if (m_scorePopTimer < 0.0f)
            m_scorePopTimer = 0.0f;

        float progress = 1.0f - (m_scorePopTimer / SCORE_POP_DURATION);
        float scale = 1.0f + 0.3f * std::sin(progress * PI);
        m_scoreValue.setScale({ scale, scale });
    }
    else
    {
        // Pop finished: reset the scale exactly once
        m_scoreValue.setScale({ 1.0f, 1.0f });
        m_scorePopActive = false;
    }
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
    window.draw(m_itemsLabel);
    window.draw(m_itemsValue);
    window.draw(m_worldLabel);
    window.draw(m_worldValue);
    window.draw(m_timeLabel);

    if (m_showTimerText)
    {
        window.draw(m_timeValue);
    }
}

void HUDManager::handleInput(const sf::Event& event)
{
    // HUD input handling (for future menu interactions)
    (void)event;
}

void HUDManager::updateScore(int score)
{
    if (score != m_renderedScore)
    {
        m_renderedScore = score;
        m_scoreValue.setString(std::to_string(score));
        m_scorePopTimer = SCORE_POP_DURATION;
        m_scorePopActive = true;
    }
}

void HUDManager::updateLives(int lives)
{
    if (lives != m_renderedLives)
    {
        m_renderedLives = lives;
        m_livesValue.setString("x " + std::to_string(lives));
    }
}

void HUDManager::updateItemCount(int count)
{
    if (count != m_renderedItemCount)
    {
        m_renderedItemCount = count;
        m_itemsValue.setString(std::to_string(count));
    }
}