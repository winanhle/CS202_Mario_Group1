#include "Game.h"
#include "../states/MenuState.h"
#include "../ui/SettingsManager.h"
#include "../ui/SaveManager.h"

Game::Game()
    : m_window(std::make_unique<sf::RenderWindow>(
        sf::VideoMode({ 800u, 600u }),
        "Mario"
    ))
    , m_settings(std::make_shared<SettingsManager>())
    , m_saveManager(std::make_shared<SaveManager>())
{
    m_settings->initialize();
    m_saveManager->initialize();
    m_window->setFramerateLimit(static_cast<unsigned int>(FRAME_RATE));

    if (sf::Image icon; icon.loadFromFile("assets/shortcut.png"))
    {
        m_window->setIcon(icon.getSize(), icon.getPixelsPtr());
    }

    m_stateManager.changeState(std::make_unique<MenuState>(m_settings, m_saveManager));
}

void Game::run()
{
    while (m_window->isOpen())
    {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents()
{
    while (const auto event = m_window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window->close();
        }
        else
        {
            m_stateManager.handleInput(*event);
        }
    }
}

void Game::update()
{
    float deltaTime = m_frameClock.restart().asSeconds();
    m_stateManager.update(deltaTime);
}

void Game::render()
{
    m_window->clear(sf::Color::Black);
    m_stateManager.render(*m_window);
    m_window->display();
}
