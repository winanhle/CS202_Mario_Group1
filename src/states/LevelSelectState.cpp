#include "LevelSelectState.h"
#include "PlayState.h"
#include "ModeSelectState.h"
#include "MapEditorState.h"
#include "../interfaces/ISaveManager.h"
#include "../core/StateManager.h"
#include "../interfaces/ISoundManager.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace {
    static constexpr const char* FONT_PATH = "assets/fonts/SuperMario256.ttf";

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

    static constexpr sf::Color TEXT_TITLE_SEL(255, 235, 60);
    static constexpr sf::Color TEXT_TITLE_LOCKED(110, 115, 130);
    static constexpr sf::Color TEXT_BTN_LOCKED(255, 180, 180);
    static constexpr sf::Color TEXT_BTN_IDLE(150, 180, 220);
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
    if (m_soundManager) {
        m_soundManager->playMenuMusic();
    }

    if (m_font.openFromFile(FONT_PATH)) {
        m_fontLoaded = true;
    } else {
        std::cerr << "[LevelSelectState] Warning: Failed to load font: " << FONT_PATH << std::endl;
    }

    // Load Stage Preview Tileset Images
    if (m_texStage1.loadFromFile("assets/tileset/world1_1.png")) {
        m_texStage1.setSmooth(false);
        m_tex1Loaded = true;
    }
    if (m_texStage2.loadFromFile("assets/tileset/world1_3.png")) {
        m_texStage2.setSmooth(false);
        m_tex2Loaded = true;
    }
    if (m_texStage3.loadFromFile("assets/tileset/world1_4.png")) {
        m_texStage3.setSmooth(false);
        m_tex3Loaded = true;
    }

    // Star background particles
    m_stars.resize(40);
    for (auto& star : m_stars) {
        star.x = static_cast<float>(std::rand() % static_cast<int>(WIN_W));
        star.y = static_cast<float>(std::rand() % static_cast<int>(WIN_H));
        star.speed = 8.0f + static_cast<float>(std::rand() % 15);
        star.size = 1.0f + static_cast<float>(std::rand() % 2);
        star.phase = static_cast<float>(std::rand() % 100) / 10.0f;
    }

    initLevels();
}

void LevelSelectState::initLevels()
{
    m_levels.clear();

    int maxUnlockedStage = 1;
    if (m_saveManager) {
        maxUnlockedStage = m_saveManager->getMaxUnlockedStage();
        if (maxUnlockedStage < 1) maxUnlockedStage = 1;
    }

    // 1. Stage 1 (Overworld)
    m_levels.push_back({ "assets/map/stage1.tmx", "STAGE 1", false, 1 });

    // 2. Stage 2 (Underground)
    m_levels.push_back({ "assets/map/stage2.tmx", "STAGE 2", (maxUnlockedStage < 2), 2 });

    // 3. Stage 3 (Castle)
    m_levels.push_back({ "assets/map/stage3.tmx", "STAGE 3", (maxUnlockedStage < 3), 3 });

    // 4. Map Editor (directly below Stage 1)
    m_levels.push_back({ "__EDITOR__", "MAP EDITOR", false, 0 });
}

void LevelSelectState::confirmSelection()
{
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_levels.size())) {
        if (m_levels[m_selectedIndex].isLocked) {
            if (m_soundManager) m_soundManager->playUIBump();
            return;
        }

        if (m_levels[m_selectedIndex].path == "__EDITOR__") {
            if (auto* mgr = getStateManager()) {
                mgr->changeState(std::make_unique<MapEditorState>(m_config, m_settings, m_saveManager, m_soundManager));
            }
            return;
        }

        if (m_soundManager) m_soundManager->playStomp();
        m_config.customMapPath = m_levels[m_selectedIndex].path;
        if (auto* mgr = getStateManager()) {
            mgr->changeState(std::make_unique<PlayState>(m_config, m_settings, m_saveManager, m_loadSave, m_soundManager));
        }
    }
}

sf::FloatRect LevelSelectState::getBoxBounds(size_t index) const
{
    if (index >= m_levels.size()) return sf::FloatRect({0.f, 0.f}, {0.f, 0.f});

    float x = 0.f;
    float y = 0.f;

    if (index < 3) {
        // Row 0: 3 columns (Stage 1 at col 0, Stage 2 at col 1, Stage 3 at col 2)
        x = GRID_START_X + static_cast<float>(index) * (BOX_W + COL_GAP);
        y = GRID_START_Y;
    } else {
        // Row 1: Map Editor placed in lower-left, directly below Stage 1 (col 0)
        x = GRID_START_X + 0.0f * (BOX_W + COL_GAP);
        y = GRID_START_Y + (BOX_H + ROW_GAP);
    }

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
        int oldIdx = m_selectedIndex;
        if (total > 0) {
            if (keyEvent->code == sf::Keyboard::Key::A || keyEvent->code == sf::Keyboard::Key::Left) {
                if (m_selectedIndex == 0) m_selectedIndex = 2;
                else if (m_selectedIndex == 1) m_selectedIndex = 0;
                else if (m_selectedIndex == 2) m_selectedIndex = 1;
                else if (m_selectedIndex == 3) m_selectedIndex = 2;
            } else if (keyEvent->code == sf::Keyboard::Key::D || keyEvent->code == sf::Keyboard::Key::Right) {
                if (m_selectedIndex == 0) m_selectedIndex = 1;
                else if (m_selectedIndex == 1) m_selectedIndex = 2;
                else if (m_selectedIndex == 2) m_selectedIndex = 3;
                else if (m_selectedIndex == 3) m_selectedIndex = 0;
            } else if (keyEvent->code == sf::Keyboard::Key::W || keyEvent->code == sf::Keyboard::Key::Up) {
                if (m_selectedIndex == 3) m_selectedIndex = 0; // directly above to Stage 1
                else m_selectedIndex = 3;
            } else if (keyEvent->code == sf::Keyboard::Key::S || keyEvent->code == sf::Keyboard::Key::Down) {
                if (m_selectedIndex == 0) m_selectedIndex = 3; // directly below Stage 1 to Map Editor
                else if (m_selectedIndex == 1 || m_selectedIndex == 2) m_selectedIndex = 3;
                else if (m_selectedIndex == 3) m_selectedIndex = 0;
            } else if (keyEvent->code == sf::Keyboard::Key::Enter || keyEvent->code == sf::Keyboard::Key::Space) {
                confirmSelection();
            }
            if (m_selectedIndex != oldIdx && m_soundManager) {
                m_soundManager->playSelect();
            }
        }
        if (keyEvent->code == sf::Keyboard::Key::Escape) {
            if (m_soundManager) m_soundManager->playSelect();
            if (auto* mgr = getStateManager())
                mgr->changeState(std::make_unique<ModeSelectState>(m_config, m_settings, m_saveManager, m_loadSave, m_soundManager));
        }
    }
    else if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mouse = toViewCoords(mouseMoved->position);

        for (size_t i = 0; i < m_levels.size(); ++i) {
            sf::FloatRect bounds = getBoxBounds(i);
            if (bounds.contains(mouse)) {
                if (m_selectedIndex != static_cast<int>(i) && m_soundManager) {
                    m_soundManager->playSelect();
                }
                m_selectedIndex = static_cast<int>(i);
                break;
            }
        }
    }
    else if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mouse = toViewCoords(mousePressed->position);

            for (size_t i = 0; i < m_levels.size(); ++i) {
                sf::FloatRect bounds = getBoxBounds(i);
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

    // Ensure standard 800x600 view
    sf::View defaultView({ WIN_W / 2.f, WIN_H / 2.f }, { WIN_W, WIN_H });
    window.setView(defaultView);

    // 1. Midnight gradient background
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

    // 2. Star particles
    for (const auto& star : m_stars) {
        float alpha = 130.0f + 100.0f * std::sin(m_animTimer * 2.5f + star.phase);
        alpha = std::clamp(alpha, 30.0f, 255.0f);
        sf::CircleShape starShape(star.size);
        starShape.setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
        starShape.setPosition({star.x, star.y});
        window.draw(starShape);
    }

    if (!m_fontLoaded) return;

    // 3. Header Title
    sf::Text title(m_font, "SELECT STAGE", 26);
    title.setFillColor(HEADER_TITLE_COLOR);
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(2.5f);
    title.setPosition({WIN_W / 2.f - title.getGlobalBounds().size.x / 2.f, 40.0f});
    window.draw(title);

    // 4. Main Stage Browser Bezel Panel
    sf::RectangleShape panel({PANEL_W, PANEL_H});
    panel.setFillColor(PANEL_BG);
    panel.setOutlineColor(PANEL_OUTLINE);
    panel.setOutlineThickness(2.0f);
    panel.setPosition({PANEL_X, PANEL_Y});
    window.draw(panel);

    // 5. Render 4 Stage Boxes
    for (size_t i = 0; i < m_levels.size(); ++i) {
        const auto& item = m_levels[i];
        bool isSelected = (static_cast<int>(i) == m_selectedIndex);
        sf::FloatRect bounds = getBoxBounds(i);
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

        // Title text (Centered in top header area)
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
        float titleX = bx + BOX_W / 2.f - optTitle.getGlobalBounds().size.x / 2.f;
        optTitle.setPosition({titleX, by + 10.0f});
        window.draw(optTitle);

        // Header separator line
        sf::RectangleShape line({BOX_W - 20.0f, 1.0f});
        line.setPosition({bx + 10.0f, by + 30.0f});
        line.setFillColor(SEP_LINE_COLOR);
        window.draw(line);

        // Level Representation Image Preview (Center)
        const float PREVIEW_W = BOX_W - 20.0f; // 196px
        const float PREVIEW_H = 96.0f;
        const float PREVIEW_X = bx + 10.0f;
        const float PREVIEW_Y = by + 36.0f;

        sf::RectangleShape previewFrame({PREVIEW_W, PREVIEW_H});
        previewFrame.setPosition({PREVIEW_X, PREVIEW_Y});
        previewFrame.setFillColor(sf::Color(10, 12, 18));
        previewFrame.setOutlineColor(isSelected ? sf::Color(100, 180, 255) : sf::Color(45, 60, 95));
        previewFrame.setOutlineThickness(1.0f);
        window.draw(previewFrame);

        if (item.stageNum == 1 && m_tex1Loaded) {
            sf::Sprite sprite(m_texStage1);
            sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 320, 160 }));
            sprite.setScale({ PREVIEW_W / 320.f, PREVIEW_H / 160.f });
            sprite.setPosition({ PREVIEW_X, PREVIEW_Y });
            window.draw(sprite);
        } else if (item.stageNum == 2 && m_tex2Loaded) {
            sf::Sprite sprite(m_texStage2);
            sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 320, 160 }));
            sprite.setScale({ PREVIEW_W / 320.f, PREVIEW_H / 160.f });
            sprite.setPosition({ PREVIEW_X, PREVIEW_Y });
            window.draw(sprite);
        } else if (item.stageNum == 3 && m_tex3Loaded) {
            sf::Sprite sprite(m_texStage3);
            sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 320, 160 }));
            sprite.setScale({ PREVIEW_W / 320.f, PREVIEW_H / 160.f });
            sprite.setPosition({ PREVIEW_X, PREVIEW_Y });
            window.draw(sprite);
        } else if (item.stageNum == 0) {
            // Map Editor Canvas Preview
            sf::RectangleShape edBg({ PREVIEW_W, PREVIEW_H });
            edBg.setPosition({ PREVIEW_X, PREVIEW_Y });
            edBg.setFillColor(sf::Color(26, 30, 40));
            window.draw(edBg);

            // Miniature grid lines
            sf::VertexArray gridLines(sf::PrimitiveType::Lines);
            for (float gx = PREVIEW_X; gx <= PREVIEW_X + PREVIEW_W; gx += 14.f) {
                gridLines.append(sf::Vertex{ sf::Vector2f{ gx, PREVIEW_Y }, sf::Color(45, 52, 68) });
                gridLines.append(sf::Vertex{ sf::Vector2f{ gx, PREVIEW_Y + PREVIEW_H }, sf::Color(45, 52, 68) });
            }
            for (float gy = PREVIEW_Y; gy <= PREVIEW_Y + PREVIEW_H; gy += 14.f) {
                gridLines.append(sf::Vertex{ sf::Vector2f{ PREVIEW_X, gy }, sf::Color(45, 52, 68) });
                gridLines.append(sf::Vertex{ sf::Vector2f{ PREVIEW_X + PREVIEW_W, gy }, sf::Color(45, 52, 68) });
            }
            window.draw(gridLines);

            // Sample blocks
            sf::RectangleShape gBlock({ 20.f, 20.f });
            gBlock.setFillColor(sf::Color(190, 115, 55));
            gBlock.setOutlineColor(sf::Color::White);
            gBlock.setOutlineThickness(0.5f);
            gBlock.setPosition({ PREVIEW_X + 22.f, PREVIEW_Y + 54.f });
            window.draw(gBlock);

            sf::RectangleShape qBlock({ 20.f, 20.f });
            qBlock.setFillColor(sf::Color(255, 200, 0));
            qBlock.setOutlineColor(sf::Color::White);
            qBlock.setOutlineThickness(0.5f);
            qBlock.setPosition({ PREVIEW_X + 50.f, PREVIEW_Y + 24.f });
            window.draw(qBlock);

            sf::RectangleShape pBlock({ 20.f, 32.f });
            pBlock.setFillColor(sf::Color(40, 180, 40));
            pBlock.setOutlineColor(sf::Color::White);
            pBlock.setOutlineThickness(0.5f);
            pBlock.setPosition({ PREVIEW_X + 80.f, PREVIEW_Y + 42.f });
            window.draw(pBlock);

            sf::RectangleShape spawnBadge({ 72.f, 15.f });
            spawnBadge.setPosition({ PREVIEW_X + 114.f, PREVIEW_Y + 28.f });
            spawnBadge.setFillColor(sf::Color(0, 200, 240, 210));
            spawnBadge.setOutlineColor(sf::Color::White);
            spawnBadge.setOutlineThickness(0.5f);
            window.draw(spawnBadge);

            sf::Text spawnText(m_font, "Spawn", 7);
            spawnText.setFillColor(sf::Color::Black);
            spawnText.setPosition({ PREVIEW_X + 130.f, PREVIEW_Y + 30.f });
            window.draw(spawnText);
        }

        // Locked Overlay
        if (item.isLocked) {
            sf::RectangleShape lockOverlay({PREVIEW_W, PREVIEW_H});
            lockOverlay.setPosition({PREVIEW_X, PREVIEW_Y});
            lockOverlay.setFillColor(sf::Color(0, 0, 0, 190));
            window.draw(lockOverlay);

            sf::RectangleShape lockBadge({72.f, 20.f});
            lockBadge.setPosition({PREVIEW_X + PREVIEW_W / 2.f - 36.f, PREVIEW_Y + PREVIEW_H / 2.f - 10.f});
            lockBadge.setFillColor(sf::Color(140, 30, 30, 230));
            lockBadge.setOutlineColor(sf::Color(220, 70, 70));
            lockBadge.setOutlineThickness(1.f);
            window.draw(lockBadge);

            sf::Text lockText(m_font, "LOCKED", 7);
            lockText.setFillColor(sf::Color::White);
            lockText.setPosition({PREVIEW_X + PREVIEW_W / 2.f - lockText.getGlobalBounds().size.x / 2.f,
                                  PREVIEW_Y + PREVIEW_H / 2.f - 6.f});
            window.draw(lockText);
        }

        // Bottom Action Button
        const float BTN_W = BOX_W - 20.0f;
        const float BTN_H = 26.0f;
        const float BTN_X = bx + 10.0f;
        const float BTN_Y = by + BOX_H - 34.0f;

        sf::RectangleShape btn({BTN_W, BTN_H});
        btn.setPosition({BTN_X, BTN_Y});

        if (item.isLocked) {
            btn.setFillColor(BTN_LOCKED_BG);
            btn.setOutlineColor(BTN_LOCKED_OUTLINE);
            btn.setOutlineThickness(1.0f);
            window.draw(btn);

            sf::Text status(m_font, "LOCKED", 8);
            status.setFillColor(TEXT_BTN_LOCKED);
            status.setOutlineColor(sf::Color::Black);
            status.setOutlineThickness(1.0f);
            status.setPosition({BTN_X + BTN_W / 2.f - status.getGlobalBounds().size.x / 2.f, BTN_Y + 6.0f});
            window.draw(status);
        } else if (isSelected) {
            float btnPulse = 180.0f + 75.0f * std::sin(m_animTimer * 6.0f);
            btn.setFillColor(sf::Color(50, 140, 240, static_cast<uint8_t>(btnPulse)));
            btn.setOutlineColor(BTN_SEL_OUTLINE);
            btn.setOutlineThickness(1.5f);
            window.draw(btn);

            std::string btnStr = (item.stageNum == 0 ? "> BUILD <" : "> PLAY <");
            sf::Text status(m_font, btnStr, 8);
            status.setFillColor(sf::Color::White);
            status.setOutlineColor(sf::Color::Black);
            status.setOutlineThickness(1.0f);
            status.setPosition({BTN_X + BTN_W / 2.f - status.getGlobalBounds().size.x / 2.f, BTN_Y + 6.0f});
            window.draw(status);
        } else {
            btn.setFillColor(BTN_IDLE_BG);
            btn.setOutlineColor(BTN_IDLE_OUTLINE);
            btn.setOutlineThickness(1.0f);
            window.draw(btn);

            std::string btnStr = (item.stageNum == 0 ? "EDIT" : "SELECT");
            sf::Text status(m_font, btnStr, 8);
            status.setFillColor(TEXT_BTN_IDLE);
            status.setPosition({BTN_X + BTN_W / 2.f - status.getGlobalBounds().size.x / 2.f, BTN_Y + 6.0f});
            window.draw(status);
        }
    }

    // 6. Bottom Navigation Bar Footer
    sf::RectangleShape footer({WIN_W, 46.0f});
    footer.setFillColor(FOOTER_BG);
    footer.setOutlineColor(FOOTER_OUTLINE);
    footer.setOutlineThickness(1.0f);
    footer.setPosition({0.f, 554.0f});
    window.draw(footer);

    sf::Text hint(m_font, "WASD / ARROWS: Navigate   ENTER / CLICK: Select   ESC: Back", 10);
    hint.setFillColor(FOOTER_TEXT);
    hint.setPosition({WIN_W / 2.f - hint.getGlobalBounds().size.x / 2.f, 569.0f});
    window.draw(hint);
}
