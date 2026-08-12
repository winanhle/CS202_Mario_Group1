#pragma once

#include "../Item.h"
#include "../ItemSprite.h"
#include "../../../interfaces/IPlayerManager.h"

class Coin : public Item
{
private:
    std::array<sf::Texture*, 4> m_frames;
    float animationTimer = 0.f;
    std::size_t currentFrame = 0;
    static constexpr float FRAME_DURATION = 1.2f;
public:
    Coin(float x, float y,
        std::array<sf::Texture*, 4> frames): 
        Item(x, y, *frames[0]), m_frames(frames) {}

    void update(float deltaTime) override {
        animationTimer += deltaTime; 
        while (animationTimer >= FRAME_DURATION) {
            animationTimer -= FRAME_DURATION;
            currentFrame = (currentFrame + 1) % 4;
            m_sprite.setTexture(*m_frames[currentFrame], true);
        }
    }

    void OnInteract(IPlayerManager* player) override
    {
        player->collectCoin(1);
    }

    void move(float dt) override {};
};