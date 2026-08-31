#include "CameraManager.h"
#include "../../interfaces/IPlayerManager.h"

#include <algorithm>
#include <cmath>

CameraManager::CameraManager()
    : m_viewWidth(320.0f)
    , m_viewHeight(240.0f)
    , m_mapWidth(0.0f)
{
}

void CameraManager::initialize(sf::Vector2u mapSizePixels)
{
    // View cao ít nhất bằng chiều cao bản đồ để không cắt mất các hàng cuối
    // (vd map 16 tile × 16px = 256px > view mặc định 240px → hàng GROUND dưới
    //  cùng bị khuất). Khi map ngắn vẫn dùng m_viewHeight mặc định.
    float viewH = std::max(m_viewHeight, static_cast<float>(mapSizePixels.y));
    m_view.setSize(sf::Vector2f(m_viewWidth, viewH));
    m_view.setCenter(sf::Vector2f(m_viewWidth / 2.0f, viewH / 2.0f));

    m_mapWidth = static_cast<float>(mapSizePixels.x);
}

void CameraManager::update(float deltaTime)
{
    (void)deltaTime;
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

    // Deadzone offsets: Mario moves freely inside the zone; pushing forward moves the camera
    const float offsetLeft  = -80.0f;
    const float offsetRight =    0.0f;

    float deadLeft  = center.x + offsetLeft;
    float deadRight = center.x + offsetRight;

    if (targetCenterX < deadLeft)
        center.x = targetCenterX - offsetLeft;
    else if (targetCenterX > deadRight)
        center.x = targetCenterX - offsetRight;

    // Giữ view canh giữa theo chiều cao view thực tế → toàn bộ bản đồ hiển thị.
    center.y = m_view.getSize().y / 2.0f;

    float halfW = m_view.getSize().x / 2.0f;
    center.x = std::clamp(center.x, halfW, std::max(halfW, m_mapWidth - halfW));

    // Prevent subpixel texture bleeding by snapping the camera target to pixel boundaries.
    // Window: 800x600, View: 320x240 -> scale factor is 2.5x (snap increments of 1/2.5 = 0.4).
    static constexpr float VIEW_SCALE = 2.5f;
    center.x = std::round(center.x * VIEW_SCALE) / VIEW_SCALE;
    center.y = std::round(center.y * VIEW_SCALE) / VIEW_SCALE;

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
