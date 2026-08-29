#pragma once

#include "../entities/map/MapData.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

class IPlayerManager;

namespace sf {
class RenderWindow;
}

/**
 * @interface ILiftManager
 * @brief Interface for the Lift platform manager.
 *
 * Lift platforms oscillate automatically and carry the Hero when
 * he stands on top.  The manager owns all Lift instances and
 * resolves player-riding each frame.
 */
class ILiftManager
{
public:
    virtual ~ILiftManager() = default;

    /** Load textures. Called once by GameWorld::initialize(). */
    virtual void initialize() = 0;

    /**
     * @brief Advance oscillation and carry any riding player(s).
     * Must be called AFTER player update so positions are final,
     * and BEFORE enemy update so enemies see the correct player hitbox.
     */
    virtual void update(float deltaTime) = 0;

    /** Render all lift sprites. */
    virtual void render(sf::RenderWindow& window) const = 0;

    /** Inject Player 1 (always present). */
    virtual void setPlayerManager(IPlayerManager* player) = 0;

    /** Inject Player 2 (2-player mode only; may be nullptr). */
    virtual void setPlayerManager2(IPlayerManager* player) = 0;

    /**
     * @brief Clear existing lifts and spawn new ones from map data.
     * Called by GameWorld::loadCurrentLevel().
     */
    virtual void spawnFromMapData(const std::vector<LiftSpawnData>& spawns) = 0;

    /**
     * @brief Resolve horizontal side collision with lift platforms.
     * @param oldX       Current world X
     * @param newX       Target world X (clamped if collision occurs)
     * @param posY       Current world Y
     * @param playerSize Player hitbox dimensions
     * @param velX       Player horizontal velocity (zeroed if blocked)
     */
    virtual void resolveCollisionX(float oldX, float& newX, float posY,
                                  const sf::Vector2f& playerSize, float& velX) const = 0;

    /**
     * @brief Resolve vertical landing, riding, and head-bump collision with lift platforms.
     * @param posX       Player world X (adjusted with platform delta X if riding)
     * @param oldY       Current world Y
     * @param newY       Target world Y (snapped to platform top/bottom if collision occurs)
     * @param playerSize Player hitbox dimensions
     * @param velY       Player vertical velocity (zeroed if landed or head bumped)
     * @param isGrounded Set to true if player lands on a lift
     * @param isJumping  Set to false if player lands on a lift
     * @return True if player is landed on or collided with a lift
     */
    virtual bool resolveCollisionY(float& posX, float oldY, float& newY,
                                  const sf::Vector2f& playerSize, float& velY,
                                  bool& isGrounded, bool& isJumping) const = 0;
};
