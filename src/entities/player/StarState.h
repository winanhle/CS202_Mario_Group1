#pragma once

#include <SFML/Graphics.hpp>
#include <array>

/**
 * @class StarState
 * @brief Quản lý trạng thái bất tử sau khi Player nhặt Star.
 *
 * Tự quản lý:
 *  - Timer đếm ngược thời gian còn lại (10 giây).
 *  - Hiệu ứng visual: 4 hình chữ nhật màu đỏ/vàng/xanh dương/xanh lá,
 *    opacity 30%, bằng hitbox Player, chớp nháy liên tục.
 *
 * PlayerManager sở hữu một unique_ptr<StarState>.
 * Khi null → Player ở trạng thái bình thường.
 */
class StarState
{
public:
    static constexpr float DURATION   = 10.f;  // giây
    static constexpr float FLASH_RATE = 0.08f; // giây mỗi frame màu

    StarState() = default;

    /** Tick timer và chuyển màu flash. */
    void update(float dt)
    {
        if (m_timer <= 0.f) return;

        m_timer -= dt;
        if (m_timer < 0.f) m_timer = 0.f;

        m_flashTimer += dt;
        if (m_flashTimer >= FLASH_RATE)
        {
            m_flashTimer -= FLASH_RATE;
            m_colorIndex  = (m_colorIndex + 1) % static_cast<int>(COLORS.size());
        }
    }

    /** Trả về true nếu Star state còn hiệu lực. */
    bool isActive() const { return m_timer > 0.f; }

    /**
     * @brief Vẽ 4 hình chữ nhật chớp nháy overlay lên hitbox Player.
     *
     * Mỗi frame chỉ vẽ 1 màu theo thứ tự vòng tròn:
     * Đỏ → Vàng → Xanh dương → Xanh lá → lặp lại.
     * Opacity 30% (alpha = 76 / 255).
     */
    void render(sf::RenderWindow& window, const sf::FloatRect& hitbox) const
    {
        if (!isActive()) return;

        sf::RectangleShape rect(hitbox.size);
        rect.setPosition(hitbox.position);

        sf::Color color = COLORS[static_cast<std::size_t>(m_colorIndex)];
        color.a = 76; // 30% opacity  (255 * 0.30 ≈ 76)
        rect.setFillColor(color);
        rect.setOutlineThickness(0.f);

        window.draw(rect);
    }

private:
    float m_timer      = DURATION;
    float m_flashTimer = 0.f;
    int   m_colorIndex = 0;

    // 4 màu chớp nháy theo thứ tự
    static constexpr std::array<sf::Color, 4> COLORS = {
        sf::Color(220,  50,  50),  // Đỏ
        sf::Color(255, 215,   0),  // Vàng
        sf::Color( 30, 100, 255),  // Xanh dương
        sf::Color( 50, 200,  80),  // Xanh lá
    };
};
