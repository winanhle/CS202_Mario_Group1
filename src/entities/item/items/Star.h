#pragma once

#include "../Item.h"
#include "../ItemSprite.h"
#include "../../../interfaces/IPlayerManager.h"

class Star : public Item
{
private:
    static constexpr float BOUNCE_SPEED = 350.f; // tweak for a higher/lower bounce

public:
    Star(float x, float y, sf::Texture& texture)
        : Item(x, y, texture)
    {
        setVelocityY(-BOUNCE_SPEED); // start bouncing immediately on spawn
    }

    void onLanded() override
    {
        setVelocityY(-BOUNCE_SPEED);
    }

    void OnInteract(IPlayerManager* player) override
    {
        player->activateStar();
    }
};