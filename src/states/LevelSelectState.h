#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include <optional>

class ISettingsManager;
class ISaveManager;

class LevelSelectState : public GameState
{
public:
    explicit LevelSelectState(const GameConfig& config,
                              std::shared_ptr<ISettingsManager> settings,
                              std::shared_ptr<ISaveManager> saveManager = nullptr,
                              bool loadSave = false);
    ~LevelSelectState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    struct LevelItem {
        std::string path;
        std::string title;
        bool isLocked = false;
        int stageNum = 0; // 1, 2, 3, or 0 (editor)
    };

    struct StarParticle {
        float x = 0.0f;
        float y = 0.0f;
        float speed = 10.0f;
        float size = 1.5f;
        float phase = 0.0f;
    };

    void initLevels();
    void confirmSelection();
    sf::FloatRect getBoxBounds(size_t index) const;
    sf::Vector2f toViewCoords(sf::Vector2i pixelPos) const;

    sf::Font m_font;
    bool m_fontLoaded = false;
    
    std::vector<LevelItem> m_levels;
    int m_selectedIndex = 0;
    
    mutable sf::Vector2u m_windowSize{ 800, 600 };
    GameConfig m_config;
    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
    bool m_loadSave = false;

    // Stage preview textures
    sf::Texture m_texStage1;
    sf::Texture m_texStage2;
    sf::Texture m_texStage3;
    bool m_tex1Loaded = false;
    bool m_tex2Loaded = false;
    bool m_tex3Loaded = false;

    std::vector<StarParticle> m_stars;
    float m_animTimer = 0.0f;
};

