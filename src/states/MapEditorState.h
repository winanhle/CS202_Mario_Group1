#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include "../entities/map/MapData.h"
#include "../entities/map/MapManager.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <optional>

class ISettingsManager;
class ISaveManager;
class ISoundManager;

/**
 * @enum EditorTool
 * @brief Tool modes in the map editor.
 */
enum class EditorTool {
    Pencil,
    Eraser,
    Fill,
    PlayerSpawn
};

/**
 * @enum BackgroundTheme
 * @brief Empty/Background block type: Sky (Stage 1/2) vs Castle (Stage 3).
 */
enum class BackgroundTheme {
    Sky,    // Overworld & Underground Sky (GID 1)
    Castle  // Castle Dark/Empty (GID 10001)
};

/**
 * @struct PaletteItem
 * @brief Represents a placeable tile or entity in the editor palette.
 */
struct PaletteItem {
    std::string name;
    std::string category;     // "Overworld", "Underground", "Castle", "Entities"
    bool        isTile        = true;
    TileType    tileType      = TileType::EMPTY;
    int         rawGid        = 0;
    std::string entityType    = "";
    bool        isPlayerSpawn = false;
    bool        isEraser      = false;
    sf::Color   previewColor  = sf::Color::White;
};

/**
 * @struct EditorAction
 * @brief Undoable editor action (Command pattern).
 */
struct EditorAction {
    enum class Kind {
        TileChange,
        EntityAdd,
        EntityRemove,
        SpawnMove
    } kind = Kind::TileChange;

    int      gx       = 0;
    int      gy       = 0;
    TileType oldType  = TileType::EMPTY;
    TileType newType  = TileType::EMPTY;
    int      oldGid   = 0;
    int      newGid   = 0;

    EntitySpawnData entity;
    int             entityIdx = -1;

    sf::Vector2f    oldSpawnPos{40.f, 200.f};
    sf::Vector2f    newSpawnPos{40.f, 200.f};
};

/**
 * @class MapEditorState
 * @brief Utilitarian, Tiled-like in-game map editor state.
 *
 * Features:
 * - 200x16 tile grid canvas with pan (WASD / Right/Middle drag) and zoom.
 * - Visible grid-snapping overlay toggleable with a toolbar button.
 * - Docked right palette with categorized blocks from 3 stage tilesets + entities.
 * - Flat-bordered UI, high-contrast active states.
 * - Instant playtesting via [ Play ] button, with automatic return to editor on death or clear.
 */
class MapEditorState : public GameState
{
public:
    explicit MapEditorState(const GameConfig& config,
                            std::shared_ptr<ISettingsManager> settings,
                            std::shared_ptr<ISaveManager> saveManager,
                            std::shared_ptr<ISoundManager> soundManager = nullptr);
    ~MapEditorState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void initPalette();
    void resetGrid();

    // Coordinate conversions
    sf::Vector2f screenToWorld(sf::Vector2f screenPos) const;
    sf::Vector2f worldToScreen(sf::Vector2f worldPos) const;
    std::optional<sf::Vector2i> screenToGrid(sf::Vector2f screenPos) const;
    sf::FloatRect getCanvasScreenBounds() const;

    // Canvas actions
    void applyToolAt(int gx, int gy, bool isDrag = false);
    void placeTile(int gx, int gy, TileType type, int gid);
    void eraseAt(int gx, int gy);
    void floodFill(int gx, int gy, TileType targetType, int targetGid);
    void placeEntity(int gx, int gy, const std::string& type);
    void movePlayerSpawn(int gx, int gy);
    void removeNearestEntity(int gx, int gy);

    // Undo / Redo
    void pushAction(const EditorAction& action);
    void undo();
    void redo();

    // Playtest launch
    void launchPlay();

    // Render helpers
    void renderCanvas(sf::RenderWindow& window) const;
    void renderToolbar(sf::RenderWindow& window) const;
    void renderPalette(sf::RenderWindow& window) const;
    void renderStatusBar(sf::RenderWindow& window) const;

    // UI layout constants
    static constexpr float WIN_W       = 800.0f;
    static constexpr float WIN_H       = 600.0f;
    static constexpr float TOOLBAR_H   = 30.0f;
    static constexpr float STATUS_H    = 22.0f;
    static constexpr float PALETTE_W   = 210.0f;
    static constexpr int   MAP_GRID_W  = 200;
    static constexpr int   MAP_GRID_H  = 16;
    static constexpr int   TILE_SIZE   = 16;

    // Editor data
    std::shared_ptr<MapManager> m_mapManager;
    std::vector<std::vector<TileType>> m_tileGrid;
    std::vector<std::vector<int>>      m_gidGrid;
    std::vector<EntitySpawnData>       m_entities;
    sf::Vector2f                       m_playerSpawnPos{40.f, 200.f};

    // Camera / Viewport
    float m_camX        = 0.0f;
    float m_camY        = 0.0f;
    float m_zoom        = 2.0f; // Scale factor: 2.0 = 32px per tile cell
    bool  m_showGrid    = true; // Grid lines toggle
    bool  m_isPanning   = false;
    sf::Vector2f m_panStartMouse{0.f, 0.f};
    sf::Vector2f m_panStartCam{0.f, 0.f};

    // Background Theme (EMPTY block style: Sky vs Castle)
    BackgroundTheme m_bgTheme = BackgroundTheme::Sky;

    // Tool & Interaction
    EditorTool m_activeTool     = EditorTool::Pencil;
    bool       m_isPainting     = false;
    std::optional<sf::Vector2i> m_hoveredGridCell;

    // Palette data
    std::vector<PaletteItem> m_palette;
    std::vector<std::string> m_categories;
    int                      m_selectedCategoryIdx = 0;
    int                      m_selectedPaletteIdx  = 0;
    float                    m_paletteScrollY      = 0.0f;

    // Undo / Redo history
    std::vector<EditorAction> m_undoStack;
    std::vector<EditorAction> m_redoStack;
    static constexpr size_t MAX_UNDO = 200;

    // Fonts & Visuals
    sf::Font m_font;
    bool     m_fontLoaded = false;
    mutable sf::Vector2u m_windowSize{800u, 600u};

    // State Dependencies
    GameConfig                        m_config;
    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager>     m_saveManager;
    std::shared_ptr<ISoundManager>    m_soundManager;
};
