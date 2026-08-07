#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Command.h"

struct KeyBinding {
    sf::Keyboard::Key jump1st;
    sf::Keyboard::Key jump2nd;
    sf::Keyboard::Key jump3rd;
    sf::Keyboard::Key left1st;
    sf::Keyboard::Key left2nd;
    sf::Keyboard::Key right1st;
    sf::Keyboard::Key right2nd;
};

class PlayerInputHandler {
private:
    KeyBinding m_keys;
    // Khởi tạo sẵn các lệnh để tái sử dụng (Object Pooling cơ bản)
    JumpCommand m_jumpCommand;
    StopJumpCommand m_stopJumpCommand;
    MoveLeftCommand m_moveLeftCommand;
    MoveRightCommand m_moveRightCommand;
    StopHorizontalCommand m_stopHorizontalCommand;

public:
    // Trả về con trỏ thô (raw pointer) vì InputHandler vẫn giữ quyền sở hữu các Command này
    PlayerInputHandler(const KeyBinding& keys);
    Command* handleEvent(const sf::Event& event);
    Command* handleRealtimeInput();
};