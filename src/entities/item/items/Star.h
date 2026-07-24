#pragma once

#include "../Item.h"
#include "../../../interfaces/IPlayerManager.h"

class Star : public Item
{
public:
    Star(float x, float y)
        : Item(x, y)
    {
    }

    void OnInteract(IPlayerManager* player) override
    {
        player->collectCoin(1);
    }

    void render(sf::RenderWindow& window) const override
    {
        // TODO: draw star sprite
        sf::CircleShape star(8.f);
        star.setFillColor(sf::Color::Cyan);
        star.setPosition(getPosition());
        window.draw(star);
    }
};