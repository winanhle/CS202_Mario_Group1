#pragma once

#include "../../interfaces/ILiftManager.h"
#include "Lift.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class IPlayerManager;

/**
 * @class LiftManager
 * @brief Manages all Lift platform instances for the current level.
 *
 * Responsibilities:
 *   - Load the shared Lift texture once on initialize()
 *   - Spawn Lift instances from TMX map data
 *   - Advance each lift's oscillation every frame
 *   - Detect when a player is standing on a lift and carry them with it
 *   - Render all lifts
 *
 * Player-carrying algorithm (per lift, per player):
 *   1. Check if the player's bottom edge is within RIDE_EPSILON pixels of
 *      the lift's top edge (Y overlap check).
 *   2. Check horizontal overlap — player's horizontal interval must share
 *      at least MIN_OVERLAP_RATIO of the player's own width with the lift.
 *   3. If both conditions are met the player is "riding":
 *      - Apply the lift's frame delta to the player via applyLiftOffset()
 *      - Snap the player's feet to the lift top (prevents slow drift-through)
 */
class LiftManager : public ILiftManager
{
public:
    LiftManager()  = default;
    ~LiftManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    void setPlayerManager(IPlayerManager* player)  override { m_player  = player; }
    void setPlayerManager2(IPlayerManager* player) override { m_player2 = player; }

    void spawnFromMapData(const std::vector<LiftSpawnData>& spawns) override;

    void resolveCollisionX(float oldX, float& newX, float posY,
                           const sf::Vector2f& playerSize, float& velX) const override;

    bool resolveCollisionY(float& posX, float oldY, float& newY,
                           const sf::Vector2f& playerSize, float& velY,
                           bool& isGrounded, bool& isJumping) const override;

private:
    IPlayerManager* m_player  = nullptr;
    IPlayerManager* m_player2 = nullptr;

    sf::Texture m_liftTexture;
    std::vector<std::unique_ptr<Lift>> m_lifts;
};
