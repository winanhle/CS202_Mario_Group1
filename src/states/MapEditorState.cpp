#include "MapEditorState.h"
#include "PlayState.h"
#include "LevelSelectState.h"
#include "../core/StateManager.h"
#include "../interfaces/ISettingsManager.h"
#include "../interfaces/ISaveManager.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <queue>
#include <algorithm>

namespace {
    static constexpr const char* FONT_PATH = "assets/fonts/SuperMario256.ttf";
}

MapEditorState::MapEditorState(const GameConfig& config,
                               std::shared_ptr<ISettingsManager> settings,
                               std::shared_ptr<ISaveManager> saveManager)
    : m_config(config),
      m_settings(std::move(settings)),
      m_saveManager(std::move(saveManager))
{
    m_config.fromEditor = true;

    m_mapManager = std::make_shared<MapManager>();
    m_mapManager->initBlank(MAP_GRID_W, MAP_GRID_H);

    if (m_font.openFromFile(FONT_PATH)) {
        m_fontLoaded = true;
    } else {
        std::cerr << "[MapEditorState] Warning: Failed to load font: " << FONT_PATH << std::endl;
    }

    initPalette();
    resetGrid();
}

void MapEditorState::initPalette()
{
    m_categories = { "All", "Overworld", "Underground", "Castle", "Entities" };
    m_palette.clear();

    // ── Overworld (world1_1.tsx, firstgid=1) ───────────────────────────────
    m_palette.push_back({ "GROUND (TOP)",    "Overworld", true, TileType::GROUND,           2744,  "",      false, false, sf::Color(190, 115, 55) });
    m_palette.push_back({ "GROUND (DIRT)",   "Overworld", true, TileType::GROUND,           2955,  "",      false, false, sf::Color(150, 85, 35) });
    m_palette.push_back({ "BRICK (SOLID)",   "Overworld", true, TileType::BRICK_SOLID,       1920,  "",      false, false, sf::Color(200, 100, 40) });
    m_palette.push_back({ "BRICK (EMPTY)",   "Overworld", true, TileType::BRICK_EMPTY,       1920,  "",      false, false, sf::Color(220, 130, 60) });
    m_palette.push_back({ "? COIN",          "Overworld", true, TileType::QUESTION_COIN,     1916,  "",      false, false, sf::Color(255, 200, 0) });
    m_palette.push_back({ "? POWERUP",       "Overworld", true, TileType::QUESTION_POWERUP,  1078,  "",      false, false, sf::Color(255, 160, 20) });
    m_palette.push_back({ "MULTI COIN",      "Overworld", true, TileType::MULTI_COIN,        1923,  "",      false, false, sf::Color(255, 215, 0) });
    m_palette.push_back({ "SOLID BLOCK",     "Overworld", true, TileType::SOLID_BRICK,       2667,  "",      false, false, sf::Color(140, 70, 20) });
    m_palette.push_back({ "PIPE TOP L",      "Overworld", true, TileType::PIPE,              2149,  "",      false, false, sf::Color(40, 180, 40) });
    m_palette.push_back({ "PIPE TOP R",      "Overworld", true, TileType::PIPE,              2150,  "",      false, false, sf::Color(40, 180, 40) });
    m_palette.push_back({ "PIPE BODY L",     "Overworld", true, TileType::PIPE,              2360,  "",      false, false, sf::Color(30, 150, 30) });
    m_palette.push_back({ "PIPE BODY R",     "Overworld", true, TileType::PIPE,              2361,  "",      false, false, sf::Color(30, 150, 30) });
    m_palette.push_back({ "FLAG TOP",        "Overworld", true, TileType::FLAGPOLE,          621,   "",      false, false, sf::Color(220, 220, 220) });
    m_palette.push_back({ "FLAG POLE",       "Overworld", true, TileType::FLAGPOLE,          832,   "",      false, false, sf::Color(200, 200, 200) });
    m_palette.push_back({ "FLAG BASE",       "Overworld", true, TileType::FLAGPOLE,          1254,  "",      false, false, sf::Color(180, 180, 180) });
    m_palette.push_back({ "SKY (EMPTY)",     "Overworld", true, TileType::EMPTY,             1,     "",      false, false, sf::Color(92, 148, 252) });

    // ── Underground (world1_3.tsx, firstgid=7001) ──────────────────────────
    m_palette.push_back({ "BLUE GROUND",     "Underground", true, TileType::GROUND,          8313,  "",      false, false, sf::Color(60, 120, 180) });
    m_palette.push_back({ "BLUE BRICK",      "Underground", true, TileType::BRICK_SOLID,      8700,  "",      false, false, sf::Color(50, 100, 160) });
    m_palette.push_back({ "BLUE SOLID",      "Underground", true, TileType::SOLID_BRICK,      7963,  "",      false, false, sf::Color(40, 80, 140) });
    m_palette.push_back({ "U-SKY (EMPTY)",   "Underground", true, TileType::EMPTY,           7001,  "",      false, false, sf::Color(0, 0, 0) });

    // ── Castle (world1_4.tsx, firstgid=10001) ─────────────────────────────
    m_palette.push_back({ "CASTLE GROUND",   "Castle",    true, TileType::GROUND,           10321, "",      false, false, sf::Color(130, 130, 130) });
    m_palette.push_back({ "CASTLE BRICK",    "Castle",    true, TileType::SOLID_BRICK,      11601, "",      false, false, sf::Color(100, 100, 100) });
    m_palette.push_back({ "FIRE BAR",        "Castle",    true, TileType::FIRE_BAR,         10984, "",      false, false, sf::Color(255, 60, 0) });
    m_palette.push_back({ "CASTLE (EMPTY)",  "Castle",    true, TileType::EMPTY,           10001, "",      false, false, sf::Color(14, 14, 18) });

    // ── Entities & Tools ─────────────────────────────────────────────────
    m_palette.push_back({ "Goomba",          "Entities",  false, TileType::EMPTY,           0,     "Goomba",      false, false, sf::Color(180, 90, 20) });
    m_palette.push_back({ "KoopaTroopa",     "Entities",  false, TileType::EMPTY,           0,     "KoopaTroopa", false, false, sf::Color(30, 180, 80) });
    m_palette.push_back({ "BuzzyBeetle",     "Entities",  false, TileType::EMPTY,           0,     "BuzzyBeetle", false, false, sf::Color(80, 100, 220) });
    m_palette.push_back({ "Star Item",       "Entities",  false, TileType::EMPTY,           0,     "Star",        false, false, sf::Color(255, 230, 0) });
    m_palette.push_back({ "Player Spawn",    "Entities",  false, TileType::EMPTY,           0,     "",            true,  false, sf::Color(0, 210, 255) });
    m_palette.push_back({ "Eraser",          "Entities",  false, TileType::EMPTY,           0,     "",            false, true,  sf::Color(200, 50, 50) });
}

void MapEditorState::resetGrid()
{
    int emptyGid = (m_bgTheme == BackgroundTheme::Castle) ? 10001 : 1;
    m_tileGrid.assign(MAP_GRID_H, std::vector<TileType>(MAP_GRID_W, TileType::EMPTY));
    m_gidGrid.assign(MAP_GRID_H, std::vector<int>(MAP_GRID_W, emptyGid));
    m_entities.clear();

    // Default starter ground (first 35 columns)
    int topGroundGid = (m_bgTheme == BackgroundTheme::Castle) ? 10321 : 2744;
    int botGroundGid = (m_bgTheme == BackgroundTheme::Castle) ? 10321 : 2955;

    for (int c = 0; c < 35; ++c) {
        m_tileGrid[14][c] = TileType::GROUND;
        m_gidGrid[14][c]  = topGroundGid;
        m_tileGrid[15][c] = TileType::GROUND;
        m_gidGrid[15][c]  = botGroundGid;
    }

    m_playerSpawnPos = { 40.f, 200.f };
    m_undoStack.clear();
    m_redoStack.clear();
    m_camX = 0.0f;
    m_camY = 0.0f;
}

sf::FloatRect MapEditorState::getCanvasScreenBounds() const
{
    return sf::FloatRect({0.f, TOOLBAR_H}, {WIN_W - PALETTE_W, WIN_H - TOOLBAR_H - STATUS_H});
}

sf::Vector2f MapEditorState::screenToWorld(sf::Vector2f screenPos) const
{
    float canvasX = screenPos.x;
    float canvasY = screenPos.y - TOOLBAR_H;
    float worldX = (canvasX / m_zoom) + m_camX;
    float worldY = (canvasY / m_zoom) + m_camY;
    return { worldX, worldY };
}

sf::Vector2f MapEditorState::worldToScreen(sf::Vector2f worldPos) const
{
    float screenX = (worldPos.x - m_camX) * m_zoom;
    float screenY = (worldPos.y - m_camY) * m_zoom + TOOLBAR_H;
    return { screenX, screenY };
}

std::optional<sf::Vector2i> MapEditorState::screenToGrid(sf::Vector2f screenPos) const
{
    auto bounds = getCanvasScreenBounds();
    if (!bounds.contains(screenPos)) return std::nullopt;

    sf::Vector2f world = screenToWorld(screenPos);
    int gx = static_cast<int>(std::floor(world.x / static_cast<float>(TILE_SIZE)));
    int gy = static_cast<int>(std::floor(world.y / static_cast<float>(TILE_SIZE)));

    if (gx < 0 || gx >= MAP_GRID_W || gy < 0 || gy >= MAP_GRID_H)
        return std::nullopt;

    return sf::Vector2i{ gx, gy };
}

void MapEditorState::pushAction(const EditorAction& action)
{
    m_undoStack.push_back(action);
    if (m_undoStack.size() > MAX_UNDO) {
        m_undoStack.erase(m_undoStack.begin());
    }
    m_redoStack.clear();
}

void MapEditorState::undo()
{
    if (m_undoStack.empty()) return;
    EditorAction action = m_undoStack.back();
    m_undoStack.pop_back();

    switch (action.kind) {
        case EditorAction::Kind::TileChange:
            if (action.gy >= 0 && action.gy < MAP_GRID_H && action.gx >= 0 && action.gx < MAP_GRID_W) {
                m_tileGrid[action.gy][action.gx] = action.oldType;
                m_gidGrid[action.gy][action.gx]  = action.oldGid;
            }
            break;
        case EditorAction::Kind::EntityAdd:
            if (!m_entities.empty()) {
                m_entities.pop_back();
            }
            break;
        case EditorAction::Kind::EntityRemove:
            if (action.entityIdx >= 0 && action.entityIdx <= static_cast<int>(m_entities.size())) {
                m_entities.insert(m_entities.begin() + action.entityIdx, action.entity);
            } else {
                m_entities.push_back(action.entity);
            }
            break;
        case EditorAction::Kind::SpawnMove:
            m_playerSpawnPos = action.oldSpawnPos;
            break;
    }

    m_redoStack.push_back(action);
}

void MapEditorState::redo()
{
    if (m_redoStack.empty()) return;
    EditorAction action = m_redoStack.back();
    m_redoStack.pop_back();

    switch (action.kind) {
        case EditorAction::Kind::TileChange:
            if (action.gy >= 0 && action.gy < MAP_GRID_H && action.gx >= 0 && action.gx < MAP_GRID_W) {
                m_tileGrid[action.gy][action.gx] = action.newType;
                m_gidGrid[action.gy][action.gx]  = action.newGid;
            }
            break;
        case EditorAction::Kind::EntityAdd:
            m_entities.push_back(action.entity);
            break;
        case EditorAction::Kind::EntityRemove:
            if (action.entityIdx >= 0 && action.entityIdx < static_cast<int>(m_entities.size())) {
                m_entities.erase(m_entities.begin() + action.entityIdx);
            }
            break;
        case EditorAction::Kind::SpawnMove:
            m_playerSpawnPos = action.newSpawnPos;
            break;
    }

    m_undoStack.push_back(action);
}

void MapEditorState::placeTile(int gx, int gy, TileType type, int gid)
{
    if (gx < 0 || gx >= MAP_GRID_W || gy < 0 || gy >= MAP_GRID_H) return;
    if (m_tileGrid[gy][gx] == type && m_gidGrid[gy][gx] == gid) return;

    EditorAction action;
    action.kind    = EditorAction::Kind::TileChange;
    action.gx      = gx;
    action.gy      = gy;
    action.oldType = m_tileGrid[gy][gx];
    action.oldGid  = m_gidGrid[gy][gx];
    action.newType = type;
    action.newGid  = gid;

    m_tileGrid[gy][gx] = type;
    m_gidGrid[gy][gx]  = gid;
    pushAction(action);
}

void MapEditorState::eraseAt(int gx, int gy)
{
    if (gx < 0 || gx >= MAP_GRID_W || gy < 0 || gy >= MAP_GRID_H) return;

    // Check entity removal first
    float wx = static_cast<float>(gx * TILE_SIZE + TILE_SIZE / 2);
    float wy = static_cast<float>(gy * TILE_SIZE + TILE_SIZE / 2);

    for (int i = static_cast<int>(m_entities.size()) - 1; i >= 0; --i) {
        if (std::abs(m_entities[i].x - wx) < 12.f && std::abs(m_entities[i].y - wy) < 12.f) {
            EditorAction action;
            action.kind      = EditorAction::Kind::EntityRemove;
            action.entity    = m_entities[i];
            action.entityIdx = i;
            m_entities.erase(m_entities.begin() + i);
            pushAction(action);
            return;
        }
    }

    int emptyGid = (m_bgTheme == BackgroundTheme::Castle) ? 10001 : 1;
    if (m_tileGrid[gy][gx] != TileType::EMPTY || m_gidGrid[gy][gx] != emptyGid) {
        EditorAction action;
        action.kind    = EditorAction::Kind::TileChange;
        action.gx      = gx;
        action.gy      = gy;
        action.oldType = m_tileGrid[gy][gx];
        action.oldGid  = m_gidGrid[gy][gx];
        action.newType = TileType::EMPTY;
        action.newGid  = emptyGid;

        m_tileGrid[gy][gx] = TileType::EMPTY;
        m_gidGrid[gy][gx]  = emptyGid;
        pushAction(action);
    }
}

void MapEditorState::floodFill(int startGx, int startGy, TileType targetType, int targetGid)
{
    if (startGx < 0 || startGx >= MAP_GRID_W || startGy < 0 || startGy >= MAP_GRID_H) return;
    TileType sourceTile = m_tileGrid[startGy][startGx];
    int sourceGid       = m_gidGrid[startGy][startGx];
    if (sourceTile == targetType && sourceGid == targetGid) return;

    std::queue<sf::Vector2i> q;
    std::vector<std::vector<bool>> visited(MAP_GRID_H, std::vector<bool>(MAP_GRID_W, false));

    q.push({ startGx, startGy });
    visited[startGy][startGx] = true;

    while (!q.empty()) {
        auto [gx, gy] = q.front();
        q.pop();

        placeTile(gx, gy, targetType, targetGid);

        const int dx[] = { 1, -1, 0, 0 };
        const int dy[] = { 0, 0, 1, -1 };
        for (int i = 0; i < 4; ++i) {
            int nx = gx + dx[i];
            int ny = gy + dy[i];
            if (nx >= 0 && nx < MAP_GRID_W && ny >= 0 && ny < MAP_GRID_H && !visited[ny][nx]) {
                if (m_tileGrid[ny][nx] == sourceTile && m_gidGrid[ny][nx] == sourceGid) {
                    visited[ny][nx] = true;
                    q.push({ nx, ny });
                }
            }
        }
    }
}

void MapEditorState::placeEntity(int gx, int gy, const std::string& type)
{
    if (gx < 0 || gx >= MAP_GRID_W || gy < 0 || gy >= MAP_GRID_H) return;
    float wx = static_cast<float>(gx * TILE_SIZE + TILE_SIZE / 2);
    float wy = static_cast<float>(gy * TILE_SIZE + TILE_SIZE / 2);

    // Don't place duplicates on the same spot
    for (const auto& e : m_entities) {
        if (std::abs(e.x - wx) < 8.f && std::abs(e.y - wy) < 8.f) return;
    }

    EntitySpawnData entity;
    entity.type = type;
    entity.x = wx;
    entity.y = wy;
    entity.direction = "left";

    m_entities.push_back(entity);

    EditorAction action;
    action.kind = EditorAction::Kind::EntityAdd;
    action.entity = entity;
    pushAction(action);
}

void MapEditorState::movePlayerSpawn(int gx, int gy)
{
    if (gx < 0 || gx >= MAP_GRID_W || gy < 0 || gy >= MAP_GRID_H) return;
    sf::Vector2f newPos = { static_cast<float>(gx * TILE_SIZE + TILE_SIZE / 2),
                            static_cast<float>(gy * TILE_SIZE + TILE_SIZE / 2) };

    if (m_playerSpawnPos == newPos) return;

    EditorAction action;
    action.kind = EditorAction::Kind::SpawnMove;
    action.oldSpawnPos = m_playerSpawnPos;
    action.newSpawnPos = newPos;

    m_playerSpawnPos = newPos;
    pushAction(action);
}

void MapEditorState::applyToolAt(int gx, int gy, bool isDrag)
{
    if (m_activeTool == EditorTool::Eraser) {
        eraseAt(gx, gy);
        return;
    }

    if (m_activeTool == EditorTool::PlayerSpawn) {
        if (!isDrag) movePlayerSpawn(gx, gy);
        return;
    }

    if (m_selectedPaletteIdx < 0 || m_selectedPaletteIdx >= static_cast<int>(m_palette.size())) return;
    const auto& item = m_palette[m_selectedPaletteIdx];

    if (item.isEraser) {
        eraseAt(gx, gy);
    } else if (item.isPlayerSpawn) {
        if (!isDrag) movePlayerSpawn(gx, gy);
    } else if (!item.isTile) {
        if (!isDrag) placeEntity(gx, gy, item.entityType);
    } else {
        if (m_activeTool == EditorTool::Fill) {
            if (!isDrag) floodFill(gx, gy, item.tileType, item.rawGid);
        } else {
            placeTile(gx, gy, item.tileType, item.rawGid);
        }
    }
}

void MapEditorState::launchPlay()
{
    std::string tmpPath = "assets/map/editor_temp.tmx";

    std::ofstream out(tmpPath);
    if (!out.is_open()) {
        std::cerr << "[MapEditorState] Error: Cannot create " << tmpPath << std::endl;
        return;
    }

    int emptyGid = (m_bgTheme == BackgroundTheme::Castle) ? 10001 : 1;

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<map version=\"1.10\" tiledversion=\"1.12.2\""
        << " orientation=\"orthogonal\" renderorder=\"right-down\""
        << " width=\"" << MAP_GRID_W << "\" height=\"" << MAP_GRID_H << "\""
        << " tilewidth=\"16\" tileheight=\"16\" infinite=\"0\">\n";

    out << " <tileset firstgid=\"1\" source=\"../tileset/world1_1.tsx\"/>\n";
    out << " <tileset firstgid=\"7001\" source=\"../tileset/world1_3.tsx\"/>\n";
    out << " <tileset firstgid=\"10001\" source=\"../tileset/world1_4.tsx\"/>\n";

    out << " <layer id=\"1\" name=\"Terrain\" width=\"" << MAP_GRID_W << "\" height=\"" << MAP_GRID_H << "\">\n";
    out << "  <data encoding=\"csv\">\n";
    for (int r = 0; r < MAP_GRID_H; ++r) {
        for (int c = 0; c < MAP_GRID_W; ++c) {
            int gid = m_gidGrid[r][c];
            if (gid <= 0) gid = emptyGid;
            out << gid;
            if (c < MAP_GRID_W - 1) out << ',';
        }
        if (r < MAP_GRID_H - 1) out << ',';
        out << '\n';
    }
    out << "  </data>\n </layer>\n";

    out << " <objectgroup id=\"2\" name=\"Entities\">\n";
    int objId = 1;

    out << "  <object id=\"" << objId++ << "\" name=\"PlayerSpawn\""
        << " x=\"" << m_playerSpawnPos.x << "\" y=\"" << m_playerSpawnPos.y << "\"/>\n";

    for (const auto& e : m_entities) {
        out << "  <object id=\"" << objId++ << "\" name=\"" << e.type << "\""
            << " x=\"" << e.x << "\" y=\"" << e.y << "\"/>\n";
    }

    out << " </objectgroup>\n</map>\n";
    out.close();

    std::cout << "[MapEditorState] Map exported to " << tmpPath << ". Launching PlayState...\n";

    GameConfig playConfig = m_config;
    playConfig.customMapPath = tmpPath;
    playConfig.fromEditor = true;

    if (auto* mgr = getStateManager()) {
        mgr->changeState(std::make_unique<PlayState>(playConfig, m_settings, m_saveManager, false));
    }
}

void MapEditorState::handleInput(const sf::Event& event)
{
    if (const auto* resizeEvent = event.getIf<sf::Event::Resized>()) {
        m_windowSize = resizeEvent->size;
        return;
    }

    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->code == sf::Keyboard::Key::Escape) {
            if (auto* mgr = getStateManager()) {
                mgr->changeState(std::make_unique<LevelSelectState>(m_config, m_settings, m_saveManager, false));
            }
            return;
        }

        // Camera Pan
        if (keyEvent->code == sf::Keyboard::Key::A || keyEvent->code == sf::Keyboard::Key::Left) {
            m_camX = std::max(0.0f, m_camX - 32.0f);
        } else if (keyEvent->code == sf::Keyboard::Key::D || keyEvent->code == sf::Keyboard::Key::Right) {
            m_camX = std::min(static_cast<float>(MAP_GRID_W * TILE_SIZE - 200), m_camX + 32.0f);
        } else if (keyEvent->code == sf::Keyboard::Key::W || keyEvent->code == sf::Keyboard::Key::Up) {
            m_camY = std::max(-50.0f, m_camY - 32.0f);
        } else if (keyEvent->code == sf::Keyboard::Key::S || keyEvent->code == sf::Keyboard::Key::Down) {
            m_camY = std::min(100.0f, m_camY + 32.0f);
        }

        // Tool hotkeys
        if (keyEvent->code == sf::Keyboard::Key::Num1) m_activeTool = EditorTool::Pencil;
        if (keyEvent->code == sf::Keyboard::Key::Num2) m_activeTool = EditorTool::Eraser;
        if (keyEvent->code == sf::Keyboard::Key::Num3) m_activeTool = EditorTool::Fill;
        if (keyEvent->code == sf::Keyboard::Key::Num4) m_activeTool = EditorTool::PlayerSpawn;
        if (keyEvent->code == sf::Keyboard::Key::G)    m_showGrid = !m_showGrid;

        // Background Theme Toggle Hotkey (B)
        if (keyEvent->code == sf::Keyboard::Key::B) {
            m_bgTheme = (m_bgTheme == BackgroundTheme::Sky) ? BackgroundTheme::Castle : BackgroundTheme::Sky;
            int newEmptyGid = (m_bgTheme == BackgroundTheme::Castle) ? 10001 : 1;
            for (int r = 0; r < MAP_GRID_H; ++r) {
                for (int c = 0; c < MAP_GRID_W; ++c) {
                    if (m_tileGrid[r][c] == TileType::EMPTY) {
                        m_gidGrid[r][c] = newEmptyGid;
                    }
                }
            }
        }

        // Undo / Redo
        if (keyEvent->control) {
            if (keyEvent->code == sf::Keyboard::Key::Z) undo();
            if (keyEvent->code == sf::Keyboard::Key::Y) redo();
        }

        // Quick playtest
        if (keyEvent->code == sf::Keyboard::Key::F5) launchPlay();
    }
    else if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos{ static_cast<float>(mouseMoved->position.x), static_cast<float>(mouseMoved->position.y) };

        if (m_isPanning) {
            sf::Vector2f delta = mousePos - m_panStartMouse;
            m_camX = m_panStartCam.x - (delta.x / m_zoom);
            m_camY = m_panStartCam.y - (delta.y / m_zoom);
            m_camX = std::clamp(m_camX, 0.0f, static_cast<float>(MAP_GRID_W * TILE_SIZE - 50));
            m_camY = std::clamp(m_camY, -100.0f, 150.0f);
        }

        m_hoveredGridCell = screenToGrid(mousePos);

        if (m_isPainting && m_hoveredGridCell) {
            applyToolAt(m_hoveredGridCell->x, m_hoveredGridCell->y, true);
        }
    }
    else if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f mousePos{ static_cast<float>(mousePressed->position.x), static_cast<float>(mousePressed->position.y) };

        if (mousePressed->button == sf::Mouse::Button::Middle || mousePressed->button == sf::Mouse::Button::Right) {
            m_isPanning = true;
            m_panStartMouse = mousePos;
            m_panStartCam   = { m_camX, m_camY };
            return;
        }

        if (mousePressed->button == sf::Mouse::Button::Left) {
            // 1. Toolbar area
            if (mousePos.y < TOOLBAR_H) {
                float bx = 6.0f;
                auto checkBtn = [&](float width) {
                    sf::FloatRect rect({ bx, 3.f }, { width, TOOLBAR_H - 6.f });
                    bx += width + 5.0f;
                    return rect.contains(mousePos);
                };

                if (checkBtn(50.f)) m_activeTool = EditorTool::Pencil;
                else if (checkBtn(50.f)) m_activeTool = EditorTool::Eraser;
                else if (checkBtn(40.f)) m_activeTool = EditorTool::Fill;
                else if (checkBtn(50.f)) m_activeTool = EditorTool::PlayerSpawn;
                else if (checkBtn(68.f)) m_showGrid = !m_showGrid;
                else if (checkBtn(78.f)) {
                    // Toggle Background Theme: Sky <-> Castle
                    m_bgTheme = (m_bgTheme == BackgroundTheme::Sky) ? BackgroundTheme::Castle : BackgroundTheme::Sky;
                    int newEmptyGid = (m_bgTheme == BackgroundTheme::Castle) ? 10001 : 1;
                    for (int r = 0; r < MAP_GRID_H; ++r) {
                        for (int c = 0; c < MAP_GRID_W; ++c) {
                            if (m_tileGrid[r][c] == TileType::EMPTY) {
                                m_gidGrid[r][c] = newEmptyGid;
                            }
                        }
                    }
                }
                else if (checkBtn(44.f)) undo();
                else if (checkBtn(44.f)) redo();
                else if (checkBtn(48.f)) resetGrid();
                else if (checkBtn(72.f)) launchPlay();
                else if (checkBtn(50.f)) {
                    if (auto* mgr = getStateManager()) {
                        mgr->changeState(std::make_unique<LevelSelectState>(m_config, m_settings, m_saveManager, false));
                    }
                }
                return;
            }

            // 2. Palette area
            if (mousePos.x >= WIN_W - PALETTE_W) {
                // Category tabs at y = TOOLBAR_H + 4 .. TOOLBAR_H + 26
                if (mousePos.y >= TOOLBAR_H && mousePos.y <= TOOLBAR_H + 26.f) {
                    float tabW = PALETTE_W / static_cast<float>(m_categories.size());
                    int clickedCat = static_cast<int>((mousePos.x - (WIN_W - PALETTE_W)) / tabW);
                    if (clickedCat >= 0 && clickedCat < static_cast<int>(m_categories.size())) {
                        m_selectedCategoryIdx = clickedCat;
                        m_paletteScrollY = 0.f;
                    }
                    return;
                }

                // Palette items list
                float listTopY = TOOLBAR_H + 30.f;
                float curY = listTopY - m_paletteScrollY;
                for (size_t i = 0; i < m_palette.size(); ++i) {
                    const auto& item = m_palette[i];
                    if (m_selectedCategoryIdx != 0 && item.category != m_categories[m_selectedCategoryIdx])
                        continue;

                    sf::FloatRect cardRect({ WIN_W - PALETTE_W + 6.f, curY }, { PALETTE_W - 12.f, 22.f });
                    if (cardRect.contains(mousePos)) {
                        m_selectedPaletteIdx = static_cast<int>(i);
                        if (item.isEraser) m_activeTool = EditorTool::Eraser;
                        else if (item.isPlayerSpawn) m_activeTool = EditorTool::PlayerSpawn;
                        else m_activeTool = EditorTool::Pencil;
                        return;
                    }
                    curY += 26.f;
                }
                return;
            }

            // 3. Canvas area
            auto cell = screenToGrid(mousePos);
            if (cell) {
                m_isPainting = true;
                applyToolAt(cell->x, cell->y, false);
            }
        }
    }
    else if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Left) {
            m_isPainting = false;
        }
        if (mouseReleased->button == sf::Mouse::Button::Middle || mouseReleased->button == sf::Mouse::Button::Right) {
            m_isPanning = false;
        }
    }
    else if (const auto* scrollEvent = event.getIf<sf::Event::MouseWheelScrolled>()) {
        sf::Vector2f mousePos{ static_cast<float>(scrollEvent->position.x), static_cast<float>(scrollEvent->position.y) };

        if (mousePos.x >= WIN_W - PALETTE_W) {
            m_paletteScrollY = std::max(0.0f, m_paletteScrollY - scrollEvent->delta * 24.0f);
        } else if (mousePos.y >= TOOLBAR_H && mousePos.y <= WIN_H - STATUS_H) {
            float oldZoom = m_zoom;
            float zoomDelta = (scrollEvent->delta > 0) ? 0.25f : -0.25f;
            m_zoom = std::clamp(m_zoom + zoomDelta, 1.0f, 4.0f);

            // Zoom centered on cursor
            float cursorCanvasX = mousePos.x;
            float cursorWorldX = (cursorCanvasX / oldZoom) + m_camX;
            m_camX = cursorWorldX - (cursorCanvasX / m_zoom);
            m_camX = std::clamp(m_camX, 0.0f, static_cast<float>(MAP_GRID_W * TILE_SIZE - 50));
        }
    }
}

void MapEditorState::update(float deltaTime)
{
    // Runtime update
}

void MapEditorState::render(sf::RenderWindow& window) const
{
    window.clear(sf::Color(20, 20, 24));

    renderCanvas(window);
    renderToolbar(window);
    renderPalette(window);
    renderStatusBar(window);
}

void MapEditorState::renderCanvas(sf::RenderWindow& window) const
{
    auto bounds = getCanvasScreenBounds();

    // Canvas background based on selected BG Theme (Sky vs Castle)
    sf::RectangleShape bg({ bounds.size.x, bounds.size.y });
    bg.setPosition({ bounds.position.x, bounds.position.y });
    if (m_bgTheme == BackgroundTheme::Sky) {
        bg.setFillColor(sf::Color(92, 148, 252)); // Classic Super Mario Sky Blue
    } else {
        bg.setFillColor(sf::Color(10, 10, 14));   // Dark Castle Black
    }
    window.draw(bg);

    // Visible column/row range
    float cellPx = static_cast<float>(TILE_SIZE) * m_zoom;
    int firstCol = std::max(0, static_cast<int>(std::floor(m_camX / static_cast<float>(TILE_SIZE))));
    int lastCol  = std::min(MAP_GRID_W - 1, static_cast<int>(std::ceil((m_camX + bounds.size.x / m_zoom) / static_cast<float>(TILE_SIZE))));

    int firstRow = std::max(0, static_cast<int>(std::floor(m_camY / static_cast<float>(TILE_SIZE))));
    int lastRow  = std::min(MAP_GRID_H - 1, static_cast<int>(std::ceil((m_camY + bounds.size.y / m_zoom) / static_cast<float>(TILE_SIZE))));

    // Draw tiles
    for (int r = firstRow; r <= lastRow; ++r) {
        for (int c = firstCol; c <= lastCol; ++c) {
            int gid = m_gidGrid[r][c];
            TileType type = m_tileGrid[r][c];

            sf::Vector2f worldPos{ static_cast<float>(c * TILE_SIZE), static_cast<float>(r * TILE_SIZE) };
            sf::Vector2f screenPos = worldToScreen(worldPos);

            // If empty and matches background theme, skip drawing explicit box (shows canvas BG)
            if (type == TileType::EMPTY) {
                if ((m_bgTheme == BackgroundTheme::Sky && gid == 1) ||
                    (m_bgTheme == BackgroundTheme::Castle && gid == 10001)) {
                    continue;
                }
                // Custom empty tile (e.g. Castle empty on Sky background or vice-versa)
                sf::RectangleShape emptyShape({ cellPx, cellPx });
                emptyShape.setPosition({ screenPos.x, screenPos.y });
                emptyShape.setFillColor((gid >= 10000 || gid == 7001) ? sf::Color(10, 10, 14) : sf::Color(92, 148, 252));
                window.draw(emptyShape);
                continue;
            }

            // Draw solid / active tiles
            sf::RectangleShape tileShape({ cellPx, cellPx });
            tileShape.setPosition({ screenPos.x, screenPos.y });

            sf::Color fill = sf::Color(120, 120, 120);
            if (type == TileType::GROUND) {
                if (gid >= 10000)      fill = sf::Color(130, 130, 130); // Castle Ground
                else if (gid >= 7000)  fill = sf::Color(60, 120, 180);  // Underground Ground
                else if (gid == 2955)  fill = sf::Color(150, 85, 35);   // Overworld Dirt
                else                   fill = sf::Color(190, 115, 55);  // Overworld Top Ground
            }
            else if (type == TileType::SOLID_BRICK) {
                if (gid >= 10000)     fill = sf::Color(100, 100, 100);
                else if (gid >= 7000) fill = sf::Color(40, 80, 140);
                else                  fill = sf::Color(140, 70, 20);
            }
            else if (type == TileType::BRICK_SOLID || type == TileType::BRICK_EMPTY) {
                fill = (gid >= 7000 && gid < 10000) ? sf::Color(50, 100, 160) : sf::Color(200, 100, 40);
            }
            else if (type == TileType::QUESTION_COIN || type == TileType::QUESTION_POWERUP || type == TileType::MULTI_COIN) {
                fill = sf::Color(255, 190, 0);
            }
            else if (type == TileType::PIPE) {
                fill = (gid >= 7000 && gid < 10000) ? sf::Color(60, 150, 200) : sf::Color(40, 180, 40);
            }
            else if (type == TileType::FLAGPOLE) {
                fill = sf::Color(210, 210, 210);
            }
            else if (type == TileType::FIRE_BAR) {
                fill = sf::Color(255, 50, 0);
            }

            tileShape.setFillColor(fill);
            tileShape.setOutlineThickness(0.5f);
            tileShape.setOutlineColor(sf::Color(255, 255, 255, 60));
            window.draw(tileShape);
        }
    }

    // Grid overlay (if enabled)
    if (m_showGrid) {
        sf::VertexArray lines(sf::PrimitiveType::Lines);
        sf::Color gridCol = (m_bgTheme == BackgroundTheme::Sky) ? sf::Color(65, 110, 200, 140) : sf::Color(55, 60, 75, 180);

        for (int c = firstCol; c <= lastCol + 1; ++c) {
            float sx = worldToScreen({ static_cast<float>(c * TILE_SIZE), 0.f }).x;
            if (sx >= bounds.position.x && sx <= bounds.position.x + bounds.size.x) {
                float topY = worldToScreen({ 0.f, static_cast<float>(firstRow * TILE_SIZE) }).y;
                float botY = worldToScreen({ 0.f, static_cast<float>((lastRow + 1) * TILE_SIZE) }).y;
                lines.append(sf::Vertex{ sf::Vector2f{ sx, std::max(bounds.position.y, topY) }, gridCol });
                lines.append(sf::Vertex{ sf::Vector2f{ sx, std::min(bounds.position.y + bounds.size.y, botY) }, gridCol });
            }
        }
        for (int r = firstRow; r <= lastRow + 1; ++r) {
            float sy = worldToScreen({ 0.f, static_cast<float>(r * TILE_SIZE) }).y;
            if (sy >= bounds.position.y && sy <= bounds.position.y + bounds.size.y) {
                float leftX  = worldToScreen({ static_cast<float>(firstCol * TILE_SIZE), 0.f }).x;
                float rightX = worldToScreen({ static_cast<float>((lastCol + 1) * TILE_SIZE), 0.f }).x;
                lines.append(sf::Vertex{ sf::Vector2f{ std::max(bounds.position.x, leftX), sy }, gridCol });
                lines.append(sf::Vertex{ sf::Vector2f{ std::min(bounds.position.x + bounds.size.x, rightX), sy }, gridCol });
            }
        }
        window.draw(lines);
    }

    // Draw Entity Spawn Badges
    for (const auto& e : m_entities) {
        sf::Vector2f screenPos = worldToScreen({ e.x, e.y });
        if (screenPos.x >= bounds.position.x - 20.f && screenPos.x <= bounds.position.x + bounds.size.x + 20.f) {
            sf::RectangleShape marker({ 14.f * m_zoom, 14.f * m_zoom });
            marker.setOrigin({ 7.f * m_zoom, 7.f * m_zoom });
            marker.setPosition({ screenPos.x, screenPos.y });

            sf::Color fill = (e.type == "Goomba") ? sf::Color(180, 90, 20, 220) :
                             (e.type == "KoopaTroopa") ? sf::Color(30, 180, 80, 220) :
                             (e.type == "Star") ? sf::Color(255, 230, 0, 220) : sf::Color(80, 100, 220, 220);

            marker.setFillColor(fill);
            marker.setOutlineColor(sf::Color::White);
            marker.setOutlineThickness(1.f);
            window.draw(marker);

            if (m_fontLoaded) {
                sf::Text label(m_font, e.type, 9);
                label.setFillColor(sf::Color::White);
                label.setPosition({ screenPos.x - 12.f, screenPos.y - 18.f });
                window.draw(label);
            }
        }
    }

    // Draw PlayerSpawn marker (Tiled-style badge)
    sf::Vector2f spawnScreen = worldToScreen(m_playerSpawnPos);
    if (spawnScreen.x >= bounds.position.x - 30.f && spawnScreen.x <= bounds.position.x + bounds.size.x + 30.f) {
        sf::RectangleShape spawnBox({ cellPx, cellPx });
        spawnBox.setOrigin({ cellPx / 2.f, cellPx / 2.f });
        spawnBox.setPosition({ spawnScreen.x, spawnScreen.y });
        spawnBox.setFillColor(sf::Color(0, 210, 255, 60));
        spawnBox.setOutlineColor(sf::Color(0, 220, 255, 240));
        spawnBox.setOutlineThickness(1.5f);
        window.draw(spawnBox);

        if (m_fontLoaded) {
            sf::RectangleShape badge({ 68.f, 13.f });
            badge.setOrigin({ 34.f, 6.5f });
            badge.setPosition({ spawnScreen.x, spawnScreen.y - cellPx / 2.f - 8.f });
            badge.setFillColor(sf::Color(60, 65, 75, 240));
            badge.setOutlineColor(sf::Color(140, 145, 160));
            badge.setOutlineThickness(1.f);
            window.draw(badge);

            sf::Text label(m_font, "PlayerSpawn", 8);
            label.setFillColor(sf::Color(240, 245, 255));
            label.setOrigin({ 28.f, 5.f });
            label.setPosition({ spawnScreen.x, spawnScreen.y - cellPx / 2.f - 8.f });
            window.draw(label);
        }
    }

    // Hovered Grid Cell Highlight
    if (m_hoveredGridCell) {
        sf::Vector2f worldPos{ static_cast<float>(m_hoveredGridCell->x * TILE_SIZE),
                               static_cast<float>(m_hoveredGridCell->y * TILE_SIZE) };
        sf::Vector2f screenPos = worldToScreen(worldPos);

        sf::RectangleShape hover({ cellPx, cellPx });
        hover.setPosition({ screenPos.x, screenPos.y });
        hover.setFillColor(sf::Color(255, 230, 60, 50));
        hover.setOutlineColor(sf::Color(255, 235, 60, 240));
        hover.setOutlineThickness(1.5f);
        window.draw(hover);
    }
}

void MapEditorState::renderToolbar(sf::RenderWindow& window) const
{
    sf::RectangleShape bar({ WIN_W, TOOLBAR_H });
    bar.setPosition({ 0.f, 0.f });
    bar.setFillColor(sf::Color(28, 30, 36));
    bar.setOutlineColor(sf::Color(55, 60, 72));
    bar.setOutlineThickness(1.f);
    window.draw(bar);

    float bx = 6.0f;
    auto drawBtn = [&](const std::string& label, float width, bool active, sf::Color bgCol = sf::Color(45, 48, 58)) {
        sf::RectangleShape btn({ width, TOOLBAR_H - 6.f });
        btn.setPosition({ bx, 3.f });
        btn.setFillColor(active ? sf::Color(65, 110, 190) : bgCol);
        btn.setOutlineColor(active ? sf::Color(140, 210, 255) : sf::Color(70, 76, 90));
        btn.setOutlineThickness(1.f);
        window.draw(btn);

        if (m_fontLoaded) {
            sf::Text text(m_font, label, 9);
            text.setFillColor(active ? sf::Color::White : sf::Color(210, 220, 235));
            text.setPosition({ bx + 4.f, 7.f });
            window.draw(text);
        }
        bx += width + 5.0f;
    };

    drawBtn("Pencil", 50.f, m_activeTool == EditorTool::Pencil);
    drawBtn("Eraser", 50.f, m_activeTool == EditorTool::Eraser);
    drawBtn("Fill",   40.f, m_activeTool == EditorTool::Fill);
    drawBtn("Spawn",  50.f, m_activeTool == EditorTool::PlayerSpawn);

    std::string gridLabel = m_showGrid ? "Grid: ON" : "Grid: OFF";
    drawBtn(gridLabel, 68.f, m_showGrid, m_showGrid ? sf::Color(35, 75, 70) : sf::Color(45, 48, 58));

    std::string bgLabel = (m_bgTheme == BackgroundTheme::Sky) ? "BG: Sky" : "BG: Castle";
    drawBtn(bgLabel, 78.f, true, (m_bgTheme == BackgroundTheme::Sky) ? sf::Color(40, 85, 160) : sf::Color(55, 55, 65));

    drawBtn("Undo",   44.f, false);
    drawBtn("Redo",   44.f, false);
    drawBtn("Clear",  48.f, false, sf::Color(90, 45, 45));
    drawBtn("> PLAY", 72.f, true, sf::Color(35, 140, 50));
    drawBtn("< Exit", 50.f, false, sf::Color(75, 40, 40));
}

void MapEditorState::renderPalette(sf::RenderWindow& window) const
{
    float px = WIN_W - PALETTE_W;
    float py = TOOLBAR_H;
    float ph = WIN_H - TOOLBAR_H - STATUS_H;

    sf::RectangleShape panel({ PALETTE_W, ph });
    panel.setPosition({ px, py });
    panel.setFillColor(sf::Color(24, 26, 32));
    panel.setOutlineColor(sf::Color(55, 60, 72));
    panel.setOutlineThickness(1.f);
    window.draw(panel);

    // Category Tabs Header
    float tabW = PALETTE_W / static_cast<float>(m_categories.size());
    for (size_t i = 0; i < m_categories.size(); ++i) {
        bool isSel = (static_cast<int>(i) == m_selectedCategoryIdx);
        sf::RectangleShape tab({ tabW, 22.f });
        tab.setPosition({ px + static_cast<float>(i) * tabW, py + 2.f });
        tab.setFillColor(isSel ? sf::Color(55, 95, 160) : sf::Color(36, 40, 50));
        tab.setOutlineColor(isSel ? sf::Color(140, 210, 255) : sf::Color(50, 55, 68));
        tab.setOutlineThickness(1.f);
        window.draw(tab);

        if (m_fontLoaded) {
            std::string shortName = m_categories[i].substr(0, 4);
            sf::Text tabText(m_font, shortName, 8);
            tabText.setFillColor(isSel ? sf::Color::White : sf::Color(170, 180, 200));
            tabText.setPosition({ px + static_cast<float>(i) * tabW + 3.f, py + 5.f });
            window.draw(tabText);
        }
    }

    // Palette Items Scrollable List
    float listTopY = py + 28.f;
    float curY = listTopY - m_paletteScrollY;

    for (size_t i = 0; i < m_palette.size(); ++i) {
        const auto& item = m_palette[i];
        if (m_selectedCategoryIdx != 0 && item.category != m_categories[m_selectedCategoryIdx])
            continue;

        if (curY >= py + 24.f && curY <= py + ph - 22.f) {
            bool isSel = (static_cast<int>(i) == m_selectedPaletteIdx);

            sf::RectangleShape card({ PALETTE_W - 12.f, 22.f });
            card.setPosition({ px + 6.f, curY });
            card.setFillColor(isSel ? sf::Color(45, 75, 130) : sf::Color(32, 35, 44));
            card.setOutlineColor(isSel ? sf::Color(255, 225, 60) : sf::Color(50, 55, 68));
            card.setOutlineThickness(isSel ? 1.5f : 1.f);
            window.draw(card);

            sf::RectangleShape swatch({ 14.f, 14.f });
            swatch.setPosition({ px + 10.f, curY + 4.f });
            swatch.setFillColor(item.previewColor);
            swatch.setOutlineColor(sf::Color::White);
            swatch.setOutlineThickness(0.5f);
            window.draw(swatch);

            if (m_fontLoaded) {
                sf::Text nameText(m_font, item.name, 9);
                nameText.setFillColor(isSel ? sf::Color(255, 245, 140) : sf::Color(215, 225, 240));
                nameText.setPosition({ px + 30.f, curY + 3.f });
                window.draw(nameText);
            }
        }
        curY += 26.f;
    }
}

void MapEditorState::renderStatusBar(sf::RenderWindow& window) const
{
    float sy = WIN_H - STATUS_H;

    sf::RectangleShape bar({ WIN_W, STATUS_H });
    bar.setPosition({ 0.f, sy });
    bar.setFillColor(sf::Color(18, 20, 24));
    bar.setOutlineColor(sf::Color(48, 52, 62));
    bar.setOutlineThickness(1.f);
    window.draw(bar);

    if (m_fontLoaded) {
        std::string toolName = (m_activeTool == EditorTool::Pencil) ? "Pencil" :
                               (m_activeTool == EditorTool::Eraser) ? "Eraser" :
                               (m_activeTool == EditorTool::Fill) ? "Fill" : "PlayerSpawn";

        std::string selName = (m_selectedPaletteIdx >= 0 && m_selectedPaletteIdx < static_cast<int>(m_palette.size()))
                              ? m_palette[m_selectedPaletteIdx].name : "None";

        std::string bgStr = (m_bgTheme == BackgroundTheme::Sky) ? "Sky" : "Castle";

        std::string status = "Tool: " + toolName + " | Selected: " + selName + " | BG: " + bgStr;
        if (m_hoveredGridCell) {
            status += " | Cell: (" + std::to_string(m_hoveredGridCell->x) + "," + std::to_string(m_hoveredGridCell->y) + ")";
        }
        status += " | 200x16 | [B: Toggle BG] [F5: PLAY]";

        sf::Text text(m_font, status, 8);
        text.setFillColor(sf::Color(160, 175, 200));
        text.setPosition({ 6.f, sy + 4.f });
        window.draw(text);
    }
}
