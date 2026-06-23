#pragma once

#include "../interfaces/IHUDManager.h"

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @class HUDManager
 * @brief Stub implementation of HUD/UI management
 * 
 * Developer: Nguyen Phuc
 * Status: STUB - Replace with full implementation
 * 
 * This stub provides minimal functionality to keep the project compilable.
 * Nguyen Phuc will implement full UI and HUD rendering here.
 */
class HUDManager : public IHUDManager
{
public:
    HUDManager();
    ~HUDManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    void updateScore(int score) override;
    void updateLives(int lives) override;
    void updateEnemyCount(int count) override;

private:
    int m_displayScore;
    int m_displayLives;
    int m_displayEnemyCount;
};
