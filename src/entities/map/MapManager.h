#pragma once
#include "../../interfaces/IMapManager.h"
#include "../../interfaces/IItemManager.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <utility>
#include <SFML/Graphics.hpp>
#include "../../tinyxml2.h"

enum class TileType {
    EMPTY = 0,
    GROUND = 1,
    PIPE = 2,
    BRICK_NORMAL = 3,
    QUESTION_COIN = 4,
    QUESTION_POWERUP = 5,
    MULTI_COIN = 6,
    HIDDEN_BLOCK = 7,
    DEATH_ZONE = 8,
    FLAGPOLE = 9,
    COIN = 10,
    SOLID_BRICK = 11  
};

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

// ─── State Structs ────────────────────────────────────────────────────────────

/**
 * @struct MultiCoinState
 * @brief Tracks the 3.5 s countdown for a MULTI_COIN block.
 */
struct MultiCoinState {
    float timer  = 0.f;   // counts down from MULTI_COIN_DURATION → 0
    bool  active = false; // true while countdown is running
};

class MapManager : public IMapManager {
private:
    std::vector<std::vector<TileType>> m_mapData;
    int m_tileSize = 16; 
    
    // ─── Injected dependency ─────────────────────────────────────────────────
    IItemManager* m_itemManager = nullptr;

    // ─── Revealed hidden blocks ──────────────────────────────────────────────
    // A HIDDEN_BLOCK becomes solid (like BRICK_NORMAL) once it has been hit
    // from below.  Before being hit it is invisible and non-solid.
    std::set<std::pair<int,int>> m_revealedHiddenBlocks;

    // ─── MULTI_COIN per-tile state ────────────────────────────────────────────
    std::map<std::pair<int,int>, MultiCoinState> m_multiCoinStates;
    static constexpr float MULTI_COIN_DURATION = 3.5f;

    // ─── Live animations ──────────────────────────────────────────────────────
    std::vector<BrickDebris> m_brickDebris;
    std::vector<CoinPopAnim> m_coinPopAnims;

    static constexpr float DEBRIS_LIFE     = 0.7f;  // seconds
    static constexpr float COINPOP_LIFE    = 0.55f; // seconds
    static constexpr float COINPOP_INIT_VY = -200.f;// px/s upward
    static constexpr float DEBRIS_GRAVITY  = 600.f; // px/s²
    static constexpr float COINPOP_GRAVITY = 400.f;

    // ─── CSV loader (kept as fallback) ───────────────────────────────────
    bool loadMapCSV(const std::string& filepath);

    // ─── TMX / TSX loaders ─────────────────────────────────────────────
    bool loadMapTMX(const std::string& tmxPath);
    // Parses the external .tsx tileset file and populates m_gidTypeMap
    bool loadTileset(const std::string& tsxPath, int firstGid);
    // Converts a GID integer to the corresponding TileType
    TileType gidToTileType(int gid) const;
    // Converts the Tiled propertytype / value string to TileType
    static TileType stringToTileType(const std::string& s);
    // Resolves a path relative to a base directory
    static std::string resolvePath(const std::string& baseDir, const std::string& relativePath);

    // GID → TileType lookup table (populated by loadTileset)
    std::unordered_map<int, TileType> m_gidTypeMap;
    int m_tilesetFirstGid  = 1;  // firstgid attribute from <tileset> element
    int m_tilesetColumns   = 0;  // columns of the tileset sprite sheet

    // ─── Tile-hit handlers ────────────────────────────────────────────────────
    void handleBrickNormal    (int gx, int gy);
    void handleQuestionCoin   (int gx, int gy);
    void handleQuestionPowerup(int gx, int gy, int formType);
    void handleMultiCoin      (int gx, int gy);
    void handleHiddenBlock    (int gx, int gy);

    // ─── Animation spawners ───────────────────────────────────────────────────
    void spawnBrickDebris(int gx, int gy);
    void spawnCoinPopAt  (int gx, int gy);

public:
    MapManager();
    ~MapManager() override = default;

    // Các hàm được override từ IMapManager
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    
    // Hàm phục vụ xử lý va chạm cho Player/Enemy
    bool isSolid(float x, float y) const override;
    int getTileSize() const override { return m_tileSize; }
    sf::Vector2u getMapPixelSize() const override {
        if (m_mapData.empty() || m_mapData[0].empty()) return sf::Vector2u(0, 0);
        return sf::Vector2u(
            static_cast<unsigned>(m_mapData[0].size()) * m_tileSize,
            static_cast<unsigned>(m_mapData.size()) * m_tileSize
        );
    }
    
    // Hàm mở rộng: Lấy chính xác loại Tile để xử lý đụng đầu
    TileType getTileType(float x, float y) const;
    void setTileType(float x, float y, TileType newType); // Dùng để biến block thành EMPTY khi vỡ

    // ─── IMapManager new API ─────────────────────────────────────────────────
    void onHitFromBelow(int tileGridX, int tileGridY, int formType) override;
    void setItemManager(IItemManager* itemManager) override { m_itemManager = itemManager; }
};