#include "MenuState.h"
#include <SFML/Graphics.hpp>

MenuState::MenuState()
    : m_displayText("Menu")
{
}

void MenuState::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::Space)
        {
            // TODO: Transition to PlayState
        }
    }
}

void MenuState::update(float deltaTime)
{
    // Placeholder for menu updates
}

void MenuState::render(sf::RenderWindow& window) const
{
    sf::RectangleShape background({ 800.0f, 600.0f });
    background.setFillColor(sf::Color::Blue);
    window.draw(background);
}
