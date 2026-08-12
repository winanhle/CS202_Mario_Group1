#include "PauseState.h"
#include "MenuState.h"
#include "../core/StateManager.h"
#include "../interfaces/ISaveManager.h"
#include "../interfaces/IPlayerManager.h"
#include <SFML/Graphics.hpp>

PauseState::PauseState(std::shared_ptr<ISettingsManager> settings,
                       ISaveManager* saveManager,IPlayerManager* player)
    : m_settings(std::move(settings))
    , m_saveManager(saveManager)
    , m_player(player)
    , m_menu(*m_settings, /*pauseContext=*/true)
{
}

void PauseState::handleInput(const sf::Event& event)
{
    SettingsMenu::Request request = m_menu.handleInput(event);

    switch (request)
    {
    case SettingsMenu::Request::Resume:
    {
        // Re-apply key bindings from the latest settings so any rebind made
        // in the pause menu takes effect immediately when the game resumes.
        if (m_player)
            m_player->initialize(m_settings.get());
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
            manager->changeState(std::make_unique<MenuState>(m_settings));
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
    // Persist the player state (already synced by PlayState before pausing)
    if (m_saveManager)
    {
        m_saveManager->saveGame();
    }

    auto* manager = getStateManager();
    if (manager)
    {
        manager->popState();
        manager->changeState(std::make_unique<MenuState>(m_settings));
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