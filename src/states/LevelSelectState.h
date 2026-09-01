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
class ISoundManager;

class LevelSelectState : public GameState
{
public:
    explicit LevelSelectState(
        const GameConfig& config,
        std::shared_ptr<ISettingsManager> settings,
        std::shared_ptr<ISaveManager> saveManager = nullptr,
        bool loadSave = false,
        std::shared_ptr<ISoundManager> soundManager = nullptr
    );
    ~LevelSelectState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    struct LevelItem {
        std::string path;
        std::string title;
        std::string subtitle;
        std::string tag;
        bool isCampaign = false;
    };

    struct StarParticle {
        float x = 0.0f;
        float y = 0.0f;
        float speed = 10.0f;
        float size = 1.5f;
        float phase = 0.0f;
    };

    void discoverMaps();
    void confirmSelection();
    void updateScrollWindow();
    sf::FloatRect getCardBounds(size_t visibleSlot) const;

    sf::Font m_font;
    bool m_fontLoaded = false;
    
    std::vector<LevelItem> m_levels;
    int m_selectedIndex = 0;
    size_t m_startIndex = 0;
    
    GameConfig m_config;
    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
    std::shared_ptr<ISoundManager> m_soundManager;
    bool m_loadSave = false;

    // Visual assets
    sf::Texture m_heroTexture;
    std::optional<sf::Sprite> m_heroSprite;
    bool m_heroLoaded = false;

    sf::Texture m_mushroomTexture;
    std::optional<sf::Sprite> m_mushroomSprite;
    bool m_mushroomLoaded = false;

    sf::Texture m_starTexture;
    std::optional<sf::Sprite> m_starSprite;
    bool m_starLoaded = false;

    std::vector<StarParticle> m_stars;
    float m_animTimer = 0.0f;
};
