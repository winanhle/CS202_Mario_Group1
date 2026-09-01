#include "MenuState.h"
#include "CharacterSelectState.h"
#include "LeaderboardState.h"
#include "../core/StateManager.h"
#include "../core/GameConfig.h"
#include "../interfaces/ISaveManager.h"
#include "../interfaces/ISoundManager.h"
#include "PlayState.h"
#include <SFML/Graphics.hpp>
#include <iostream>

MenuState::MenuState(
    std::shared_ptr<ISettingsManager> settings,
    std::shared_ptr<ISaveManager> saveManager,
    std::shared_ptr<ISoundManager> soundManager
)
    : m_fontLoaded(false)
    , m_hasSave(false)
    , m_blinkTimer(0.0f)
    , m_showPrompt(true)
    , m_settings(std::move(settings))
    , m_saveManager(std::move(saveManager))
    , m_soundManager(std::move(soundManager))
    , m_settingsMenu(*m_settings, /*pauseContext=*/false)
    , m_inSettings(false)
{
    m_bg.load();
    
    // Load the Mario font
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");
    if (!m_fontLoaded)
    {
        std::cerr << "[MenuState] ERROR: Failed to open font assets/fonts/SuperMario256.ttf\n";
    }

    // Show the Continue option only when a save file exists
    m_hasSave = m_saveManager ? m_saveManager->hasSaveFile() : false;

    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("SUPER MARIO");
        m_titleText.setCharacterSize(64);
        m_titleText.setFillColor(sf::Color::White);
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(2.0f);

        centerOrigin(m_titleText);
        m_titleText.setPosition({ 400.0f, 180.0f });

        m_promptText.setFont(m_font);
        m_promptText.setString("Press SPACE to start");
        m_promptText.setCharacterSize(28);
        m_promptText.setFillColor(sf::Color::Yellow);
        m_promptText.setOutlineColor(sf::Color::Black);
        m_promptText.setOutlineThickness(1.0f);

        centerOrigin(m_promptText);

        m_highScoreText.setFont(m_font);
        m_highScoreText.setString("Press H for High Scores");
        m_highScoreText.setCharacterSize(24);
        m_highScoreText.setFillColor(sf::Color(255, 215, 0));
        m_highScoreText.setOutlineColor(sf::Color::Black);
        m_highScoreText.setOutlineThickness(1.0f);

        centerOrigin(m_highScoreText);

        if (m_hasSave)
        {
            m_continueText.setFont(m_font);
            m_continueText.setString("Press C to continue");
            m_continueText.setCharacterSize(28);
            m_continueText.setFillColor(sf::Color::White);
            m_continueText.setOutlineColor(sf::Color::Black);
            m_continueText.setOutlineThickness(1.0f);

            centerOrigin(m_continueText);
            m_continueText.setPosition({ 400.0f, 350.0f });

            m_promptText.setPosition({ 400.0f, 405.0f });
            m_highScoreText.setPosition({ 400.0f, 460.0f });
        }
        else
        {
            m_promptText.setPosition({ 400.0f, 380.0f });
            m_highScoreText.setPosition({ 400.0f, 435.0f });
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
        m_promptText.setPosition({ 250.0f, 380.0f });

        m_highScoreText.setString("Press H for High Scores");
        m_highScoreText.setCharacterSize(20);
        m_highScoreText.setFillColor(sf::Color(255, 215, 0));
        m_highScoreText.setPosition({ 250.0f, 435.0f });

        if (m_hasSave)
        {
            m_continueText.setString("Press C to continue");
            m_continueText.setCharacterSize(24);
            m_continueText.setFillColor(sf::Color::White);
            m_continueText.setPosition({ 250.0f, 350.0f });
        }
    }
}

void MenuState::handleInput(const sf::Event& event)
{
    // While the settings menu is open, it consumes all input
    if (m_inSettings)
    {
        SettingsMenu::Request request = m_settingsMenu.handleInput(event);
        if (request == SettingsMenu::Request::ExitSettings)
        {
            m_inSettings = false;
        }
        return;
    }

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
        else if (keyEvent->code == sf::Keyboard::Key::H)
        {
            if (auto* manager = getStateManager())
            {
                manager->changeState(std::make_unique<LeaderboardState>(m_settings, m_saveManager, m_soundManager));
            }
        }
        else if (keyEvent->code == sf::Keyboard::Key::Escape)
        {
            m_inSettings = true;
            m_settingsMenu.openSettings();
        }
    }
}

void MenuState::update(float deltaTime)
{
    if (m_inSettings)
    {
        m_settingsMenu.update(deltaTime);
        return;
    }

    // Toggle prompt visibility on a timer
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL)
    {
        m_blinkTimer = 0.0f;
        m_showPrompt = !m_showPrompt;
    }
    
    m_bg.update(deltaTime);
}

void MenuState::render(sf::RenderWindow& window) const
{
    // Ensure standard 800x600 view for UI rendering
    sf::View defaultView({ 400.0f, 300.0f }, { 800.0f, 600.0f });
    window.setView(defaultView);

    m_bg.render(window);

    if (m_inSettings)
    {
        m_settingsMenu.render(window);
        return;
    }

    window.draw(m_titleText);

    if (m_hasSave)
    {
        window.draw(m_continueText);
    }

    if (m_showPrompt)
    {
        window.draw(m_promptText);
    }

    window.draw(m_highScoreText);
}

void MenuState::startGame(bool loadSave)
{
    auto* manager = getStateManager();
    if (manager)
    {
        if (loadSave)
        {
            if (m_saveManager)
            {
                if (auto memento = m_saveManager->loadGame())
                {
                    manager->changeState(std::make_unique<PlayState>(memento->config, m_settings, m_saveManager, loadSave, m_soundManager));
                }
                else
                {
                    std::cerr << "[MenuState] ERROR: Failed to load save game snapshot." << std::endl;
                }
            }
        }
        else
        {
            manager->changeState(std::make_unique<CharacterSelectState>(m_settings, m_saveManager, loadSave, m_soundManager));
        }
    }
}