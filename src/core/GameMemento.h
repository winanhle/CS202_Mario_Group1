#pragma once

#include "GameConfig.h"

/**
 * @struct GameMemento
 * @brief Memento pattern snapshot capturing all persistent game state.
 *
 * Encapsulates the score, lives, current level/stage, coins, and game configuration
 * (characters and game mode) without exposing internal representation or requiring
 * callers to handle individual primitive fields.
 */
struct GameMemento
{
    int score = 0;
    int lives = 3;
    int stage = 1;
    int coins = 0;
    GameConfig config{};
};
