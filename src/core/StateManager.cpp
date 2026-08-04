#include "StateManager.h"
#include "GameState.h"
#include <SFML/Window/Event.hpp>

StateManager::StateManager()
{
}

void StateManager::changeState(GameState::Ptr newState)
{
    m_pendingTransitions.push_back({ PendingOp::Change, std::move(newState) });
}

void StateManager::pushState(GameState::Ptr newState)
{
    m_pendingTransitions.push_back({ PendingOp::Push, std::move(newState) });
}

void StateManager::popState()
{
    m_pendingTransitions.push_back({ PendingOp::Pop, nullptr });
}

GameState* StateManager::currentState() const
{
    if (m_stateStack.empty())
    {
        return nullptr;
    }
    return m_stateStack.back().get();
}

void StateManager::handleInput(const sf::Event& event)
{
    // SFML sends repeated KeyPressed events while a key is held down. Track
    // which keys are down so a held key only triggers a state once.
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (!m_heldKeys.insert(keyEvent->code).second)
        {
            return; // auto-repeat while the key is still held, ignore it
        }
    }
    else if (const auto* releaseEvent = event.getIf<sf::Event::KeyReleased>())
    {
        m_heldKeys.erase(releaseEvent->code);
    }

    auto* state = currentState();
    if (state)
    {
        state->handleInput(event);
    }

    applyPending();
}

void StateManager::update(float deltaTime)
{
    auto* state = currentState();
    if (state)
    {
        state->update(deltaTime);
    }

    applyPending();
}

void StateManager::render(sf::RenderWindow& window) const
{
    // Render every state bottom-up so overlays (pause menu) draw on top of
    // the frozen game world underneath.
    for (const auto& state : m_stateStack)
    {
        if (state)
        {
            state->render(window);
        }
    }
}

bool StateManager::hasActiveState() const
{
    return !m_stateStack.empty();
}

void StateManager::applyPending()
{
    for (auto& pending : m_pendingTransitions)
    {
        switch (pending.op)
        {
        case PendingOp::Change:
            m_stateStack.clear();
            if (pending.state)
            {
                pending.state->setStateManager(this);
                m_stateStack.push_back(std::move(pending.state));
            }
            break;

        case PendingOp::Push:
            if (pending.state)
            {
                pending.state->setStateManager(this);
                m_stateStack.push_back(std::move(pending.state));
            }
            break;

        case PendingOp::Pop:
            if (m_stateStack.size() > 1)
            {
                m_stateStack.pop_back();
            }
            break;
        }
    }
    m_pendingTransitions.clear();
}
