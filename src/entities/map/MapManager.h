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
#include <optional>
#include <SFML/Graphics.hpp>
#include "../../tinyxml2.h"
#include "block/IBlockBehavior.h"
#include "MapData.h"

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
    // Raw GID grid — mirrors m_mapData but stores the original Tiled GID
    // (0 = no tile).  Used to compute UV rects into the tileset sprite-sheet.
    std::vector<std::vector<int>> m_rawGids;
    int m_tileSize = 16; 
    
    // ─── Injected dependency ─────────────────────────────────────────────────
    IItemManager* m_itemManager = nullptr;

    // ─── Parsed object layer data ─────────────────────────────────────────────
    MapObjectData m_objectData;

    // ─── MULTI_COIN per-tile state ────────────────────────────────────────────
    std::map<std::pair<int,int>, MultiCoinState> m_multiCoinStates;
    static constexpr float MULTI_COIN_DURATION = 3.5f;

    // ─── Live animations ──────────────────────────────────────────────────────
    std::vector<BrickDebris> m_brickDebris;
    std::vector<CoinPopAnim> m_coinPopAnims;
    FlagSlideAnim            m_flagAnim;

    // ─── Tileset sprite-sheet rendering ──────────────────────────────────────
    sf::Texture                  m_tilesetTexture;  // PNG loaded from the TSX <image> node
    mutable std::optional<sf::Sprite> m_tileSprite; // absent until texture is ready (SFML 3 has no default ctor)
    bool m_textureLoaded = false;                   // true once the texture loaded successfully

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

    // Parses <objectgroup> layers from TMX for enemy/player spawn points
    void parseObjectGroups(tinyxml2::XMLElement* mapElement);

    // GID → TileType lookup table (populated by loadTileset)
    std::unordered_map<int, TileType> m_gidTypeMap;
    // TileType → GID lookup table (reverse of m_gidTypeMap, built in loadTileset).
    // EMPTY cũng có GID riêng (tile EMPTY trong tileset) để setTile(EMPTY) load
    // đúng texture EMPTY; GID 0 chỉ dành cho ô "không có tile".
    std::unordered_map<TileType, int> m_typeToGid;
    int m_tilesetFirstGid  = 1;  // firstgid attribute from <tileset> element
    int m_tilesetColumns   = 0;  // columns of the tileset sprite sheet

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

    // ─── Block transition API ─────────────────────────────────────────────────
    // Điểm duy nhất thay đổi tile: cập nhật ĐỒNG THỜI m_mapData (logic)
    // và m_rawGids (texture) qua m_typeToGid — Encapsulation.
    void setTile(int gx, int gy, TileType type);

    // ─── Side-effect API (được các IBlockBehavior gọi) ───────────────────────
    void spawnBrickDebris(int gx, int gy);
    void spawnCoinPop(int gx, int gy);                 // pop animation + award coin
    void spawnItemForFormType(int gx, int gy, int formType); // Mushroom / FireFlower
    void setMultiCoinActive(int gx, int gy);           // bắt đầu/giữ countdown 3.5s

    // ─── IMapManager new API ─────────────────────────────────────────────────
    void onHitFromBelow(int tileGridX, int tileGridY, int formType) override;
    void setItemManager(IItemManager* itemManager) override { m_itemManager = itemManager; }

    void triggerFlagSlide(int poleGridX) override;
    bool isFlagSliding() const override { return m_flagAnim.active; }
    bool hasFlagSlideFinished() const override { return m_flagAnim.finished; }
};