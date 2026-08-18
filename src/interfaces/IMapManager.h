#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include "../entities/map/MapData.h"

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
     * @brief Query the logical tile type at a world pixel coordinate.
     * Out-of-bounds pixels are treated as GROUND.
     */
    virtual TileType getTileType(float x, float y) const = 0;

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

    /**
     * @brief Load a map from a TMX file path.
     * Replaces any previously loaded map data.
     * @param tmxPath Path to the .tmx file
     */
    virtual void loadMap(const std::string& tmxPath) = 0;

    /**
     * @brief Get parsed object layer data (enemy spawns, player spawn)
     * from the last loaded map.
     * @return Reference to the parsed MapObjectData
     */
    virtual const MapObjectData& getMapObjectData() const = 0;
};