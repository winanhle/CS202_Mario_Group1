#pragma once

#include "../Item.h"
#include "../../../interfaces/IPlayerManager.h"

class FireFlower : public Item
{
public:
    FireFlower(float x, float y)
        : Item(x, y)
    {
    }

    void OnInteract(IPlayerManager* player) override
    {
        player->collectCoin(1);
    }

    void render(sf::RenderWindow& window) const override
    {
        // TODO: draw fireflower sprite
        sf::CircleShape flower(8.f);
        flower.setFillColor(sf::Color(255, 140, 0));
        flower.setPosition(getPosition());
        window.draw(flower);
    }
};