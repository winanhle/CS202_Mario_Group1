#pragma once
#include "../../interfaces/IPlayerManager.h"
#include "../../interfaces/IMapManager.h"
#include "../../interfaces/IMapContext.h"
#include "../../interfaces/IItemManager.h"
#include "../../interfaces/IEnemyManager.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <utility>
#include <optional>
#include <SFML/Graphics.hpp>
#include "../../tinyxml2.h"
#include "block/IBlockBehavior.h"
#include "MapData.h"
#include "MapEdit.h"

// ─── Animation Structs ────────────────────────────────────────────────────────

/**
 * @struct BrickDebris
 * @brief One of the 4 quarter-pieces that fly out when a BRICK_NORMAL is broken.
 */
struct BrickDebris {
    sf::Vector2f pos;      // current world position
    sf::Vector2f vel;      // pixel/s  velocity (includes upward launch + gravity)
    sf::Color    color;    // tint (matches the brick colour)
    float        life;     // remaining lifetime in seconds
    float        maxLife;  // used to fade alpha
    sf::Vector2f size;     // width/height of the quarter piece
};

/**
 * @struct CoinPopAnim
 * @brief Visual coin that pops upward when a ?-block or multi-coin is hit.
 *        Score is awarded immediately; this is purely cosmetic.
 */
struct CoinPopAnim {
    sf::Vector2f pos;
    float        velY;      // pixels/s, negative = moving up
    float        life;      // remaining lifetime in seconds
};

/**
 * @struct FlagSlideAnim
 * @brief Sliding flag animation on the goal flagpole.
 */
struct FlagSlideAnim {
    sf::Vector2f pos{0.f, 0.f};
    float        targetY  = 0.f;
    float        speed    = 90.f;
    bool         active   = false;
    bool         finished = false;
};

/**
 * @struct BlockBumpAnim
 * @brief Animation for blocks bumping up by half a tile (8px) when hit from below.
 */
struct BlockBumpAnim {
    int gx = 0;
    int gy = 0;
    float time = 0.f;
    static constexpr float DURATION = 0.18f;  // seconds
    static constexpr float MAX_OFFSET = -8.f; // 8 pixels upward (half a 16px tile)
};

// ─── State Structs ────────────────────────────────────────────────────────────

/**
 * @struct MultiCoinState
 * @brief Tracks the 3.5 s countdown for a MULTI_COIN block.
 */
struct MultiCoinState {
    float timer     = 0.f;   // counts down from MULTI_COIN_DURATION → 0
    float animTimer = 0.f;   // accumulates deltaTime to toggle between MULTI_COIN and MULTI_COIN2
    bool  isFrame2  = false; // toggles between false (MULTI_COIN) and true (MULTI_COIN2)
    bool  active    = false; // true while countdown is running
};

/**
 * @class MapManager
 * @brief Implements IMapManager (public game API) and IMapContext (block-behavior API).
 *
 * IMapContext is a narrow interface that block behaviors use to make tile
 * transitions and spawn side-effects without depending on the full MapManager
 * concrete class (Dependency-Inversion Principle).
 */
class MapManager : public IMapManager, public IMapContext {
private:
    std::vector<std::vector<TileType>> m_mapData;
    // Raw GID grid — mirrors m_mapData but stores the original Tiled GID
    // (0 = no tile).  Used to compute UV rects into the tileset sprite-sheet.
    std::vector<std::vector<int>> m_rawGids;
    // Background GID cache — stores the GID that was visually beneath each
    // interactive tile before it was overwritten by a higher layer.
    // When setTile(EMPTY) is called (e.g. brick breaks), this is restored
    // into m_rawGids so the correct background (sky, underground, etc.) shows.
    std::vector<std::vector<int>> m_bgGids;

    int m_tileSize = 16;

    // ─── Injected dependencies ────────────────────────────────────────────────
    IItemManager*  m_itemManager  = nullptr;
    IEnemyManager* m_enemyManager = nullptr;

    // ─── Parsed object layer data ─────────────────────────────────────────────
    MapObjectData m_objectData;

    // ─── MULTI_COIN per-tile state ────────────────────────────────────────────
    std::map<std::pair<int,int>, MultiCoinState> m_multiCoinStates;
    static constexpr float MULTI_COIN_DURATION = 3.5f;

    // ─── Live animations ──────────────────────────────────────────────────────
    std::vector<BrickDebris>    m_brickDebris;
    std::vector<CoinPopAnim>    m_coinPopAnims;
    std::vector<BlockBumpAnim>  m_blockBumpAnims;
    FlagSlideAnim               m_flagAnim;

    // ─── Tileset sprite-sheet rendering ──────────────────────────────────────
    sf::Texture                  m_tilesetTexture;  // PNG loaded from the TSX <image> node
    mutable std::optional<sf::Sprite> m_tileSprite; // absent until texture is ready (SFML 3 has no default ctor)
    bool m_textureLoaded = false;                   // true once the texture loaded successfully

    // ─── TSX metadata ─────────────────────────────────────────────────────────
    std::string m_tsxRelativePath; // relative path used in the TMX <tileset source="...">
    std::string m_tmxPath;         // path of the last successfully loaded TMX (for saveToTMX)

    static constexpr float DEBRIS_LIFE     = 0.7f;  // seconds
    static constexpr float COINPOP_LIFE    = 0.55f; // seconds
    static constexpr float COINPOP_INIT_VY = -200.f;// px/s upward
    static constexpr float DEBRIS_GRAVITY  = 600.f; // px/s²
    static constexpr float COINPOP_GRAVITY = 400.f;

    // ─── Map Editor undo/redo stacks ──────────────────────────────────────────
    std::vector<TileEdit> m_undoStack;
    std::vector<TileEdit> m_redoStack;

    // ─── Pending Warp Request ────────────────────────────────────────────────
    std::optional<WarpRequest> m_pendingWarp;

    // ─── CSV loader (kept as fallback) ───────────────────────────────────
    bool loadMapCSV(const std::string& filepath);

    // ─── TMX / TSX loaders ─────────────────────────────────────────────
    bool loadMapTMX(const std::string& tmxPath);
    // Parses the external .tsx tileset file and populates m_gidTypeMap
    bool loadTileset(const std::string& tsxPath, int firstGid);
    // Parses a <tileset> XML element (used for both external .tsx and inline <tileset>)
    bool parseTilesetElement(tinyxml2::XMLElement* root, const std::string& baseDir, int firstGid);
    // Converts a GID integer to the corresponding TileType
    TileType gidToTileType(int gid) const;
    // Converts the Tiled propertytype / value string to TileType
    static TileType stringToTileType(const std::string& s);
    // Resolves a path relative to a base directory
    static std::string resolvePath(const std::string& baseDir, const std::string& relativePath);

    // Parses <objectgroup> layers from TMX for enemy/player spawn points
    void parseObjectGroups(tinyxml2::XMLElement* mapElement);

    // GID → TileType lookup table (populated by loadTileset / parseTilesetElement)
    std::unordered_map<int, TileType> m_gidTypeMap;
    // TileType → GID lookup table (reverse of m_gidTypeMap).
    std::unordered_map<TileType, int> m_typeToGid;

    struct LoadedTileset {
        int firstGid = 1;
        int columns  = 0;
    };
    std::vector<LoadedTileset> m_tilesets;

    int m_tilesetFirstGid  = 1;  // firstgid attribute from <tileset> element
    int m_tilesetColumns   = 0;  // columns of the tileset sprite sheet

    // ─── Internal tile write (no undo recording) ──────────────────────────────
    // Used by game logic (block behaviors, MultiCoin countdown, etc.).
    // Editor operations use editTile() which records to m_undoStack.
    void setTileInternal(int gx, int gy, TileType type);

public:
    MapManager();
    ~MapManager() override = default;

    // Các hàm được override từ IMapManager
    void initialize() override;
    void loadMap(const std::string& tmxPath) override;
    const MapObjectData& getMapObjectData() const override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    // Hàm phục vụ xử lý va chạm cho Player/Enemy
    bool isSolid(float x, float y) const override;
    bool isSolidFromBelow(float x, float y) const override;
    int getTileSize() const override { return m_tileSize; }
    sf::Vector2u getMapPixelSize() const override {
        if (m_mapData.empty() || m_mapData[0].empty()) return sf::Vector2u(0, 0);
        return sf::Vector2u(
            static_cast<unsigned>(m_mapData[0].size()) * m_tileSize,
            static_cast<unsigned>(m_mapData.size()) * m_tileSize
        );
    }

    // Hàm mở rộng: Lấy chính xác loại Tile để xử lý đụng đầu
    TileType getTileType(float x, float y) const override;

    // IMapManager — grid-coord query for editor
    TileType getTileTypeAt(int gx, int gy) const override;

    // ─── Block transition API (IMapManager) ───────────────────────────────────
    void onHitFromBelow(int tileGridX, int tileGridY, IPlayerManager* player) override;
    void onStandingOn(int tileGridX, int tileGridY, IPlayerManager* player) override;
    void onSideTouch(int tileGridX, int tileGridY, IPlayerManager* player) override;
    bool hasPendingWarp() const override { return m_pendingWarp.has_value(); }
    WarpRequest consumePendingWarp() override;
    void setItemManager(IItemManager* itemManager) override  { m_itemManager  = itemManager; }
    void setEnemyManager(IEnemyManager* enemyManager) override { m_enemyManager = enemyManager; }

    // ─── IMapContext implementation (called by IBlockBehavior subclasses) ─────
    void setTile(int gx, int gy, TileType type) override;      // wraps setTileInternal
    void spawnBrickDebris(int gx, int gy) override;
    void spawnCoinPop(int gx, int gy) override;
    void spawnItemForFormType(int gx, int gy, int formType) override;
    void setMultiCoinActive(int gx, int gy) override;
    void killEnemiesAboveTile(int gx, int gy) override;
    void spawnBlockBump(int gx, int gy, TileType finalType) override;
    void requestWarp(const std::string& targetMap, float targetX = -1.f, float targetY = -1.f) override;

    // ─── Flag API ─────────────────────────────────────────────────────────────
    void triggerFlagSlide(int poleGridX) override;
    bool isFlagSliding() const override { return m_flagAnim.active; }
    bool hasFlagSlideFinished() const override { return m_flagAnim.finished; }

    // ─── Map Editor API (IMapManager) ─────────────────────────────────────────
    void editTile(int gx, int gy, TileType newType) override;
    bool undoEdit() override;
    bool redoEdit() override;
    bool saveToTMX(const std::string& path) const override;
    void addEnemySpawn(const EntitySpawnData& spawn) override;
    void removeEnemySpawn(int index) override;
};