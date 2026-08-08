#pragma once

#include "../core/GameState.h"
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
 * Quynh Anh (Architect) maintains this class.
 * Other developers should NOT modify this file.
 * 
 * PlayState is the bridge between StateManager (game flow)
 * and GameWorld (actual gameplay systems).
 */
class PlayState : public GameState
{
public:
    explicit PlayState(std::shared_ptr<ISettingsManager> settings, bool loadSave = false);
    ~PlayState() override;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    std::unique_ptr<GameWorld> m_gameWorld;
    std::shared_ptr<ISettingsManager> m_settings;
    bool m_loadSave;
};