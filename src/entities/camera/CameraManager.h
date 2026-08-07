#pragma once

#include "../../interfaces/ICameraManager.h"

class CameraManager : public ICameraManager
{
public:
    CameraManager();
    ~CameraManager() override = default;

    void initialize(sf::Vector2u mapSizePixels) override;
    void update(float deltaTime) override;
    void setFollowTarget(const IPlayerManager* player) override;
    sf::View getView() const override;

private:
    sf::View m_view;
    const IPlayerManager* m_targetPlayer;

    float m_deadzoneWidth;
    float m_viewWidth;
    float m_viewHeight;
    float m_lerpSpeedX;
    float m_lerpSpeedY;
    float m_mapWidth;
};
