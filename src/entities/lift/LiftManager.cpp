#include "LiftManager.h"
#include "../../interfaces/IPlayerManager.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

// =============================================================================
//  INITIALIZE
// =============================================================================

void LiftManager::initialize()
{
    m_lifts.clear();
    if (!m_liftTexture.loadFromFile("assets/texture/item/Lift.png"))
        throw std::runtime_error("[LiftManager] Failed to load assets/texture/item/Lift.png");
    std::cout << "[LiftManager] Initialized." << std::endl;
}

// =============================================================================
//  SPAWN
// =============================================================================

void LiftManager::spawnFromMapData(const std::vector<LiftSpawnData>& spawns)
{
    m_lifts.clear();
    for (const auto& data : spawns)
    {
        Lift::MotionType motion =
            (data.motionType == "leftright")
                ? Lift::MotionType::LeftRight
                : Lift::MotionType::UpDown;

        int holes = (data.holes == 6) ? 6 : 4;

        m_lifts.push_back(std::make_unique<Lift>(
            data.x, data.y,
            motion, holes,
            m_liftTexture,
            data.range, data.speed));
    }
    std::cout << "[LiftManager] Spawned " << m_lifts.size()
              << " lift platforms from map data." << std::endl;
}

// =============================================================================
//  UPDATE
// =============================================================================

void LiftManager::update(float deltaTime)
{
    for (auto& lift : m_lifts)
    {
        lift->update(deltaTime);
    }
}

// =============================================================================
//  COLLISION RESOLUTION (CALLED BY PLAYER DURING PHYSICS STEPS)
// =============================================================================

void LiftManager::resolveCollisionX(float oldX, float& newX, float posY,
                                    const sf::Vector2f& playerSize, float& velX) const
{
    float playerTop = posY;
    float playerBottom = posY + playerSize.y;

    for (const auto& lift : m_lifts)
    {
        sf::FloatRect liftBox = lift->getHitbox();
        float liftTop = liftBox.position.y;
        float liftBottom = liftBox.position.y + liftBox.size.y;
        float liftLeft = liftBox.position.x;
        float liftRight = liftBox.position.x + liftBox.size.x;

        // Side collision only applies if Mario vertically overlaps the height of the lift,
        // and is not standing on top of it (give 4px tolerance from top)
        if (playerBottom <= liftTop + 4.f || playerTop >= liftBottom - 2.f)
            continue;

        // Moving right into the lift's left side
        if (velX > 0.f || (newX + playerSize.x > liftLeft && oldX + playerSize.x <= liftLeft + 4.f))
        {
            if (newX + playerSize.x > liftLeft && oldX + playerSize.x <= liftLeft + 8.f)
            {
                newX = liftLeft - playerSize.x;
                velX = 0.f;
            }
        }
        // Moving left into the lift's right side
        else if (velX < 0.f || (newX < liftRight && oldX >= liftRight - 4.f))
        {
            if (newX < liftRight && oldX >= liftRight - 8.f)
            {
                newX = liftRight;
                velX = 0.f;
            }
        }
    }
}

bool LiftManager::resolveCollisionY(float& posX, float oldY, float& newY,
                                    const sf::Vector2f& playerSize, float& velY,
                                    bool& isGrounded, bool& isJumping) const
{
    float playerLeft = posX;
    float playerRight = posX + playerSize.x;
    float oldBottom = oldY + playerSize.y;
    float newBottom = newY + playerSize.y;
    float oldTop = oldY;
    float newTop = newY;

    for (const auto& lift : m_lifts)
    {
        sf::FloatRect liftBox = lift->getHitbox();
        float liftTop = liftBox.position.y;
        float liftBottom = liftBox.position.y + liftBox.size.y;
        float liftLeft = liftBox.position.x;
        float liftRight = liftBox.position.x + liftBox.size.x;

        // Horizontal overlap
        float overlapLeft = std::max(playerLeft, liftLeft);
        float overlapRight = std::min(playerRight, liftRight);
        float overlap = overlapRight - overlapLeft;

        // Require at least a small horizontal overlap (4px or > 0)
        if (overlap < 4.f)
            continue;

        // 1. Landing / Standing on top of the Lift
        // Only if player is not jumping upward (or upward velocity has ceased)
        if (!isJumping || velY >= 0.f)
        {
            // Player feet were above/near lift top last frame AND reach/cross lift top this frame
            if (oldBottom <= liftTop + 8.f && newBottom >= liftTop - 4.f)
            {
                newY = liftTop - playerSize.y;
                velY = 0.f;
                isGrounded = true;
                isJumping = false;

                // Carry player with lift movement
                sf::Vector2f delta = lift->getDelta();
                posX += delta.x;
                newY += delta.y;

                return true;
            }
        }

        // 2. Head bump into underside of the Lift
        if (velY < 0.f)
        {
            if (oldTop >= liftBottom - 6.f && newTop <= liftBottom)
            {
                newY = liftBottom;
                velY = 0.f;
                return true;
            }
        }
    }

    return false;
}

// =============================================================================
//  RENDER
// =============================================================================

void LiftManager::render(sf::RenderWindow& window) const
{
    for (const auto& lift : m_lifts)
        lift->render(window);
}

