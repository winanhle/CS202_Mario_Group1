#pragma once

#include "../Item.h"
#include "../ItemSprite.h"
#include "../../../interfaces/IPlayerManager.h"

class Star : public Item
{
public:
    Star(float x, float y, sf::Texture& texture)
        : Item(x, y, texture, ItemSprite::Star) {}

    void OnInteract(IPlayerManager* player) override
    {
        player->collectCoin(1);
    }

    void render(sf::RenderWindow& window) const override
    {
        window.draw(m_sprite);
    }
};