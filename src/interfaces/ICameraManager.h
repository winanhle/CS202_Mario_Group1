#pragma once

#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

class IPlayerManager;

class ICameraManager
{
public:
    virtual ~ICameraManager() = default;

    virtual void initialize(sf::Vector2u mapSizePixels) = 0;

    virtual void update(float deltaTime) = 0;

    /**
     * @brief Follow 1 player (single player mode).
     * Backward-compatible — gọi setFollowTargets(player, nullptr) bên dưới.
     */
    virtual void setFollowTarget(const IPlayerManager* player) = 0;

    /**
     * @brief Follow điểm giữa 2 player (2-player mode).
     * Nếu p2 = nullptr, hành vi giống setFollowTarget(p1).
     */
    virtual void setFollowTargets(const IPlayerManager* p1,
                                   const IPlayerManager* p2) = 0;

    virtual sf::View getView() const = 0;
};
