#pragma once
#include <SFML/Graphics/Rect.hpp>

/**
 * @namespace FireballSprite
 * @brief Sprite sheet ngang: 4 fly frame (8x8) + 3 explode frame (16x16).
 *
 * Pixel offsets là placeholder — cần đo từ sprite sheet thực tế sau khi có asset.
 */
namespace FireballSprite
{
    constexpr const char* Sheet = "assets/texture/hero/mario.png";

    constexpr sf::IntRect FlyFrames[4] = {
        {{2, 95}, {8, 8}},
        {{12, 95}, {8, 8}},
        {{22, 95}, {8, 8}},
        {{32, 95}, {8, 8}},
    };

    constexpr sf::IntRect ExplodeFrames[3] = {
        {{6, 110}, {8, 8}},
        {{21, 107}, {14, 14}},
        {{38, 106}, {16, 16}},
    };
}