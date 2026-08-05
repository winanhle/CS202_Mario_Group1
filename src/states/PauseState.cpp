#include "PauseState.h"
#include "../core/StateManager.h"
#include <SFML/Graphics.hpp>

PauseState::PauseState()
    : m_fontLoaded(false)
    , m_blinkTimer(0.0f)
    , m_showPrompt(true)
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    if (m_fontLoaded)
    {
        m_pauseTitle.setFont(m_font);
        m_pauseTitle.setString("PAUSED");
        m_pauseTitle.setCharacterSize(64);
        m_pauseTitle.setFillColor(sf::Color::White);
        m_pauseTitle.setOutlineColor(sf::Color::Black);
        m_pauseTitle.setOutlineThickness(2.0f);

        sf::FloatRect bounds = m_pauseTitle.getLocalBounds();
        m_pauseTitle.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                                 bounds.position.y + bounds.size.y / 2.0f });
        m_pauseTitle.setPosition({ 400.0f, 200.0f });

        m_promptText.setFont(m_font);
        m_promptText.setString("Press P to resume");
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
        m_pauseTitle.setString("PAUSED");
        m_pauseTitle.setCharacterSize(48);
        m_pauseTitle.setFillColor(sf::Color::White);
        m_pauseTitle.setPosition({ 300.0f, 200.0f });

        m_promptText.setString("Press P to resume");
        m_promptText.setCharacterSize(24);
        m_promptText.setFillColor(sf::Color::Yellow);
        m_promptText.setPosition({ 280.0f, 400.0f });
    }
}

void PauseState::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::P)
        {
            resumeGame();
        }
    }
}

void PauseState::update(float deltaTime)
{
    // Toggle prompt visibility on a timer
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL)
    {
        m_blinkTimer = 0.0f;
        m_showPrompt = !m_showPrompt;
    }
}

void PauseState::render(sf::RenderWindow& window) const
{
    // Dim everything behind so the frozen game is visible but muted
    sf::RectangleShape overlay({ 800.0f, 600.0f });
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    window.draw(m_pauseTitle);

    if (m_showPrompt)
    {
        window.draw(m_promptText);
    }
}

void PauseState::resumeGame()
{
    auto* manager = getStateManager();
    if (manager)
    {
        // Pop back to PlayState below; the game world is untouched
        manager->popState();
    }
}