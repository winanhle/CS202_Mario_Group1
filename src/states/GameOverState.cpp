#include "GameOverState.h"
#include "MenuState.h"
#include "../core/StateManager.h"
#include <SFML/Graphics.hpp>
#include <string>

GameOverState::GameOverState()
    : m_fontLoaded(false)
    , m_blinkTimer(0.0f)
    , m_showPrompt(true)
    , m_finalScore(0)
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("GAME OVER");
        m_titleText.setCharacterSize(64);
        m_titleText.setFillColor(sf::Color::Red);
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(2.0f);

        sf::FloatRect bounds = m_titleText.getLocalBounds();
        m_titleText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                                bounds.position.y + bounds.size.y / 2.0f });
        m_titleText.setPosition({ 400.0f, 180.0f });

        m_scoreText.setFont(m_font);
        m_scoreText.setString("SCORE: " + std::to_string(m_finalScore));
        m_scoreText.setCharacterSize(28);
        m_scoreText.setFillColor(sf::Color::White);

        bounds = m_scoreText.getLocalBounds();
        m_scoreText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                                bounds.position.y + bounds.size.y / 2.0f });
        m_scoreText.setPosition({ 400.0f, 300.0f });

        m_promptText.setFont(m_font);
        m_promptText.setString("Press SPACE to return to menu");
        m_promptText.setCharacterSize(28);
        m_promptText.setFillColor(sf::Color::Yellow);
        m_promptText.setOutlineColor(sf::Color::Black);
        m_promptText.setOutlineThickness(1.0f);

        bounds = m_promptText.getLocalBounds();
        m_promptText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                                 bounds.position.y + bounds.size.y / 2.0f });
        m_promptText.setPosition({ 400.0f, 400.0f });
    }
    else
    {
        // No font loaded, use plain SFML defaults
        m_titleText.setString("GAME OVER");
        m_titleText.setCharacterSize(48);
        m_titleText.setFillColor(sf::Color::Red);
        m_titleText.setPosition({ 250.0f, 180.0f });

        m_scoreText.setString("SCORE: 0");
        m_scoreText.setCharacterSize(24);
        m_scoreText.setFillColor(sf::Color::White);
        m_scoreText.setPosition({ 300.0f, 300.0f });

        m_promptText.setString("Press SPACE to return to menu");
        m_promptText.setCharacterSize(24);
        m_promptText.setFillColor(sf::Color::Yellow);
        m_promptText.setPosition({ 220.0f, 400.0f });
    }
}

void GameOverState::setFinalScore(int score)
{
    m_finalScore = score;
    if (m_fontLoaded)
    {
        m_scoreText.setString("SCORE: " + std::to_string(m_finalScore));
        sf::FloatRect bounds = m_scoreText.getLocalBounds();
        m_scoreText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                                bounds.position.y + bounds.size.y / 2.0f });
        m_scoreText.setPosition({ 400.0f, 300.0f });
    }
}

void GameOverState::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::Space)
        {
            returnToMenu();
        }
    }
}

void GameOverState::update(float deltaTime)
{
    // Toggle prompt visibility on a timer
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL)
    {
        m_blinkTimer = 0.0f;
        m_showPrompt = !m_showPrompt;
    }
}

void GameOverState::render(sf::RenderWindow& window) const
{
    sf::RectangleShape background({ 800.0f, 600.0f });
    background.setFillColor(sf::Color(20, 20, 20));
    window.draw(background);

    window.draw(m_titleText);
    window.draw(m_scoreText);

    if (m_showPrompt)
    {
        window.draw(m_promptText);
    }
}

void GameOverState::returnToMenu()
{
    auto* manager = getStateManager();
    if (manager)
    {
        manager->changeState(std::make_unique<MenuState>());
    }
}
