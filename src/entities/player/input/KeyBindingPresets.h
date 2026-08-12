#pragma once
#include "PlayerInputHandler.h"

/**
 * @namespace KeyBindingPresets
 * @brief Bộ phím mặc định cho từng chế độ chơi.
 *
 * Single player  : tất cả phím (W/Up/Space jump, A/Left left, D/Right right)
 * 2P – Player 1  : WASD + Space
 * 2P – Player 2  : mũi tên ↑ ← →
 */
namespace KeyBindingPresets
{
    /**
     * @brief Single player – dùng tất cả phím.
     * jump1st=W, jump2nd=Up, jump3rd=Space
     * left1st=A, left2nd=Left
     * right1st=D, right2nd=Right
     */
    inline KeyBinding singlePlayer()
    {
        KeyBinding kb;
        kb.jump1st  = sf::Keyboard::Key::W;
        kb.jump2nd  = sf::Keyboard::Key::Up;
        kb.jump3rd  = sf::Keyboard::Key::Space;
        kb.left1st  = sf::Keyboard::Key::A;
        kb.left2nd  = sf::Keyboard::Key::Left;
        kb.right1st = sf::Keyboard::Key::D;
        kb.right2nd = sf::Keyboard::Key::Right;
        kb.shoot    = sf::Keyboard::Key::F;
        return kb;
    }

    /**
     * @brief 2-Player – Player 1 dùng WASD + Space.
     * jump = W hoặc Space, left = A, right = D
     */
    inline KeyBinding player1TwoPlayer()
    {
        KeyBinding kb;
        kb.jump1st  = sf::Keyboard::Key::W;
        kb.jump2nd  = sf::Keyboard::Key::Space;
        kb.jump3rd  = sf::Keyboard::Key::W;  // slot dư, trùng jump1st
        kb.left1st  = sf::Keyboard::Key::A;
        kb.left2nd  = sf::Keyboard::Key::A;  // slot dư
        kb.right1st = sf::Keyboard::Key::D;
        kb.right2nd = sf::Keyboard::Key::D;  // slot dư
        kb.shoot    = sf::Keyboard::Key::F;
        return kb;
    }

    /**
     * @brief 2-Player – Player 2 dùng mũi tên ↑ ← →.
     * jump = ↑, left = ←, right = →
     */
    inline KeyBinding player2TwoPlayer()
    {
        KeyBinding kb;
        kb.jump1st  = sf::Keyboard::Key::Up;
        kb.jump2nd  = sf::Keyboard::Key::Up;   // slot dư
        kb.jump3rd  = sf::Keyboard::Key::Up;   // slot dư
        kb.left1st  = sf::Keyboard::Key::Left;
        kb.left2nd  = sf::Keyboard::Key::Left; // slot dư
        kb.right1st = sf::Keyboard::Key::Right;
        kb.right2nd = sf::Keyboard::Key::Right; // slot dư
        kb.shoot    = sf::Keyboard::Key::Period;
        return kb;
    }
}
