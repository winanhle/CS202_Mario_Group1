#include "StateManager.h"
#include "GameState.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <cstdint>

StateManager::StateManager()
{
    m_fadeRect.setSize({ 800.0f, 600.0f });
}

void StateManager::changeState(GameState::Ptr newState)
{
    if (m_fadePhase == FadePhase::None)
    {
        m_pendingFadeState = std::move(newState);
        m_fadePhase = FadePhase::FadeOut;
        m_fadeTimer = 0.0f;
    }
    else
    {
        // Fallback if a fade is somehow already in progress (should not happen)
        m_pendingTransitions.push_back({ PendingOp::Change, std::move(newState) });
    }
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
    // Ignore input during fade transitions
    if (m_fadePhase != FadePhase::None)
    {
        return;
    }

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
    if (m_fadePhase != FadePhase::None)
    {
        m_fadeTimer += deltaTime;
        if (m_fadeTimer >= FADE_DURATION)
        {
            if (m_fadePhase == FadePhase::FadeOut)
            {
                // Fade out complete, swap state and begin fade in
                m_pendingTransitions.push_back({ PendingOp::Change, std::move(m_pendingFadeState) });
                m_fadePhase = FadePhase::FadeIn;
                m_fadeTimer = 0.0f;
            }
            else if (m_fadePhase == FadePhase::FadeIn)
            {
                // Fade in complete
                m_fadePhase = FadePhase::None;
            }
        }
    }

    auto* state = currentState();
    if (state && m_fadePhase == FadePhase::None)
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

    if (m_fadePhase != FadePhase::None)
    {
        float progress = m_fadeTimer / FADE_DURATION;
        if (progress > 1.0f) progress = 1.0f;

        float alpha = 0.0f;
        if (m_fadePhase == FadePhase::FadeOut)
        {
            alpha = progress * 255.0f;
        }
        else if (m_fadePhase == FadePhase::FadeIn)
        {
            alpha = (1.0f - progress) * 255.0f;
        }

        m_fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(alpha)));
        
        // Temporarily reset view so fade overlay covers the whole screen
        sf::View currentView = window.getView();
        window.setView(window.getDefaultView());
        window.draw(m_fadeRect);
        window.setView(currentView);
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