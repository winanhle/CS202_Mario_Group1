#include "CameraManager.h"
#include "../../interfaces/IPlayerManager.h"

#include <algorithm>
#include <cmath>

CameraManager::CameraManager()
    : m_viewWidth(320.0f)
    , m_viewHeight(240.0f)
    , m_lerpSpeedX(5.0f)
    , m_lerpSpeedY(3.0f)
    , m_mapWidth(0.0f)
    , m_deadzoneWidth(0.0f)
{
}

void CameraManager::initialize(sf::Vector2u mapSizePixels)
{
    m_view.setSize(sf::Vector2f(m_viewWidth, m_viewHeight));
    m_view.setCenter(sf::Vector2f(m_viewWidth / 2.0f, m_viewHeight / 2.0f));

    m_mapWidth     = static_cast<float>(mapSizePixels.x);
    m_deadzoneWidth = m_viewWidth / 2.0f - 5.0f;
}

void CameraManager::update(float deltaTime)
{
    // Tính target X dựa trên số lượng player đang theo dõi
    float targetCenterX = 0.f;

    if (m_targetPlayer && m_targetPlayer2
        && m_targetPlayer->isAlive() && m_targetPlayer2->isAlive())
    {
        // 2P mode: follow điểm giữa của 2 player
        auto box1 = m_targetPlayer->getHitbox();
        auto box2 = m_targetPlayer2->getHitbox();
        float cx1 = box1.position.x + box1.size.x / 2.f;
        float cx2 = box2.position.x + box2.size.x / 2.f;
        targetCenterX = (cx1 + cx2) / 2.f;
    }
    else if (m_targetPlayer && m_targetPlayer->isAlive())
    {
        // 1P mode hoặc khi chỉ 1 player còn sống: follow player đó
        auto box = m_targetPlayer->getHitbox();
        targetCenterX = box.position.x + box.size.x / 2.f;
    }
    else if (m_targetPlayer2 && m_targetPlayer2->isAlive())
    {
        // Fallback: P1 đã chết, follow P2
        auto box = m_targetPlayer2->getHitbox();
        targetCenterX = box.position.x + box.size.x / 2.f;
    }
    else
    {
        return; // Không có player nào, giữ nguyên camera
    }

    sf::Vector2f center = m_view.getCenter();

    // Deadzone offsets (logic cũ)
    const float offsetLeft  = -100.0f;
    const float offsetRight =    0.0f;

    float deadLeft  = center.x + offsetLeft;
    float deadRight = center.x + offsetRight;

    float newTargetX = center.x;
    if (targetCenterX < deadLeft)
        newTargetX = targetCenterX - offsetLeft;
    else if (targetCenterX > deadRight)
        newTargetX = targetCenterX - offsetRight + 40.0f;

    float targetY = m_viewHeight / 2.0f;

    float alphaX = 1.0f - std::exp(-m_lerpSpeedX * deltaTime);
    float alphaY = 1.0f - std::exp(-m_lerpSpeedY * deltaTime);
    center.x += (newTargetX - center.x) * alphaX;
    center.y += (targetY    - center.y) * alphaY;

    float halfW = m_view.getSize().x / 2.0f;
    center.x = std::clamp(center.x, halfW, m_mapWidth - halfW);

    m_view.setCenter(center);
}

void CameraManager::setFollowTarget(const IPlayerManager* player)
{
    m_targetPlayer  = player;
    m_targetPlayer2 = nullptr;
}

void CameraManager::setFollowTargets(const IPlayerManager* p1,
                                      const IPlayerManager* p2)
{
    m_targetPlayer  = p1;
    m_targetPlayer2 = p2;
}

sf::View CameraManager::getView() const
{
    return m_view;
}
