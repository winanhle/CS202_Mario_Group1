#pragma once

#include "../../interfaces/IItemManager.h"
#include "Item.h"
#include <memory>
#include <vector>

class IPlayerManager;

namespace sf {
class RenderWindow;
}

class ItemManager : public IItemManager
{
private:
    IPlayerManager* m_player = nullptr;
    std::vector<std::unique_ptr<Item>> m_items;
    std::array<sf::Texture, 4> m_coinTextures;
    std::array<sf::Texture, 4> m_fireFlowerTextures;
    sf::Texture m_mushroomTexture;
    sf::Texture m_starTexture;
public:
    ItemManager();
    ~ItemManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getItemCount() const override;

    void setPlayerManager(IPlayerManager* player) override
    {
        m_player = player;
    }

    // ─── Spawn API (called by MapManager) ────────────────────────────────────
    void spawnCoinPop    (float worldX, float worldY) override;
    void spawnMushroom   (float worldX, float worldY) override;
    void spawnFireFlower (float worldX, float worldY) override;
};