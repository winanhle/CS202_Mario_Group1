#include "PlayerInputHandler.h"

PlayerInputHandler::PlayerInputHandler(const KeyBinding& keys) : m_keys(keys) {}

Command* PlayerInputHandler::handleEvent(const sf::Event& event) {
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->code == m_keys.jump1st || keyEvent->code == m_keys.jump2nd || keyEvent->code == m_keys.jump3rd) {
            return &m_jumpCommand;
        }
    }
    else if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        if (keyReleased->code == m_keys.jump1st || keyReleased->code == m_keys.jump2nd || keyReleased->code == m_keys.jump3rd) {
            return &m_stopJumpCommand;
        }
    }
    return nullptr;
}

Command* PlayerInputHandler::handleRealtimeInput() {
    if (sf::Keyboard::isKeyPressed(m_keys.left1st) || sf::Keyboard::isKeyPressed(m_keys.left2nd)) {
        return &m_moveLeftCommand;
    }
    else if (sf::Keyboard::isKeyPressed(m_keys.right1st) || sf::Keyboard::isKeyPressed(m_keys.right2nd)) {
        return &m_moveRightCommand;
    }
    return &m_stopHorizontalCommand;
}