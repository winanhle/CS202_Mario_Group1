#pragma once

#include "GameConfig.h"

#include <vector>

/**
 * @struct PlayerStateMemento
 * @brief Represents the persistent state of a single player.
 */
struct PlayerStateMemento
{
    int score = 0;
    int coins = 0;
};

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
    std::vector<PlayerStateMemento> players; // Supports N players
    int lives = 3; // Shared lives pool
    int stage = 1;
    GameConfig config{};

    int getTotalScore() const {
        int total = 0;
        for (const auto& p : players) {
            total += p.score;
        }
        return total;
    }
};
