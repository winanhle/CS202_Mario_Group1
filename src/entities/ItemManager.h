#pragma once

#include "../interfaces/IItemManager.h"

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
public:
    ItemManager();
    ~ItemManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getItemCount() const override;

private:
    int m_itemCount;
};
