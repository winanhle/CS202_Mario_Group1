#include "StateManager.h"
#include "GameState.h"

StateManager::StateManager()
    : m_currentState(nullptr)
{
}

void StateManager::changeState(GameState::Ptr newState)
{
    m_currentState = std::move(newState);
}

void StateManager::handleInput(const sf::Event& event)
{
    if (m_currentState)
    {
        m_currentState->handleInput(event);
    }
}

void StateManager::update(float deltaTime)
{
    if (m_currentState)
    {
        m_currentState->update(deltaTime);
    }
}

void StateManager::render(sf::RenderWindow& window) const
{
    if (m_currentState)
    {
        m_currentState->render(window);
    }
}

bool StateManager::hasActiveState() const
{
    return m_currentState != nullptr;
}
