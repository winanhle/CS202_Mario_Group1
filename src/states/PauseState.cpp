#include "PauseState.h"
#include <SFML/Graphics.hpp>

PauseState::PauseState()
    : m_displayText("Paused")
{
}

void PauseState::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::P)
        {
            // TODO: Transition back to PlayState
        }
    }
}

void PauseState::update(float deltaTime)
{
    // Placeholder for pause state updates
}

void PauseState::render(sf::RenderWindow& window) const
{
    sf::RectangleShape background({ 800.0f, 600.0f });
    background.setFillColor(sf::Color::Yellow);
    window.draw(background);
}
