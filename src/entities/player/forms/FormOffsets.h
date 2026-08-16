#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

/**
 * @struct FormFrames
 * @brief Chứa tọa độ sprite và kích thước hitbox cho một dạng (form) của nhân vật.
 *
 * Mỗi nhân vật (Mario, Luigi, ...) khai báo riêng tọa độ pixel phù hợp
 * với sprite sheet của mình, sau đó truyền vào constructor của các Form class.
 */
struct FormFrames {
    sf::IntRect  walkFrame1;
    sf::IntRect  walkFrame2;
    sf::IntRect  jumpFrame;
    sf::Vector2f hitboxSize;
};

/**
 * @struct CharacterFormOffsets
 * @brief Tập hợp FormFrames cho cả 3 dạng biến hình của một nhân vật.
 *
 * Được khai báo dưới dạng static const trong từng subclass (Mario.cpp, Luigi.cpp, ...).
 * Khi form evolve() hoặc takeDamage() tạo form mới, con trỏ đến bộ offsets này
 * được truyền tiếp để đảm bảo nhân vật luôn dùng đúng bộ sprite của mình.
 */
struct CharacterFormOffsets {
    FormFrames normalForm;
    FormFrames superForm;
    FormFrames fireForm;
};
