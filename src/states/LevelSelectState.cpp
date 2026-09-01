#include "LevelSelectState.h"
#include "PlayState.h"
#include "ModeSelectState.h"
#include "../interfaces/ISaveManager.h"
#include "../core/StateManager.h"
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

    static constexpr size_t COLS = 3;
    static constexpr size_t ROWS = 2;

    static constexpr float PANEL_X = 40.0f;
    static constexpr float PANEL_Y = 115.0f;
    static constexpr float PANEL_W = 720.0f;
    static constexpr float PANEL_H = 415.0f;

    static constexpr float BOX_W = 216.0f;
    static constexpr float BOX_H = 175.0f;
    static constexpr float COL_GAP = 16.0f;
    static constexpr float ROW_GAP = 18.0f;

    static constexpr float GRID_START_X = 60.0f;
    static constexpr float GRID_START_Y = 138.0f;

    // Background & Panel Colors
    static constexpr sf::Color BG_TOP(18, 28, 62);
    static constexpr sf::Color BG_BOT(8, 12, 28);
    static constexpr sf::Color PANEL_BG(16, 22, 44, 230);
    static constexpr sf::Color PANEL_OUTLINE(65, 105, 185, 200);

    // Box Styles
    static constexpr sf::Color BOX_IDLE_BG(24, 34, 65, 190);
    static constexpr sf::Color BOX_IDLE_OUTLINE(48, 70, 125, 200);
    static constexpr sf::Color BOX_SEL_BG(35, 75, 160, 230);
    static constexpr sf::Color BOX_SEL_OUTLINE(140, 215, 255);
    static constexpr sf::Color BOX_LOCKED_BG(18, 20, 30, 210);
    static constexpr sf::Color BOX_LOCKED_OUTLINE(45, 50, 70, 180);

    // Tag Badges
    static constexpr sf::Color TAG_CAMPAIGN_BG(200, 140, 30, 220);
    static constexpr sf::Color TAG_CAMPAIGN_OUTLINE(255, 215, 100, 180);
    static constexpr sf::Color TAG_CUSTOM_BG(45, 120, 190, 200);
    static constexpr sf::Color TAG_CUSTOM_OUTLINE(140, 215, 255, 180);
    static constexpr sf::Color TAG_BONUS_BG(140, 45, 190, 200);
    static constexpr sf::Color TAG_BONUS_OUTLINE(220, 150, 255, 180);
    static constexpr sf::Color TAG_DEBUG_BG(70, 85, 110, 200);
    static constexpr sf::Color TAG_DEBUG_OUTLINE(150, 175, 205, 180);
    static constexpr sf::Color TAG_LOCKED_BG(45, 50, 65, 180);
    static constexpr sf::Color TAG_LOCKED_OUTLINE(70, 75, 95, 150);

    // UI Accents & Text
    static constexpr sf::Color SEP_LINE_COLOR(55, 75, 125, 140);
    static constexpr sf::Color BTN_LOCKED_BG(120, 25, 25, 200);
    static constexpr sf::Color BTN_LOCKED_OUTLINE(200, 50, 50, 180);
    static constexpr sf::Color BTN_SEL_OUTLINE(220, 240, 255);
    static constexpr sf::Color BTN_IDLE_BG(30, 45, 80, 140);
    static constexpr sf::Color BTN_IDLE_OUTLINE(55, 80, 135, 160);
    static constexpr sf::Color FOOTER_BG(10, 16, 35, 220);
    static constexpr sf::Color FOOTER_OUTLINE(45, 70, 130, 180);
    static constexpr sf::Color FOOTER_TEXT(180, 205, 235);
    static constexpr sf::Color HEADER_TITLE_COLOR(255, 225, 60);
    static constexpr sf::Color CAPSULE_BG(25, 38, 75, 200);
    static constexpr sf::Color CAPSULE_OUTLINE(80, 140, 240, 180);
    static constexpr sf::Color CAPSULE_TEXT(200, 230, 255);
    static constexpr sf::Color TEXT_TITLE_SEL(255, 235, 60);
    static constexpr sf::Color TEXT_TITLE_LOCKED(110, 115, 130);
    static constexpr sf::Color TEXT_SUB_SEL(215, 235, 255);
    static constexpr sf::Color TEXT_SUB_IDLE(160, 185, 220);
    static constexpr sf::Color TEXT_BTN_LOCKED(255, 180, 180);
    static constexpr sf::Color TEXT_BTN_IDLE(150, 180, 220);
    static constexpr sf::Color PAGE_HINT_COLOR(255, 230, 100);

    struct StageMetadata {
        const char* stem;
        const char* title;
        const char* subtitle;
        const char* tag;
        int requiredUnlockStage;
    };

    static constexpr StageMetadata KNOWN_STAGES[] = {
        { "stage1",        "STAGE 1",     "Grassland Hills\nGoombas & Pipes",   "WORLD 1-1", 1 },
        { "stage1_hidden", "BONUS VAULT", "Secret Underground\nCoin Trove Cavern", "BONUS", 1 },
        { "stage2",        "STAGE 2",     "Underground Cavern\nMoving Lifts",    "WORLD 1-2", 2 },
        { "stage3",        "STAGE 3",     "Castle Battlements\nFirebars & Heights", "WORLD 1-3", 3 },
        { "map_test",      "TEST MAP",    "Sandbox Playground\nMechanics Testing", "DEBUG", 0 }
    };
}

LevelSelectState::LevelSelectState(const GameConfig& config,
                                   std::shared_ptr<ISettingsManager> settings,
                                   std::shared_ptr<ISaveManager> saveManager,
                                   bool loadSave)
    : m_config(config),
      m_settings(std::move(settings)),
      m_saveManager(std::move(saveManager)),
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
        m_mushroomSprite->setScale({0.65f, 0.65f});
    }

    if (m_starTexture.loadFromFile("assets/texture/item/Star.PNG")) {
        m_starLoaded = true;
        m_starSprite.emplace(m_starTexture);
        m_starSprite->setScale({0.65f, 0.65f});
    }

    // Initialize background starfield particles
    m_stars.resize(40);
    for (auto& star : m_stars) {
        star.x = static_cast<float>(std::rand() % static_cast<int>(WIN_W));
        star.y = static_cast<float>(std::rand() % static_cast<int>(WIN_H));
        star.speed = 8.0f + static_cast<float>(std::rand() % 25);
        star.size = 1.0f + static_cast<float>(std::rand() % 3) * 0.5f;
        star.phase = static_cast<float>(std::rand() % 100) * 0.1f;
    }

    discoverMaps();
}

void LevelSelectState::discoverMaps()
{
    m_levels.clear();

    int maxUnlockedStage = 1;
    if (m_saveManager) {
        maxUnlockedStage = m_saveManager->getMaxUnlockedStage();
    }

    // 1. Official Campaign Entry
    m_levels.push_back({
        "",
        "CAMPAIGN",
        "World 1-1 onwards\nFull story mode",
        "STORY",
        true,
        false
    });

    if (!fs::exists(MAPS_DIR)) {
        return;
    }

    // 2. Discover .tmx files in assets/map/
    std::vector<std::string> discovered;
    for (const auto& entry : fs::directory_iterator(MAPS_DIR)) {
        if (entry.is_regular_file() && entry.path().extension() == ".tmx") {
            discovered.push_back(entry.path().string());
        }
    }

    std::sort(discovered.begin(), discovered.end());

    for (const auto& p : discovered) {
        std::string filename = fs::path(p).stem().string();
        std::string title = filename;
        std::string subtitle = "Custom Stage Map";
        std::string tag = "MAP";
        bool locked = false;

        bool matched = false;
        for (const auto& meta : KNOWN_STAGES) {
            if (filename == meta.stem) {
                title = meta.title;
                subtitle = meta.subtitle;
                tag = meta.tag;
                if (meta.requiredUnlockStage > 0) {
                    locked = (meta.requiredUnlockStage > maxUnlockedStage);
                }
                matched = true;
                break;
            }
        }

        if (!matched && filename.find("stage") == 0) {
            try {
                int stageNum = std::stoi(filename.substr(5));
                locked = (stageNum > maxUnlockedStage);
            } catch (...) {}
        }

        m_levels.push_back({ p, title, subtitle, tag, false, locked });
    }
}

void LevelSelectState::confirmSelection()
{
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_levels.size())) {
        if (m_levels[m_selectedIndex].isLocked) return;
        m_config.customMapPath = m_levels[m_selectedIndex].path;
        if (auto* mgr = getStateManager()) {
            mgr->changeState(std::make_unique<PlayState>(m_config, m_settings, m_saveManager, m_loadSave));
        }
    }
}

sf::FloatRect LevelSelectState::getBoxBounds(size_t slotOnPage) const
{
    if (slotOnPage >= PAGE_SIZE) return sf::FloatRect({0.f, 0.f}, {0.f, 0.f});

    size_t col = slotOnPage % COLS;
    size_t row = slotOnPage / COLS;

    float x = GRID_START_X + static_cast<float>(col) * (BOX_W + COL_GAP);
    float y = GRID_START_Y + static_cast<float>(row) * (BOX_H + ROW_GAP);
    return sf::FloatRect({x, y}, {BOX_W, BOX_H});
}

sf::Vector2f LevelSelectState::toViewCoords(sf::Vector2i pixelPos) const
{
    if (m_windowSize.x == 0 || m_windowSize.y == 0)
        return { static_cast<float>(pixelPos.x), static_cast<float>(pixelPos.y) };

    const float scaleX = WIN_W / static_cast<float>(m_windowSize.x);
    const float scaleY = WIN_H / static_cast<float>(m_windowSize.y);
    return { static_cast<float>(pixelPos.x) * scaleX,
             static_cast<float>(pixelPos.y) * scaleY };
}

void LevelSelectState::handleInput(const sf::Event& event)
{
    if (const auto* resizeEvent = event.getIf<sf::Event::Resized>()) {
        m_windowSize = resizeEvent->size;
        return;
    }

    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        int total = static_cast<int>(m_levels.size());
        if (total > 0) {
            if (keyEvent->code == sf::Keyboard::Key::A || keyEvent->code == sf::Keyboard::Key::Left) {
                m_selectedIndex = (m_selectedIndex - 1 + total) % total;
            } else if (keyEvent->code == sf::Keyboard::Key::D || keyEvent->code == sf::Keyboard::Key::Right) {
                m_selectedIndex = (m_selectedIndex + 1) % total;
            } else if (keyEvent->code == sf::Keyboard::Key::W || keyEvent->code == sf::Keyboard::Key::Up) {
                if (m_selectedIndex - static_cast<int>(COLS) >= 0) {
                    m_selectedIndex -= static_cast<int>(COLS);
                } else {
                    int target = m_selectedIndex + static_cast<int>(COLS);
                    if (target < total) m_selectedIndex = target;
                }
            } else if (keyEvent->code == sf::Keyboard::Key::S || keyEvent->code == sf::Keyboard::Key::Down) {
                if (m_selectedIndex + static_cast<int>(COLS) < total) {
                    m_selectedIndex += static_cast<int>(COLS);
                } else {
                    int target = m_selectedIndex - static_cast<int>(COLS);
                    if (target >= 0) m_selectedIndex = target;
                }
            } else if (keyEvent->code == sf::Keyboard::Key::PageUp) {
                m_selectedIndex = std::max(0, m_selectedIndex - static_cast<int>(PAGE_SIZE));
            } else if (keyEvent->code == sf::Keyboard::Key::PageDown) {
                m_selectedIndex = std::min(total - 1, m_selectedIndex + static_cast<int>(PAGE_SIZE));
            } else if (keyEvent->code == sf::Keyboard::Key::Enter || keyEvent->code == sf::Keyboard::Key::Space) {
                confirmSelection();
            }
            m_currentPage = static_cast<size_t>(m_selectedIndex) / PAGE_SIZE;
        }
        if (keyEvent->code == sf::Keyboard::Key::Escape) {
            if (auto* mgr = getStateManager())
                mgr->changeState(std::make_unique<ModeSelectState>(m_config, m_settings, m_saveManager, m_loadSave));
        }
    }
    else if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mouse = toViewCoords(mouseMoved->position);
        size_t startIdx = m_currentPage * PAGE_SIZE;
        size_t endIdx = std::min(startIdx + PAGE_SIZE, m_levels.size());

        for (size_t i = startIdx; i < endIdx; ++i) {
            sf::FloatRect bounds = getBoxBounds(i - startIdx);
            if (bounds.contains(mouse)) {
                m_selectedIndex = static_cast<int>(i);
                break;
            }
        }
    }
    else if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mouse = toViewCoords(mousePressed->position);

            size_t startIdx = m_currentPage * PAGE_SIZE;
            size_t endIdx = std::min(startIdx + PAGE_SIZE, m_levels.size());

            for (size_t i = startIdx; i < endIdx; ++i) {
                sf::FloatRect bounds = getBoxBounds(i - startIdx);
                if (bounds.contains(mouse)) {
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
    m_windowSize = window.getSize();

    // Ensure render uses standard 800x600 view
    sf::View defaultView({ WIN_W / 2.f, WIN_H / 2.f }, { WIN_W, WIN_H });
    window.setView(defaultView);

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
    title.setFillColor(HEADER_TITLE_COLOR);
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
    badge.setFillColor(CAPSULE_BG);
    badge.setOutlineColor(CAPSULE_OUTLINE);
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
    cfg.setFillColor(CAPSULE_TEXT);
    cfg.setOutlineColor(sf::Color::Black);
    cfg.setOutlineThickness(1.0f);
    cfg.setPosition({BADGE_X + 46.0f, BADGE_Y + 9.0f});
    window.draw(cfg);

    // 5. Draw Main Stage Browser Bezel Panel
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

    // Multi-page calculation
    size_t totalPages = (m_levels.size() + PAGE_SIZE - 1) / PAGE_SIZE;
    size_t m_startIndex = m_currentPage * PAGE_SIZE;
    size_t endIndex = std::min(m_startIndex + PAGE_SIZE, m_levels.size());

    // 6. Render Grid Boxes for current page
    for (size_t i = m_startIndex; i < endIndex; ++i) {
        const auto& item = m_levels[i];
        bool isSelected = (static_cast<int>(i) == m_selectedIndex);
        size_t slot = i - m_startIndex;
        sf::FloatRect bounds = getBoxBounds(slot);
        float bx = bounds.position.x;
        float by = bounds.position.y;

        // Box Rectangle
        sf::RectangleShape box({BOX_W, BOX_H});
        box.setPosition({bx, by});

        if (item.isLocked) {
            box.setFillColor(BOX_LOCKED_BG);
            box.setOutlineColor(BOX_LOCKED_OUTLINE);
            box.setOutlineThickness(1.0f);
        } else if (isSelected) {
            float pulse = 210.0f + 45.0f * std::sin(m_animTimer * 5.0f);
            box.setFillColor(sf::Color(BOX_SEL_BG.r, BOX_SEL_BG.g, BOX_SEL_BG.b, static_cast<uint8_t>(pulse)));
            box.setOutlineColor(BOX_SEL_OUTLINE);
            box.setOutlineThickness(2.5f);
        } else {
            box.setFillColor(BOX_IDLE_BG);
            box.setOutlineColor(BOX_IDLE_OUTLINE);
            box.setOutlineThickness(1.0f);
        }
        window.draw(box);

        // Icon (top-left inside box)
        float iconX = bx + 12.0f;
        float iconY = by + 10.0f;
        if (!item.isLocked) {
            if (item.isCampaign && m_mushroomLoaded && m_mushroomSprite.has_value()) {
                sf::RenderStates states;
                states.transform.translate({iconX, iconY});
                window.draw(m_mushroomSprite.value(), states);
            } else if (m_starLoaded && m_starSprite.has_value()) {
                sf::RenderStates states;
                states.transform.translate({iconX, iconY});
                window.draw(m_starSprite.value(), states);
            }
        }

        // Tag Badge (top-right inside box)
        const float TAG_W = 68.0f;
        const float TAG_H = 18.0f;
        const float TAG_X = bx + BOX_W - TAG_W - 10.0f;
        const float TAG_Y = by + 10.0f;

        sf::RectangleShape tagRect({TAG_W, TAG_H});
        tagRect.setPosition({TAG_X, TAG_Y});
        if (item.isLocked) {
            tagRect.setFillColor(TAG_LOCKED_BG);
            tagRect.setOutlineColor(TAG_LOCKED_OUTLINE);
        } else if (item.isCampaign) {
            tagRect.setFillColor(TAG_CAMPAIGN_BG);
            tagRect.setOutlineColor(TAG_CAMPAIGN_OUTLINE);
        } else if (item.tag == "BONUS") {
            tagRect.setFillColor(TAG_BONUS_BG);
            tagRect.setOutlineColor(TAG_BONUS_OUTLINE);
        } else if (item.tag == "DEBUG") {
            tagRect.setFillColor(TAG_DEBUG_BG);
            tagRect.setOutlineColor(TAG_DEBUG_OUTLINE);
        } else {
            tagRect.setFillColor(TAG_CUSTOM_BG);
            tagRect.setOutlineColor(TAG_CUSTOM_OUTLINE);
        }
        tagRect.setOutlineThickness(1.0f);
        window.draw(tagRect);

        sf::Text tagText(m_font, item.tag, 7);
        tagText.setFillColor(item.isLocked ? sf::Color(140, 145, 160) : sf::Color::White);
        tagText.setOutlineColor(sf::Color::Black);
        tagText.setOutlineThickness(1.0f);
        tagText.setPosition({TAG_X + TAG_W / 2.f - tagText.getGlobalBounds().size.x / 2.f, TAG_Y + 3.0f});
        window.draw(tagText);

        // Header separator line
        sf::RectangleShape line({BOX_W - 20.0f, 1.0f});
        line.setPosition({bx + 10.0f, by + 34.0f});
        line.setFillColor(SEP_LINE_COLOR);
        window.draw(line);

        // Title text
        sf::Text optTitle(m_font, item.title, 11);
        optTitle.setOutlineThickness(1.0f);
        optTitle.setOutlineColor(sf::Color::Black);
        if (item.isLocked) {
            optTitle.setFillColor(TEXT_TITLE_LOCKED);
        } else if (isSelected) {
            optTitle.setFillColor(TEXT_TITLE_SEL);
        } else {
            optTitle.setFillColor(sf::Color::White);
        }
        optTitle.setPosition({bx + 12.0f, by + 44.0f});
        window.draw(optTitle);

        // Subtitle description text
        if (!item.isLocked) {
            sf::Text optSub(m_font, item.subtitle, 8);
            optSub.setLineSpacing(1.3f);
            optSub.setFillColor(isSelected ? TEXT_SUB_SEL : TEXT_SUB_IDLE);
            optSub.setPosition({bx + 12.0f, by + 68.0f});
            window.draw(optSub);
        }

        // Bottom Action Bar / Status Badge
        const float BTN_W = BOX_W - 20.0f;
        const float BTN_H = 22.0f;
        const float BTN_X = bx + 10.0f;
        const float BTN_Y = by + BOX_H - 30.0f;

        sf::RectangleShape btn({BTN_W, BTN_H});
        btn.setPosition({BTN_X, BTN_Y});

        if (item.isLocked) {
            btn.setFillColor(BTN_LOCKED_BG);
            btn.setOutlineColor(BTN_LOCKED_OUTLINE);
            btn.setOutlineThickness(1.0f);
            window.draw(btn);

            sf::Text status(m_font, "LOCKED", 9);
            status.setFillColor(TEXT_BTN_LOCKED);
            status.setOutlineColor(sf::Color::Black);
            status.setOutlineThickness(1.0f);
            status.setPosition({BTN_X + BTN_W / 2.f - status.getGlobalBounds().size.x / 2.f, BTN_Y + 4.0f});
            window.draw(status);
        } else if (isSelected) {
            float btnPulse = 180.0f + 75.0f * std::sin(m_animTimer * 6.0f);
            btn.setFillColor(sf::Color(50, 140, 240, static_cast<uint8_t>(btnPulse)));
            btn.setOutlineColor(BTN_SEL_OUTLINE);
            btn.setOutlineThickness(1.5f);
            window.draw(btn);

            sf::Text status(m_font, "> PLAY <", 8);
            status.setFillColor(sf::Color::White);
            status.setOutlineColor(sf::Color::Black);
            status.setOutlineThickness(1.0f);
            status.setPosition({BTN_X + BTN_W / 2.f - status.getGlobalBounds().size.x / 2.f, BTN_Y + 5.0f});
            window.draw(status);
        } else {
            btn.setFillColor(BTN_IDLE_BG);
            btn.setOutlineColor(BTN_IDLE_OUTLINE);
            btn.setOutlineThickness(1.0f);
            window.draw(btn);

            sf::Text status(m_font, "SELECT", 8);
            status.setFillColor(TEXT_BTN_IDLE);
            status.setPosition({BTN_X + BTN_W / 2.f - status.getGlobalBounds().size.x / 2.f, BTN_Y + 5.0f});
            window.draw(status);
        }
    }

    // 7. Page indicators (if more than 1 page)
    if (totalPages > 1) {
        std::string pageStr = "PAGE " + std::to_string(m_currentPage + 1) + " / " + std::to_string(totalPages);
        sf::Text pageText(m_font, pageStr, 9);
        pageText.setFillColor(PAGE_HINT_COLOR);
        pageText.setOutlineColor(sf::Color::Black);
        pageText.setOutlineThickness(1.0f);
        pageText.setPosition({WIN_W / 2.f - pageText.getGlobalBounds().size.x / 2.f, PANEL_Y + PANEL_H - 16.0f});
        window.draw(pageText);
    }

    // 8. Bottom Navigation Bar Footer
    sf::RectangleShape footer({WIN_W, 46.0f});
    footer.setFillColor(FOOTER_BG);
    footer.setOutlineColor(FOOTER_OUTLINE);
    footer.setOutlineThickness(1.0f);
    footer.setPosition({0.f, 554.0f});
    window.draw(footer);

    sf::Text hint(m_font, "WASD / ARROWS: Navigate   ENTER / CLICK: Play   ESC: Back", 10);
    hint.setFillColor(FOOTER_TEXT);
    hint.setPosition({WIN_W / 2.f - hint.getGlobalBounds().size.x / 2.f, 569.0f});
    window.draw(hint);
}
