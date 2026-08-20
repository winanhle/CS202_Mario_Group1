#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include "../ui/UINavigator.h"
#include <SFML/Graphics.hpp>

/**
 * @class CharacterSelectState
 * @brief Màn hình chọn nhân vật trước khi chơi.
 *
 * Player 1 dùng ← / → (hoặc A/D) để chuyển highlight,
 * Enter để xác nhận và chuyển sang ModeSelectState.
 * Escape để quay lại MenuState.
 */
class ISettingsManager;

class CharacterSelectState : public GameState
{
public:
    CharacterSelectState(std::shared_ptr<ISettingsManager> settings, bool loadSave);
    ~CharacterSelectState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void confirm();
    void refreshUI();

    GameConfig m_config;          // sẽ được truyền sang ModeSelectState
    UINavigator m_nav{2};         // 0 = Mario, 1 = Luigi

    sf::Font  m_font;
    bool      m_fontLoaded;

    // Nền + tiêu đề
    sf::Text m_titleText{m_font};

    // Thẻ nhân vật (Mario / Luigi)
    sf::RectangleShape m_card[2];
    sf::Text           m_charName[2]{{m_font}, {m_font}};
    sf::Text           m_charDesc[2]{{m_font}, {m_font}};

    // Sprite preview từ texture
    sf::Texture m_marioTexture;
    sf::Texture m_luigiTexture;
    sf::Sprite  m_preview[2];

    // Hướng dẫn bên dưới
    sf::Text m_hintText{m_font};

    // Nhấp nháy highlight
    float m_blinkTimer = 0.f;
    bool  m_blinkOn    = true;
    static constexpr float BLINK_INTERVAL = 0.5f;

    // Kích thước màn hình
    static constexpr float WIN_W = 800.f;
    static constexpr float WIN_H = 600.f;

    std::shared_ptr<ISettingsManager> m_settings;
    bool m_loadSave;
    mutable sf::Vector2u m_windowSize{800u, 600u};
};
