#include "../ui/UIUtils.h"
#include "MenuState.h"
#include "CharacterSelectState.h"
#include "PlayState.h"
#include "StatsState.h"
#include "LeaderboardState.h"
#include "../core/StateManager.h"
#include "../core/GameConfig.h"
#include "../interfaces/ISaveManager.h"
#include "../interfaces/ISoundManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>

MenuState::MenuState(
    std::shared_ptr<ISettingsManager> settings,
    std::shared_ptr<ISaveManager> saveManager,
    std::shared_ptr<ISoundManager> soundManager
)
    : m_fontLoaded(false)
    , m_blinkTimer(0.0f)
    , m_showPrompt(true)
    , m_settings(std::move(settings))
    , m_saveManager(std::move(saveManager))
    , m_soundManager(std::move(soundManager))
    , m_settingsMenu(*m_settings, /*pauseContext=*/false, m_soundManager.get())
    , m_inSettings(false)
{
    m_bg.load();
    
    // Start menu music
    if (m_soundManager)
        m_soundManager->playMenuMusic();
    
    // Load the Mario font
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");
    if (!m_fontLoaded)
    {
        std::cerr << "[MenuState] ERROR: Failed to open font assets/fonts/SuperMario256.ttf\n";
    }

    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("SUPER MARIO");
        m_titleText.setCharacterSize(64);
        m_titleText.setFillColor(sf::Color::White);
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(2.0f);
        UIUtils::centerOrigin(m_titleText);
        m_titleText.setPosition({ 400.0f, 170.0f });

        const bool hasSave = m_saveManager && m_saveManager->hasSaveFile();

        m_promptText.setFont(m_font);
        m_promptText.setString("Press SPACE to Start");
        m_promptText.setCharacterSize(26);
        m_promptText.setFillColor(sf::Color::Yellow);
        m_promptText.setOutlineColor(sf::Color::Black);
        m_promptText.setOutlineThickness(1.0f);
        UIUtils::centerOrigin(m_promptText);
        m_promptText.setPosition({ 400.0f, hasSave ? 310.0f : 330.0f });

        m_continueText.setFont(m_font);
        m_continueText.setString("Press C to Continue");
        m_continueText.setCharacterSize(24);
        m_continueText.setFillColor(sf::Color(100, 240, 100));
        m_continueText.setOutlineColor(sf::Color::Black);
        m_continueText.setOutlineThickness(1.0f);
        UIUtils::centerOrigin(m_continueText);
        m_continueText.setPosition({ 400.0f, 355.0f });

        m_recordsText.setFont(m_font);
        m_recordsText.setString("Press R for Records/Trophies");
        m_recordsText.setCharacterSize(20);
        m_recordsText.setFillColor(sf::Color(200, 200, 220));
        m_recordsText.setOutlineColor(sf::Color::Black);
        m_recordsText.setOutlineThickness(1.0f);
        UIUtils::centerOrigin(m_recordsText);
        m_recordsText.setPosition({ 400.0f, hasSave ? 405.0f : 390.0f });

        m_highScoreText.setFont(m_font);
        m_highScoreText.setString("Press H for High Scores");
        m_highScoreText.setCharacterSize(22);
        m_highScoreText.setFillColor(sf::Color(255, 215, 0));
        m_highScoreText.setOutlineColor(sf::Color::Black);
        m_highScoreText.setOutlineThickness(1.0f);
        UIUtils::centerOrigin(m_highScoreText);
        m_highScoreText.setPosition({ 400.0f, hasSave ? 450.0f : 440.0f });
    }
    else
    {
        // Font unavailable — set default positions so objects aren't all at (0,0)
        m_titleText.setPosition({ 400.0f, 170.0f });
        m_promptText.setPosition({ 400.0f, 330.0f });
        m_continueText.setPosition({ 400.0f, 355.0f });
        m_recordsText.setPosition({ 400.0f, 390.0f });
        m_highScoreText.setPosition({ 400.0f, 440.0f });
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
            if (m_soundManager) m_soundManager->playStomp();
            if (auto* manager = getStateManager())
            {
                manager->changeState(std::make_unique<CharacterSelectState>(m_settings, m_saveManager, m_soundManager));
            }
        }
        else if (keyEvent->code == sf::Keyboard::Key::C)
        {
            if (m_saveManager && m_saveManager->hasSaveFile())
            {
                if (m_soundManager) m_soundManager->playStomp();
                if (auto* manager = getStateManager())
                {
                    manager->changeState(std::make_unique<PlayState>(
                        GameConfig{}, m_settings, m_saveManager, /*loadSave=*/true, m_soundManager
                    ));
                }
            }
        }
        else if (keyEvent->code == sf::Keyboard::Key::H)
        {
            if (m_soundManager) m_soundManager->playSelect();
            if (auto* manager = getStateManager())
            {
                manager->changeState(std::make_unique<LeaderboardState>(m_settings, m_saveManager, m_soundManager));
            }
        }
        else if (keyEvent->code == sf::Keyboard::Key::R)
        {
            if (m_soundManager) m_soundManager->playSelect();
            if (auto* manager = getStateManager())
            {
                manager->changeState(std::make_unique<StatsState>(m_settings, m_saveManager, m_soundManager));
            }
        }
        else if (keyEvent->code == sf::Keyboard::Key::Escape)
        {
            if (m_soundManager) m_soundManager->playSelect();
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

    if (m_showPrompt)
    {
        window.draw(m_promptText);
    }

    if (m_saveManager && m_saveManager->hasSaveFile())
    {
        window.draw(m_continueText);
    }

    window.draw(m_recordsText);
    window.draw(m_highScoreText);
}