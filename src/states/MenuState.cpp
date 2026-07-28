#include "MenuState.h"
#include "PlayState.h"
#include "../core/StateManager.h"
#include <SFML/Graphics.hpp>

MenuState::MenuState()
    : m_fontLoaded(false)
    , m_blinkTimer(0.0f)
    , m_showPrompt(true)
{
    // Load the Mario font
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");
    // --- Title: "SUPER MARIO" ---
    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("SUPER MARIO");
        m_titleText.setCharacterSize(64);
        m_titleText.setFillColor(sf::Color::White);
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(2.0f);

        // Center the title
        sf::FloatRect bounds = m_titleText.getLocalBounds();
        m_titleText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                                bounds.position.y + bounds.size.y / 2.0f });
        m_titleText.setPosition({ 400.0f, 200.0f });

        m_promptText.setFont(m_font);
        m_promptText.setString("Press SPACE to start");
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
        // Fallback text without font
        m_titleText.setString("SUPER MARIO");
        m_titleText.setCharacterSize(48);
        m_titleText.setFillColor(sf::Color::White);
        m_titleText.setPosition({ 200.0f, 200.0f });

        m_promptText.setString("Press SPACE to start");
        m_promptText.setCharacterSize(24);
        m_promptText.setFillColor(sf::Color::Yellow);
        m_promptText.setPosition({ 250.0f, 400.0f });
    }
}

void MenuState::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::Space)
        {
            startGame();
        }
    }
}

void MenuState::update(float deltaTime)
{
    // Blink the prompt text
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL)
    {
        m_blinkTimer = 0.0f;
        m_showPrompt = !m_showPrompt;
    }
}

void MenuState::render(sf::RenderWindow& window) const
{
    // Draw gradient-like blue background
    sf::RectangleShape background({ 800.0f, 600.0f });
    background.setFillColor(sf::Color(50, 50, 180));  // Deeper blue
    window.draw(background);
    // Draw title
    window.draw(m_titleText);
    // Draw blinking prompt
    if (m_showPrompt)
    {
        window.draw(m_promptText);
    }
}
void MenuState::startGame()
{
    auto* manager = getStateManager();
    if (manager)
    {
        manager->changeState(std::make_unique<PlayState>());
    }
}
