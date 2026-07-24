#pragma once

#include "../Item.h"
#include "../../../interfaces/IPlayerManager.h"

class Coin : public Item
{
public:
    Coin(float x, float y)
        : Item(x, y)
    {
    }

    void OnInteract(IPlayerManager* player) override
    {
        player->collectCoin(1);
    }

    void render(sf::RenderWindow& window) const override
    {
        // TODO: draw coin sprite
        sf::CircleShape coin(8.f);
        coin.setFillColor(sf::Color::Yellow);
        coin.setPosition(getPosition());
        window.draw(coin);
    }
};