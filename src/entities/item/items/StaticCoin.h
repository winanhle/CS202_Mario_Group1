#pragma once

#include "../Item.h"
#include "../ItemSprite.h"
#include "../../../interfaces/IPlayerManager.h"

class StaticCoin : public Item
{
public:
    StaticCoin(float x, float y)
        : Item(x, y, *m_coinTextures[0]) {}

    void update(float deltaTime) override {
        m_animationTimer += deltaTime;
        while (m_animationTimer >= FRAME_DURATION) {
            m_animationTimer -= FRAME_DURATION;
            m_currentFrame = (m_currentFrame + 1) % 4;
            m_sprite.setTexture(*m_coinTextures[m_currentFrame], true);
        }
    }

    void OnInteract(IPlayerManager* player) override {
        player->collectCoin(1);
    }

    void move(float dt) override {};

    static void setCoinTextures(std::array<sf::Texture*, 4> textures) {
        m_coinTextures = textures;
    }

private:
    static constexpr float FRAME_DURATION = 1.2f;
    float m_animationTimer = 0.f;
    int m_currentFrame = 0;

    static std::array<sf::Texture*, 4> m_coinTextures;
};