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
        std::string subtitle;
        std::string tag;
        bool isCampaign = false;
        bool isLocked = false;
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
    sf::FloatRect getBoxBounds(size_t slotOnPage) const;
    sf::Vector2f toViewCoords(sf::Vector2i pixelPos) const;

    sf::Font m_font;
    bool m_fontLoaded = false;
    
    std::vector<LevelItem> m_levels;
    int m_selectedIndex = 0;
    size_t m_currentPage = 0;
    static constexpr size_t PAGE_SIZE = 6;
    
    mutable sf::Vector2u m_windowSize{ 800, 600 };
    GameConfig m_config;
    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
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
