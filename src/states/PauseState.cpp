#include "PauseState.h"
#include "MenuState.h"
#include "../core/StateManager.h"
#include "../interfaces/ISaveManager.h"
#include "../interfaces/IPlayerManager.h"
#include "../interfaces/ISoundManager.h"
#include <SFML/Graphics.hpp>

PauseState::PauseState(
    std::shared_ptr<ISettingsManager> settings,
    std::shared_ptr<ISaveManager> saveManager,
    IPlayerManager* player1,
    IPlayerManager* player2,
    std::optional<GameMemento> saveSnapshot,
    std::shared_ptr<ISoundManager> soundManager
)
    : m_settings(std::move(settings))
    , m_saveManager(std::move(saveManager))
    , m_soundManager(std::move(soundManager))
    , m_player1(player1)
    , m_player2(player2)
    , m_saveSnapshot(std::move(saveSnapshot))
    , m_menu(*m_settings, /*pauseContext=*/true, m_soundManager.get())
{
}

void PauseState::handleInput(const sf::Event& event)
{
    SettingsMenu::Request request = m_menu.handleInput(event);

    switch (request)
    {
    case SettingsMenu::Request::Resume:
    {
        if (m_soundManager)
            m_soundManager->playPause();

        // Re-apply key bindings from the latest settings so any rebind made
        // in the pause menu takes effect immediately when the game resumes.
        if (m_player1)
            m_player1->initialize(m_settings.get());
        if (m_player2)
            m_player2->initialize(m_settings.get());
        auto* manager = getStateManager();
        if (manager)
        {
            // Pop back to PlayState below; the game world is untouched
            manager->popState();
        }
        break;
    }
    case SettingsMenu::Request::SaveAndQuit:
        saveAndQuitToMenu();
        break;
    case SettingsMenu::Request::QuitToMenu:
    {
        auto* manager = getStateManager();
        if (manager)
        {
            // Pop the pause state, then replace PlayState with the menu
            manager->popState();
            manager->changeState(std::make_unique<MenuState>(m_settings, m_saveManager, m_soundManager));
        }
        break;
    }
    case SettingsMenu::Request::None:
    case SettingsMenu::Request::ExitSettings:
        // Nothing to do; the menu stays open
        break;
    }
}

void PauseState::saveAndQuitToMenu()
{
    if (m_soundManager)
        m_soundManager->playSaveGame();

    // Persist the game state Memento snapshot if available
    if (m_saveManager)
    {
        if (m_saveSnapshot.has_value())
        {
            if (!m_saveManager->saveGame(m_saveSnapshot.value()))
            {
                std::cerr << "[PauseState] ERROR: Failed to save game on exit!" << std::endl;
            }
        }
        else
        {
            std::cerr << "[PauseState] WARNING: Save & Quit called with no active save snapshot." << std::endl;
        }
    }

    auto* manager = getStateManager();
    if (manager)
    {
        manager->popState();
        manager->changeState(std::make_unique<MenuState>(m_settings, m_saveManager, m_soundManager));
    }
}

void PauseState::update(float deltaTime)
{
    m_menu.update(deltaTime);
}

void PauseState::render(sf::RenderWindow& window) const
{
    // Dim everything behind so the frozen game is visible but muted
    sf::RectangleShape overlay({ 800.0f, 600.0f });
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    m_menu.render(window);
}