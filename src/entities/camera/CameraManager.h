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
    void setFollowTargets(const IPlayerManager* p1, const IPlayerManager* p2) override;
    sf::View getView() const override;

private:
    sf::View m_view;
    const IPlayerManager* m_targetPlayer  = nullptr;
    const IPlayerManager* m_targetPlayer2 = nullptr; // null trong 1P mode

    float m_deadzoneWidth;
    float m_viewWidth;
    float m_viewHeight;
    float m_lerpSpeedX;
    float m_lerpSpeedY;
    float m_mapWidth;
};
