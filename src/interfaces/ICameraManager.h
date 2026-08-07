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

    virtual void setFollowTarget(const IPlayerManager* player) = 0;

    virtual sf::View getView() const = 0;
};
