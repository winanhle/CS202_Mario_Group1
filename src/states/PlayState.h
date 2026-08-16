#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include <memory>

namespace sf {
class Event;
class RenderWindow;
}

class GameWorld;
class ISettingsManager;

/**
 * @class PlayState
 * @brief Main gameplay state
 *
 * PlayState owns and coordinates the GameWorld.
 * It delegates all game logic to GameWorld and its modules.
 *
 * Nhận GameConfig từ ModeSelectState để biết:
 *   - Nhân vật Player 1 (Mario / Luigi)
 *   - Nhân vật Player 2 (chỉ dùng ở 2P mode)
 *   - Chế độ chơi (SinglePlayer / TwoPlayer)
 *
 * Quynh Anh (Architect) maintains this class.
 */
class PlayState : public GameState
{
public:
    /** Khởi tạo với config từ màn hình lựa chọn */
    explicit PlayState(const GameConfig& config, std::shared_ptr<ISettingsManager> settings, bool loadSave = false);

    /** Constructor mặc định: 1P, Mario (dùng để quick-test) */
    PlayState();
    ~PlayState() override;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    std::unique_ptr<GameWorld> m_gameWorld;
    std::shared_ptr<ISettingsManager> m_settings;
    GameConfig m_config;
    bool m_loadSave;
    void setup(const GameConfig& config);
};
