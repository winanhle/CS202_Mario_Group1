#pragma once

#include "../interfaces/IItemManager.h"
#include <SFML/Graphics/Rect.hpp>
#include <vector>

class IPlayerManager;

namespace sf {
class RenderWindow;
}

/**
 * @class ItemManager
 * @brief Stub implementation of item management
 * 
 * Developer: Dinh Anh
 * Status: STUB - Replace with full implementation
 * 
 * This stub provides minimal functionality to keep the project compilable.
 * Dinh Anh will implement full item spawning and management logic here.
 */
class ItemManager : public IItemManager
{
    IPlayerManager* m_player = nullptr;

    struct Item {
        float x, y;
        int coinValue = 1;
        bool collected = false;
        sf::FloatRect getHitbox() const { return {{x, y}, {16.f, 16.f}}; }
    };
    std::vector<Item> m_items;

public:
    ItemManager();
    ~ItemManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getItemCount() const override;

    // ─── NHẬN DEPENDENCY ───
    void setPlayerManager(IPlayerManager* player) override { m_player = player; }
};
