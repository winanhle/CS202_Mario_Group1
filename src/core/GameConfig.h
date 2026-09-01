#pragma once

#include <string>

/**
 * @enum CharacterType
 * @brief Nhân vật người chơi có thể chọn
 */
enum class CharacterType { Mario, Luigi };

/**
 * @enum GameMode
 * @brief Chế độ chơi
 */
enum class GameMode { SinglePlayer, TwoPlayer };

/**
 * @struct GameConfig
 * @brief Cấu hình game được truyền từ màn hình lựa chọn vào PlayState.
 *
 * Được tạo tại CharacterSelectState, hoàn chỉnh tại ModeSelectState,
 * sau đó truyền vào PlayState để build đúng số lượng và loại player.
 */
struct GameConfig {
    CharacterType player1Character = CharacterType::Mario;
    CharacterType player2Character = CharacterType::Luigi;
    GameMode      mode             = GameMode::SinglePlayer;
    
    std::string customMapPath = ""; // Empty means normal progression
    bool fromEditor = false;         // True when launched from MapEditorState
};
