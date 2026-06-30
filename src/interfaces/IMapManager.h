#pragma once

#include <SFML/System/Vector2.hpp>

namespace sf {
class RenderWindow;
class Event;
}

class IMapManager {
public:
    virtual ~IMapManager() = default;
    virtual void initialize() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) const = 0;
    
    virtual bool isSolid(float x, float y) const = 0;
    virtual int getTileSize() const = 0;
    virtual sf::Vector2u getMapPixelSize() const = 0;
};