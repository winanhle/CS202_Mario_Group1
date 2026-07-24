#pragma once

#include "../Item.h"
#include "../../../interfaces/IPlayerManager.h"

class Mushroom : public Item
{
public:
    Mushroom(float x, float y)
        : Item(x, y)
    {
    }

    void OnInteract(IPlayerManager* player) override
    {
       player->collectCoin(1);
    }

    void render(sf::RenderWindow& window) const override
    {
        // TODO: draw mushroom sprite
        sf::CircleShape mushroom(8.f);
        mushroom.setFillColor(sf::Color::Red);
        mushroom.setPosition(getPosition());
        window.draw(mushroom);
    }
};