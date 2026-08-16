#pragma once

#include <SFML/System/Vector2.hpp>

class IItemManager; // forward declaration

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

    /**
     * @brief Direction-aware solidity cho trường hợp player đập tile từ dưới lên.
     * Mọi block thường trả cùng kết quả với isSolid(); riêng HIDDEN_BLOCK trả true
     * (bump được) dù isSolid() là false (Mario xuyên qua từ trái/phải/trên).
     */
    virtual bool isSolidFromBelow(float x, float y) const = 0;

    virtual int getTileSize() const = 0;
    virtual sf::Vector2u getMapPixelSize() const = 0;

    /**
     * @brief Called by PlayerManager when the player bumps a tile from below
     *        (upward velocity collides with the bottom face of a tile).
     * @param tileGridX  Grid column of the hit tile
     * @param tileGridY  Grid row of the hit tile
     * @param formType   Current form of the player (0=Normal, 1=Super, 2=Fire)
     */
    virtual void onHitFromBelow(int tileGridX, int tileGridY, int formType) = 0;

    /**
     * @brief Inject ItemManager so MapManager can spawn coins/power-ups.
     * Called by GameWorld::injectDependencies().
     */
    virtual void setItemManager(IItemManager* itemManager) = 0;
};