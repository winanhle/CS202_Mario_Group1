#include "MenuState.h"
#include "CharacterSelectState.h"
#include "../core/StateManager.h"
#include "../ui/SaveManager.h"
#include <SFML/Graphics.hpp>

MenuState::MenuState()
    : m_fontLoaded(false)
    , m_hasSave(false)
    , m_blinkTimer(0.0f)
    , m_showPrompt(true)
{
    // Load the Mario font
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    // Show the Continue option only when a save file exists
    m_hasSave = SaveManager::saveFileExists();

    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("SUPER MARIO");
        m_titleText.setCharacterSize(64);
        m_titleText.setFillColor(sf::Color::White);
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(2.0f);

        sf::FloatRect bounds = m_titleText.getLocalBounds();
        m_titleText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                                bounds.position.y + bounds.size.y / 2.0f });
        m_titleText.setPosition({ 400.0f, 180.0f });

        m_promptText.setFont(m_font);
        m_promptText.setString("Press SPACE to start");
        m_promptText.setCharacterSize(28);
        m_promptText.setFillColor(sf::Color::Yellow);
        m_promptText.setOutlineColor(sf::Color::Black);
        m_promptText.setOutlineThickness(1.0f);

        bounds = m_promptText.getLocalBounds();
        m_promptText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                                 bounds.position.y + bounds.size.y / 2.0f });

        if (m_hasSave)
        {
            m_continueText.setFont(m_font);
            m_continueText.setString("Press C to continue");
            m_continueText.setCharacterSize(28);
            m_continueText.setFillColor(sf::Color::White);
            m_continueText.setOutlineColor(sf::Color::Black);
            m_continueText.setOutlineThickness(1.0f);

            bounds = m_continueText.getLocalBounds();
            m_continueText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                                       bounds.position.y + bounds.size.y / 2.0f });
            m_continueText.setPosition({ 400.0f, 370.0f });

            // Move the SPACE prompt below the Continue option
            m_promptText.setPosition({ 400.0f, 430.0f });
        }
        else
        {
            m_promptText.setPosition({ 400.0f, 400.0f });
        }
    }
    else
    {
        // Fallback text without font
        m_titleText.setString("SUPER MARIO");
        m_titleText.setCharacterSize(48);
        m_titleText.setFillColor(sf::Color::White);
        m_titleText.setPosition({ 200.0f, 180.0f });

        m_promptText.setString("Press SPACE to start");
        m_promptText.setCharacterSize(24);
        m_promptText.setFillColor(sf::Color::Yellow);
        m_promptText.setPosition({ 250.0f, 400.0f });

        if (m_hasSave)
        {
            m_continueText.setString("Press C to continue");
            m_continueText.setCharacterSize(24);
            m_continueText.setFillColor(sf::Color::White);
            m_continueText.setPosition({ 250.0f, 370.0f });
        }
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
        else if (keyEvent->code == sf::Keyboard::Key::C && m_hasSave)
        {
            startGame(true);
        }
    }
}

void MenuState::update(float deltaTime)
{
    // Toggle prompt visibility on a timer
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL)
    {
        m_blinkTimer = 0.0f;
        m_showPrompt = !m_showPrompt;
    }
}

void MenuState::render(sf::RenderWindow& window) const
{
    sf::RectangleShape background({ 800.0f, 600.0f });
    background.setFillColor(sf::Color(50, 50, 180));  // deeper blue
    window.draw(background);

    window.draw(m_titleText);

    if (m_hasSave)
    {
        window.draw(m_continueText);
    }

    if (m_showPrompt)
    {
        window.draw(m_promptText);
    }
}

void MenuState::startGame(bool loadSave)
{
    // TODO: khi loadSave=true, khôi phục save sau khi khởi tạo PlayState.
    (void)loadSave;
    auto* manager = getStateManager();
    if (manager)
    {
        // Điều hướng qua màn hình chọn nhân vật → chọn chế độ → PlayState
        manager->changeState(std::make_unique<CharacterSelectState>());
    }
}