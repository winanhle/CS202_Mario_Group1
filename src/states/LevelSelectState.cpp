#include "LevelSelectState.h"
#include "PlayState.h"
#include "ModeSelectState.h"
#include "../core/StateManager.h"
#include "../interfaces/ISoundManager.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace fs = std::filesystem;

namespace {
    static constexpr const char* FONT_PATH = "assets/fonts/SuperMario256.ttf";
    static constexpr const char* MAPS_DIR = "assets/map";

    static constexpr float WIN_W = 800.0f;
    static constexpr float WIN_H = 600.0f;

    static constexpr size_t VISIBLE_CARDS = 6;
    static constexpr float CARD_X = 60.0f;
    static constexpr float CARD_W = 670.0f;
    static constexpr float CARD_H = 52.0f;
    static constexpr float CARD_START_Y = 135.0f;
    static constexpr float CARD_GAP = 10.0f;

    static constexpr sf::Color BG_TOP(18, 28, 62);
    static constexpr sf::Color BG_BOT(8, 12, 28);
    static constexpr sf::Color PANEL_BG(16, 22, 44, 230);
    static constexpr sf::Color PANEL_OUTLINE(65, 105, 185, 200);

    static constexpr sf::Color CARD_IDLE_BG(26, 36, 68, 180);
    static constexpr sf::Color CARD_IDLE_OUTLINE(45, 65, 115);
    static constexpr sf::Color CARD_SEL_BG(42, 90, 195, 230);
    static constexpr sf::Color CARD_SEL_OUTLINE(140, 215, 255);

    static constexpr sf::Color TAG_CAMPAIGN_BG(200, 140, 30, 220);
    static constexpr sf::Color TAG_CUSTOM_BG(45, 120, 190, 200);
    static constexpr sf::Color TAG_BONUS_BG(140, 45, 190, 200);
}

LevelSelectState::LevelSelectState(
    const GameConfig& config,
    std::shared_ptr<ISettingsManager> settings,
    std::shared_ptr<ISaveManager> saveManager,
    bool loadSave,
    std::shared_ptr<ISoundManager> soundManager
)
    : m_config(config),
      m_settings(std::move(settings)),
      m_saveManager(std::move(saveManager)),
      m_soundManager(std::move(soundManager)),
      m_loadSave(loadSave)
{
    if (m_font.openFromFile(FONT_PATH)) {
        m_fontLoaded = true;
    } else {
        std::cerr << "[LevelSelectState] ERROR: Failed to load font: " << FONT_PATH << std::endl;
    }

    // Load hero sprite preview
    const char* heroPath = (m_config.player1Character == CharacterType::Luigi)
        ? "assets/texture/hero/luigi.png"
        : "assets/texture/hero/mario.png";
    if (m_heroTexture.loadFromFile(heroPath)) {
        m_heroLoaded = true;
        m_heroSprite.emplace(m_heroTexture);
        if (m_config.player1Character == CharacterType::Luigi) {
            m_heroSprite->setTextureRect(sf::IntRect({154, 29}, {16, 29}));
        } else {
            m_heroSprite->setTextureRect(sf::IntRect({154, 30}, {16, 28}));
        }
        m_heroSprite->setScale({1.4f, 1.4f});
    }

    // Load mushroom and star icons
    if (m_mushroomTexture.loadFromFile("assets/texture/item/Mushroom.PNG")) {
        m_mushroomLoaded = true;
        m_mushroomSprite.emplace(m_mushroomTexture);
        m_mushroomSprite->setScale({0.55f, 0.55f});
    }

    if (m_starTexture.loadFromFile("assets/texture/item/Star.PNG")) {
        m_starLoaded = true;
        m_starSprite.emplace(m_starTexture);
        m_starSprite->setScale({0.55f, 0.55f});
    }

    // Initialize background starfield particles
    m_stars.resize(40);
    for (size_t i = 0; i < m_stars.size(); ++i) {
        m_stars[i].x = static_cast<float>(std::rand() % 800);
        m_stars[i].y = static_cast<float>(std::rand() % 600);
        m_stars[i].speed = 5.0f + static_cast<float>(std::rand() % 15);
        m_stars[i].size = 1.0f + static_cast<float>(std::rand() % 20) / 10.0f;
        m_stars[i].phase = static_cast<float>(std::rand() % 628) / 100.0f;
    }

    discoverMaps();
}

void LevelSelectState::discoverMaps()
{
    m_levels.clear();

    // 1. Official Campaign Entry
    m_levels.push_back({
        "",
        "[CAMPAIGN] WORLD 1-1",
        "Classic adventure mode with seamless stage-to-stage progression",
        "STORY",
        true
    });

    if (!fs::exists(MAPS_DIR)) {
        std::cerr << "[LevelSelectState] WARNING: Directory does not exist: " << MAPS_DIR << std::endl;
        return;
    }

    std::vector<std::string> discovered;
    for (const auto& entry : fs::recursive_directory_iterator(MAPS_DIR)) {
        if (entry.path().extension() == ".tmx") {
            discovered.push_back(entry.path().string());
        }
    }

    std::sort(discovered.begin(), discovered.end());

    for (const auto& p : discovered) {
        std::string filename = fs::path(p).stem().string();
        std::string title = filename;
        std::string subtitle = "Custom map: " + fs::path(p).filename().string();
        std::string tag = "MAP";

        if (filename == "stage1") {
            title = "STAGE 1 - GRASSLAND (WORLD 1-1)";
            subtitle = "Overworld grassland with pipes, goombas, and mystery blocks";
            tag = "WORLD 1-1";
        } else if (filename == "stage1_hidden") {
            title = "STAGE 1 - BONUS / COIN VAULT";
            subtitle = "Underground secret room accessible via subterranean pipe";
            tag = "BONUS";
        } else if (filename == "stage2") {
            title = "STAGE 2 - UNDERGROUND (WORLD 1-2)";
            subtitle = "Subterranean cavern with moving lifts, koopas, and hazards";
            tag = "WORLD 1-2";
        } else if (filename == "stage3") {
            title = "STAGE 3 - ATHLETIC / CASTLE (WORLD 1-3)";
            subtitle = "Platforming heights, fire bars, and castle battlements";
            tag = "WORLD 1-3";
        } else if (filename == "map_test") {
            title = "TEST MAP - SANDBOX PLAYGROUND";
            subtitle = "Mechanics testing sandbox environment";
            tag = "DEBUG";
        }

        m_levels.push_back({ p, title, subtitle, tag, false });
    }
}

void LevelSelectState::confirmSelection()
{
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_levels.size())) {
        m_config.customMapPath = m_levels[m_selectedIndex].path;
        if (auto* mgr = getStateManager()) {
            mgr->changeState(std::make_unique<PlayState>(m_config, m_settings, m_saveManager, m_loadSave, m_soundManager));
        }
    }
}

void LevelSelectState::updateScrollWindow()
{
    if (m_levels.size() <= VISIBLE_CARDS) {
        m_startIndex = 0;
        return;
    }
    if (m_selectedIndex < static_cast<int>(m_startIndex)) {
        m_startIndex = static_cast<size_t>(m_selectedIndex);
    } else if (m_selectedIndex >= static_cast<int>(m_startIndex + VISIBLE_CARDS)) {
        m_startIndex = static_cast<size_t>(m_selectedIndex - VISIBLE_CARDS + 1);
    }
    if (m_startIndex + VISIBLE_CARDS > m_levels.size()) {
        m_startIndex = m_levels.size() - VISIBLE_CARDS;
    }
}

sf::FloatRect LevelSelectState::getCardBounds(size_t visibleSlot) const
{
    float yPos = CARD_START_Y + static_cast<float>(visibleSlot) * (CARD_H + CARD_GAP);
    return sf::FloatRect({CARD_X, yPos}, {CARD_W, CARD_H});
}

void LevelSelectState::handleInput(const sf::Event& event)
{
    // Keyboard navigation
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->code == sf::Keyboard::Key::W || keyEvent->code == sf::Keyboard::Key::Up) {
            if (!m_levels.empty()) {
                m_selectedIndex--;
                if (m_selectedIndex < 0)
                    m_selectedIndex = static_cast<int>(m_levels.size()) - 1;
                updateScrollWindow();
            }
        } else if (keyEvent->code == sf::Keyboard::Key::S || keyEvent->code == sf::Keyboard::Key::Down) {
            if (!m_levels.empty()) {
                m_selectedIndex++;
                if (m_selectedIndex >= static_cast<int>(m_levels.size()))
                    m_selectedIndex = 0;
                updateScrollWindow();
            }
        } else if (keyEvent->code == sf::Keyboard::Key::Enter || keyEvent->code == sf::Keyboard::Key::Space) {
            confirmSelection();
        } else if (keyEvent->code == sf::Keyboard::Key::Escape) {
            if (auto* mgr = getStateManager())
                mgr->changeState(std::make_unique<ModeSelectState>(m_config, m_settings, m_saveManager, m_loadSave, m_soundManager));
        }
    }

    // Mouse hover and click support (using stable hysteresis scrolling)
    size_t endIndex = std::min(m_startIndex + VISIBLE_CARDS, m_levels.size());

    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        float mx = static_cast<float>(mouseMoved->position.x);
        float my = static_cast<float>(mouseMoved->position.y);

        for (size_t i = m_startIndex; i < endIndex; ++i) {
            sf::FloatRect bounds = getCardBounds(i - m_startIndex);
            if (bounds.contains({mx, my})) {
                m_selectedIndex = static_cast<int>(i);
                break;
            }
        }
    } else if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            float mx = static_cast<float>(mousePressed->position.x);
            float my = static_cast<float>(mousePressed->position.y);

            for (size_t i = m_startIndex; i < endIndex; ++i) {
                sf::FloatRect bounds = getCardBounds(i - m_startIndex);
                if (bounds.contains({mx, my})) {
                    m_selectedIndex = static_cast<int>(i);
                    confirmSelection();
                    break;
                }
            }
        }
    }
}

void LevelSelectState::update(float deltaTime)
{
    m_animTimer += deltaTime;

    for (auto& star : m_stars) {
        star.y += star.speed * deltaTime;
        if (star.y > WIN_H) {
            star.y = 0.0f;
            star.x = static_cast<float>(std::rand() % static_cast<int>(WIN_W));
        }
    }
}

void LevelSelectState::render(sf::RenderWindow& window) const
{
    window.setView(window.getDefaultView());

    // 1. Draw dynamic midnight gradient background
    sf::VertexArray bg(sf::PrimitiveType::TriangleStrip, 4);
    bg[0].position = {0.f, 0.f};
    bg[0].color = BG_TOP;
    bg[1].position = {WIN_W, 0.f};
    bg[1].color = BG_TOP;
    bg[2].position = {0.f, WIN_H};
    bg[2].color = BG_BOT;
    bg[3].position = {WIN_W, WIN_H};
    bg[3].color = BG_BOT;
    window.draw(bg);

    // 2. Draw star particles
    for (const auto& star : m_stars) {
        float alpha = 130.0f + 100.0f * std::sin(m_animTimer * 2.5f + star.phase);
        alpha = std::clamp(alpha, 30.0f, 255.0f);
        sf::CircleShape starShape(star.size);
        starShape.setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
        starShape.setPosition({star.x, star.y});
        window.draw(starShape);
    }

    if (!m_fontLoaded) return;

    // 3. Draw Header Title
    sf::Text title(m_font, "SELECT STAGE", 26);
    title.setFillColor(sf::Color(255, 225, 60));
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(2.5f);
    title.setPosition({WIN_W / 2.f - title.getGlobalBounds().size.x / 2.f, 25.0f});
    window.draw(title);

    // 4. Draw Selected Configuration Capsule Badge
    const float BADGE_W = 340.0f;
    const float BADGE_H = 34.0f;
    const float BADGE_X = WIN_W / 2.f - BADGE_W / 2.f;
    const float BADGE_Y = 68.0f;

    sf::RectangleShape badge({BADGE_W, BADGE_H});
    badge.setFillColor(sf::Color(25, 38, 75, 200));
    badge.setOutlineColor(sf::Color(80, 140, 240, 180));
    badge.setOutlineThickness(1.5f);
    badge.setPosition({BADGE_X, BADGE_Y});
    window.draw(badge);

    if (m_heroLoaded && m_heroSprite.has_value()) {
        sf::RenderStates states;
        states.transform.translate({BADGE_X + 10.0f, BADGE_Y - 2.0f});
        window.draw(m_heroSprite.value(), states);
    }

    std::string configText = (m_config.player1Character == CharacterType::Luigi ? "LUIGI" : "MARIO");
    configText += "  |  ";
    configText += (m_config.mode == GameMode::TwoPlayer ? "2 PLAYERS" : "1 PLAYER");
    sf::Text cfg(m_font, configText, 11);
    cfg.setFillColor(sf::Color(200, 230, 255));
    cfg.setOutlineColor(sf::Color::Black);
    cfg.setOutlineThickness(1.0f);
    cfg.setPosition({BADGE_X + 46.0f, BADGE_Y + 9.0f});
    window.draw(cfg);

    // 5. Draw Main Stage Browser Bezel Panel
    const float PANEL_X = 40.0f;
    const float PANEL_Y = 115.0f;
    const float PANEL_W = 720.0f;
    const float PANEL_H = 410.0f;

    sf::RectangleShape panel({PANEL_W, PANEL_H});
    panel.setFillColor(PANEL_BG);
    panel.setOutlineColor(PANEL_OUTLINE);
    panel.setOutlineThickness(2.0f);
    panel.setPosition({PANEL_X, PANEL_Y});
    window.draw(panel);

    if (m_levels.empty()) {
        sf::Text noMaps(m_font, "NO MAPS AVAILABLE", 16);
        noMaps.setFillColor(sf::Color::Red);
        noMaps.setPosition({WIN_W / 2.f - noMaps.getGlobalBounds().size.x / 2.f, 250.0f});
        window.draw(noMaps);
        return;
    }

    // 6. Calculate Visible Card Window
    size_t totalItems = m_levels.size();
    size_t endIndex = std::min(m_startIndex + VISIBLE_CARDS, totalItems);

    // Up scroll hint
    if (m_startIndex > 0) {
        sf::Text upHint(m_font, "^ MORE STAGES ABOVE ^", 10);
        float pulse = 180.0f + 70.0f * std::sin(m_animTimer * 6.0f);
        upHint.setFillColor(sf::Color(255, 230, 100, static_cast<uint8_t>(pulse)));
        upHint.setPosition({WIN_W / 2.f - upHint.getGlobalBounds().size.x / 2.f, PANEL_Y + 4.0f});
        window.draw(upHint);
    }

    // 7. Render Level Cards
    for (size_t i = m_startIndex; i < endIndex; ++i) {
        const auto& item = m_levels[i];
        bool isSelected = (static_cast<int>(i) == m_selectedIndex);
        size_t slot = i - m_startIndex;
        sf::FloatRect bounds = getCardBounds(slot);
        float yPos = bounds.position.y;

        // Card Rectangle
        sf::RectangleShape card({CARD_W, CARD_H});
        card.setPosition({CARD_X, yPos});

        if (isSelected) {
            float pulse = 210.0f + 40.0f * std::sin(m_animTimer * 5.0f);
            card.setFillColor(sf::Color(42, 90, 195, static_cast<uint8_t>(pulse)));
            card.setOutlineColor(CARD_SEL_OUTLINE);
            card.setOutlineThickness(2.5f);
        } else {
            card.setFillColor(CARD_IDLE_BG);
            card.setOutlineColor(CARD_IDLE_OUTLINE);
            card.setOutlineThickness(1.0f);
        }
        window.draw(card);

        // Icon inside card (zero-copy transform drawing)
        float iconX = CARD_X + 16.0f;
        float iconY = yPos + 10.0f;
        if (item.isCampaign && m_mushroomLoaded && m_mushroomSprite.has_value()) {
            sf::RenderStates states;
            states.transform.translate({iconX, iconY});
            window.draw(m_mushroomSprite.value(), states);
        } else if (m_starLoaded && m_starSprite.has_value()) {
            sf::RenderStates states;
            states.transform.translate({iconX, iconY});
            window.draw(m_starSprite.value(), states);
        }

        // Title text
        sf::Text optTitle(m_font, item.title, 13);
        optTitle.setOutlineThickness(1.5f);
        optTitle.setOutlineColor(sf::Color::Black);
        if (isSelected) {
            optTitle.setFillColor(sf::Color(255, 240, 80));
        } else {
            optTitle.setFillColor(sf::Color::White);
        }
        optTitle.setPosition({CARD_X + 54.0f, yPos + 8.0f});
        window.draw(optTitle);

        // Subtitle text
        sf::Text optSub(m_font, item.subtitle, 9);
        optSub.setFillColor(isSelected ? sf::Color(200, 230, 255) : sf::Color(150, 175, 210));
        optSub.setPosition({CARD_X + 54.0f, yPos + 30.0f});
        window.draw(optSub);

        // Right side tag badge
        const float TAG_W = 100.0f;
        const float TAG_H = 24.0f;
        const float TAG_X = CARD_X + CARD_W - TAG_W - 12.0f;
        const float TAG_Y = yPos + 14.0f;

        sf::RectangleShape tagRect({TAG_W, TAG_H});
        tagRect.setPosition({TAG_X, TAG_Y});
        if (item.isCampaign) {
            tagRect.setFillColor(TAG_CAMPAIGN_BG);
        } else if (item.tag == "BONUS") {
            tagRect.setFillColor(TAG_BONUS_BG);
        } else {
            tagRect.setFillColor(TAG_CUSTOM_BG);
        }
        tagRect.setOutlineColor(sf::Color(255, 255, 255, 120));
        tagRect.setOutlineThickness(1.0f);
        window.draw(tagRect);

        sf::Text tagText(m_font, item.tag, 8);
        tagText.setFillColor(sf::Color::White);
        tagText.setOutlineColor(sf::Color::Black);
        tagText.setOutlineThickness(1.0f);
        tagText.setPosition({TAG_X + TAG_W / 2.f - tagText.getGlobalBounds().size.x / 2.f, TAG_Y + 5.0f});
        window.draw(tagText);

        // Selected animated indicator cursor
        if (isSelected) {
            float bounceX = CARD_X - 22.0f + std::sin(m_animTimer * 8.0f) * 4.0f;
            sf::Text cursor(m_font, ">", 18);
            cursor.setFillColor(sf::Color(255, 225, 50));
            cursor.setOutlineColor(sf::Color::Black);
            cursor.setOutlineThickness(2.0f);
            cursor.setPosition({bounceX, yPos + 12.0f});
            window.draw(cursor);
        }
    }

    // Down scroll hint
    if (endIndex < totalItems) {
        sf::Text downHint(m_font, "v MORE STAGES BELOW v", 10);
        float pulse = 180.0f + 70.0f * std::sin(m_animTimer * 6.0f);
        downHint.setFillColor(sf::Color(255, 230, 100, static_cast<uint8_t>(pulse)));
        downHint.setPosition({WIN_W / 2.f - downHint.getGlobalBounds().size.x / 2.f, PANEL_Y + PANEL_H - 18.0f});
        window.draw(downHint);
    }

    // 8. Right side scroll track & bar
    if (totalItems > VISIBLE_CARDS) {
        float trackX = CARD_X + CARD_W + 10.0f;
        float trackY = CARD_START_Y;
        float trackH = static_cast<float>(VISIBLE_CARDS) * (CARD_H + CARD_GAP) - CARD_GAP;

        sf::RectangleShape track({6.0f, trackH});
        track.setFillColor(sf::Color(30, 45, 80, 160));
        track.setPosition({trackX, trackY});
        window.draw(track);

        float thumbRatio = static_cast<float>(VISIBLE_CARDS) / static_cast<float>(totalItems);
        float thumbH = std::max(20.0f, trackH * thumbRatio);
        float progress = static_cast<float>(m_selectedIndex) / static_cast<float>(totalItems - 1);
        float thumbY = trackY + (trackH - thumbH) * progress;

        sf::RectangleShape thumb({6.0f, thumbH});
        thumb.setFillColor(sf::Color(130, 190, 255, 220));
        thumb.setPosition({trackX, thumbY});
        window.draw(thumb);
    }

    // 9. Bottom Navigation Bar Footer
    sf::RectangleShape footer({WIN_W, 46.0f});
    footer.setFillColor(sf::Color(10, 16, 35, 220));
    footer.setOutlineColor(sf::Color(45, 70, 130, 180));
    footer.setOutlineThickness(1.0f);
    footer.setPosition({0.f, 554.0f});
    window.draw(footer);

    sf::Text hint(m_font, "W/S or UP/DOWN to select   ENTER/CLICK to play   ESC to back", 10);
    hint.setFillColor(sf::Color(180, 205, 235));
    hint.setPosition({WIN_W / 2.f - hint.getGlobalBounds().size.x / 2.f, 569.0f});
    window.draw(hint);
}
