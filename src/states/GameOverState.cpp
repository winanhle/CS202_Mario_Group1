#include "GameOverState.h"
#include <SFML/Graphics.hpp>

GameOverState::GameOverState()
    : m_displayText("Game Over")
{
}

void GameOverState::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::Space)
        {
            // TODO: Transition back to MenuState
        }
    }
}

void GameOverState::update(float deltaTime)
{
    // Placeholder for game over state updates
}

void GameOverState::render(sf::RenderWindow& window) const
{
    sf::RectangleShape background({ 800.0f, 600.0f });
    background.setFillColor(sf::Color::Red);
    window.draw(background);
}
