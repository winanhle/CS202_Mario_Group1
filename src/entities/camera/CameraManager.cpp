#include "CameraManager.h"
#include "../../interfaces/IPlayerManager.h"

#include <algorithm>
#include <cmath>

CameraManager::CameraManager()
    : m_targetPlayer(nullptr)
    , m_viewWidth(320.0f)
    , m_viewHeight(240.0f)
    , m_lerpSpeedX(5.0f)
    , m_lerpSpeedY(3.0f)
    , m_mapWidth(0.0f)
{
}

void CameraManager::initialize(sf::Vector2u mapSizePixels)
{
    m_view.setSize(sf::Vector2f(m_viewWidth, m_viewHeight));
    m_view.setCenter(sf::Vector2f(m_viewWidth / 2.0f, m_viewHeight / 2.0f));

    m_mapWidth = static_cast<float>(mapSizePixels.x);
    m_deadzoneWidth = m_viewWidth / 2.0f - 5.0f;
}

void CameraManager::update(float deltaTime)
{
    if (!m_targetPlayer)
        return;

    sf::FloatRect box = m_targetPlayer->getHitbox();
    float playerCenterX = box.position.x + box.size.x / 2.0f;
    float playerCenterY = box.position.y + box.size.y / 2.0f;

    sf::Vector2f center = m_view.getCenter();

    float offsetLeft = -100.0f;
    float offsetRight = 0.0f;

    float deadLeft = center.x + offsetLeft;
    float deadRight = center.x + offsetRight;

    float targetX = center.x;
    if (playerCenterX < deadLeft)
        targetX = playerCenterX - offsetLeft;
    else if (playerCenterX > deadRight)
        targetX = playerCenterX - offsetRight + 40.0f;

    float targetY = m_viewHeight / 2.0f;

    float alphaX = 1.0f - std::exp(-m_lerpSpeedX * deltaTime);
    float alphaY = 1.0f - std::exp(-m_lerpSpeedY * deltaTime);
    center.x += (targetX - center.x) * alphaX;
    center.y += (targetY - center.y) * alphaY;

    float halfW = m_view.getSize().x / 2.0f;
    center.x = std::clamp(center.x, halfW, m_mapWidth - halfW);

    m_view.setCenter(center);
}

void CameraManager::setFollowTarget(const IPlayerManager* player)
{
    m_targetPlayer = player;
}

sf::View CameraManager::getView() const
{
    return m_view;
}
