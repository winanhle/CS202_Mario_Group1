#pragma once

#include "../interfaces/IPlayerManager.h"

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @class PlayerManager
 * @brief Stub implementation of player management
 * 
 * Developer: Le Tran
 * Status: STUB - Replace with full implementation
 * 
 * This stub provides minimal functionality to keep the project compilable.
 * Le Tran will implement full Mario character logic here.
 */
class PlayerManager : public IPlayerManager
{
public:
    PlayerManager();
    ~PlayerManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    bool isAlive() const override;
    int getScore() const override;
    int getLives() const override;
    float getPositionX() const override;
    float getPositionY() const override;

private:
    int m_score;
    int m_lives;
    float m_positionX;
    float m_positionY;
    bool m_isAlive;
};
