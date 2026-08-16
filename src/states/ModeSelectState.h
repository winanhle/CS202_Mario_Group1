#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include <SFML/Graphics.hpp>

/**
 * @class ModeSelectState
 * @brief Màn hình chọn chế độ chơi (1 Player / 2 Players).
 *
 * Nhận GameConfig từ CharacterSelectState (nhân vật đã chọn).
 * ← / → để chọn mode, Enter để xác nhận và bắt đầu game.
 * Escape để quay lại CharacterSelectState.
 */
class ISettingsManager;

class ModeSelectState : public GameState
{
public:
    explicit ModeSelectState(const GameConfig& config, std::shared_ptr<ISettingsManager> settings, bool loadSave);
    ~ModeSelectState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void confirm();
    void refreshUI();

    GameConfig m_config;       // nhận từ CharacterSelectState, hoàn chỉnh ở đây
    int        m_selectedIndex; // 0 = 1 Player, 1 = 2 Players

    sf::Font m_font;
    bool     m_fontLoaded;

    sf::Text m_titleText{m_font};
    sf::Text m_subtitleText{m_font};  // hiển thị nhân vật đã chọn

    sf::RectangleShape m_card[2];
    sf::Text           m_modeName[2]{{m_font}, {m_font}};
    sf::Text           m_modeDesc[2]{{m_font}, {m_font}};

    sf::Text m_hintText{m_font};

    // Nhấp nháy
    float m_blinkTimer = 0.f;
    bool  m_blinkOn    = true;
    static constexpr float BLINK_INTERVAL = 0.5f;

    static constexpr float WIN_W = 800.f;
    static constexpr float WIN_H = 600.f;

    std::shared_ptr<ISettingsManager> m_settings;
    bool m_loadSave;
};
