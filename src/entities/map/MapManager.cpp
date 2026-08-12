#include "MapManager.h"
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace fs = std::filesystem;

MapManager::MapManager() {}

void MapManager::initialize() {
    // Prefer the Tiled TMX map; fall back to the legacy CSV if not found
    if (!loadMapTMX("assets/map/map_test.tmx")) {
        std::cerr << "[MapManager] TMX load failed, falling back to CSV." << std::endl;
        if (!loadMapCSV("assets/map/test_map.csv")) {
            std::cerr << "[MapManager] ERROR: Could not load any map!" << std::endl;
        }
    }
}

bool MapManager::loadMapCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    m_mapData.clear();

    while (std::getline(file, line)) {
        std::vector<TileType> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            if (cell.empty()) continue;
            int tileID = std::stoi(cell);
            row.push_back(static_cast<TileType>(tileID));
        }
        if (!row.empty())
            m_mapData.push_back(row);
    }
    
    std::cout << "[MapManager] CSV loaded: "
              << m_mapData[0].size() << "x" << m_mapData.size() << std::endl;
    return true;
}

// =============================================================================
//  TMX / TSX LOADER
// =============================================================================

// ── Helpers ───────────────────────────────────────────────────────────────────────────────

TileType MapManager::stringToTileType(const std::string& s) {
    if (s == "GROUND")           return TileType::GROUND;
    if (s == "PIPE")             return TileType::PIPE;
    if (s == "BRICK_NORMAL")     return TileType::BRICK_NORMAL;
    if (s == "QUESTION_COIN")    return TileType::QUESTION_COIN;
    if (s == "QUESTION_POWERUP") return TileType::QUESTION_POWERUP;
    if (s == "MULTI_COIN")       return TileType::MULTI_COIN;
    if (s == "HIDDEN_BLOCK")     return TileType::HIDDEN_BLOCK;
    if (s == "DEATH_ZONE")       return TileType::DEATH_ZONE;
    if (s == "FLAGPOLE")         return TileType::FLAGPOLE;
    if (s == "SOLID_BRICK")      return TileType::SOLID_BRICK;
    return TileType::EMPTY; // unknown / decorative
}

std::string MapManager::resolvePath(const std::string& baseDir, const std::string& relativePath) {
    // Use std::filesystem to cleanly resolve relative paths
    fs::path resolved = fs::path(baseDir) / fs::path(relativePath);
    return resolved.lexically_normal().string();
}

TileType MapManager::gidToTileType(int gid) const {
    if (gid == 0) return TileType::EMPTY; // GID 0 means no tile
    auto it = m_gidTypeMap.find(gid);
    if (it != m_gidTypeMap.end()) return it->second;
    return TileType::EMPTY; // unmapped GIDs are decorative
}

// ── TSX tileset parser ─────────────────────────────────────────────────────────────────────
//
// Tiled property format used in this project:
//   <tile id="N">
//     <properties>
//       <property name="TileTypeName" type="int" propertytype="TileTypeName" value="0"/>
//     </properties>
//   </tile>
//
// The TileType is encoded in the `propertytype` attribute (which is the
// Tiled enum class name).  We also accept `name` as a secondary fallback
// in case the tile was annotated using the simple string approach.

bool MapManager::loadTileset(const std::string& tsxPath, int firstGid) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(tsxPath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "[MapManager] Cannot open TSX: " << tsxPath << std::endl;
        return false;
    }

    auto* root = doc.FirstChildElement("tileset");
    if (!root) { std::cerr << "[MapManager] No <tileset> root in " << tsxPath << std::endl; return false; }

    m_tilesetFirstGid = firstGid;
    root->QueryIntAttribute("columns", &m_tilesetColumns);

    // ── Load the sprite-sheet image declared in <image source="..."/> ─────────
    auto* imgEl = root->FirstChildElement("image");
    if (imgEl) {
        const char* imgSrc = imgEl->Attribute("source");
        if (imgSrc) {
            // The image path is relative to the .tsx file's directory
            std::string tsxDir = fs::path(tsxPath).parent_path().string();
            std::string imgPath = resolvePath(tsxDir, imgSrc);
            if (m_tilesetTexture.loadFromFile(imgPath)) {
                m_tilesetTexture.setSmooth(false); // pixel-art: no blurring
                // SFML 3: Sprite must be constructed with a texture (no default ctor)
                m_tileSprite.emplace(m_tilesetTexture);
                m_textureLoaded = true;
                std::cout << "[MapManager] Tileset texture loaded: " << imgPath << std::endl;
            } else {
                std::cerr << "[MapManager] Failed to load tileset image: " << imgPath << std::endl;
            }
        }
    }

    // Iterate over every <tile> element
    for (auto* tileEl = root->FirstChildElement("tile"); tileEl;
         tileEl = tileEl->NextSiblingElement("tile")) {

        int localId = 0;
        tileEl->QueryIntAttribute("id", &localId);
        int gid = firstGid + localId;

        TileType resolved = TileType::EMPTY;
        bool found = false;

        auto* propsEl = tileEl->FirstChildElement("properties");
        if (propsEl) {
            for (auto* prop = propsEl->FirstChildElement("property"); prop;
                 prop = prop->NextSiblingElement("property")) {

                // PRIMARY: use the `propertytype` attribute (Tiled enum class name)
                const char* proptype = prop->Attribute("propertytype");
                if (proptype && proptype[0] != '\0') {
                    TileType candidate = stringToTileType(proptype);
                    if (candidate != TileType::EMPTY) {
                        resolved = candidate;
                        found = true;
                        break;
                    }
                }

                // FALLBACK A: property name IS the type string
                //   e.g. <property name="GROUND" .../>
                const char* propname = prop->Attribute("name");
                if (!found && propname) {
                    TileType candidate = stringToTileType(propname);
                    if (candidate != TileType::EMPTY) {
                        resolved = candidate;
                        found = true;
                        break;
                    }
                }

                // FALLBACK B: property named "type" with string value
                //   e.g. <property name="type" value="GROUND"/>
                if (!found && propname && std::string(propname) == "type") {
                    const char* val = prop->Attribute("value");
                    if (val) {
                        resolved = stringToTileType(val);
                        found = true;
                        break;
                    }
                }
            }
        }

        // Only store non-EMPTY types to keep the map small
        if (found && resolved != TileType::EMPTY) {
            m_gidTypeMap[gid] = resolved;
        }
    }

    std::cout << "[MapManager] TSX loaded: " << tsxPath
              << "  (" << m_gidTypeMap.size() << " typed tiles)" << std::endl;
    return true;
}

// ── TMX map parser ─────────────────────────────────────────────────────────────────────────
bool MapManager::loadMapTMX(const std::string& tmxPath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(tmxPath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "[MapManager] Cannot open TMX: " << tmxPath << std::endl;
        return false;
    }

    auto* mapEl = doc.FirstChildElement("map");
    if (!mapEl) { std::cerr << "[MapManager] No <map> root in " << tmxPath << std::endl; return false; }

    int mapWidth = 0, mapHeight = 0, tileW = 16, tileH = 16;
    mapEl->QueryIntAttribute("width",      &mapWidth);
    mapEl->QueryIntAttribute("height",     &mapHeight);
    mapEl->QueryIntAttribute("tilewidth",  &tileW);
    mapEl->QueryIntAttribute("tileheight", &tileH);
    m_tileSize = tileW; // use the map's declared tile size

    // Base directory of the .tmx file (used to resolve relative .tsx paths)
    std::string baseDir = fs::path(tmxPath).parent_path().string();

    // ── Load external tileset(s) ────────────────────────────────────────
    for (auto* tsEl = mapEl->FirstChildElement("tileset"); tsEl;
         tsEl = tsEl->NextSiblingElement("tileset")) {

        int firstGid = 1;
        tsEl->QueryIntAttribute("firstgid", &firstGid);

        const char* src = tsEl->Attribute("source");
        if (src) {
            // External .tsx
            std::string tsxPath = resolvePath(baseDir, src);
            loadTileset(tsxPath, firstGid);
        } else {
            // Inline tileset – parse <tile> children directly
            // (reuse the same logic by forwarding to a temporary doc would be
            // complex; for now we call loadTileset with the mapEl as the root)
            // This project uses external .tsx so this branch is for safety.
            std::cerr << "[MapManager] Inline tilesets not supported yet." << std::endl;
        }
    }

    // ── Parse layer data ──────────────────────────────────────────────────
    // Only the FIRST tile layer is used for collision / game logic.
    // Additional layers (background, foreground decoration) are ignored.
    auto* layerEl = mapEl->FirstChildElement("layer");
    if (!layerEl) { std::cerr << "[MapManager] No <layer> in TMX." << std::endl; return false; }

    auto* dataEl = layerEl->FirstChildElement("data");
    if (!dataEl) { std::cerr << "[MapManager] No <data> in layer." << std::endl; return false; }

    const char* encoding = dataEl->Attribute("encoding");
    if (!encoding || std::string(encoding) != "csv") {
        std::cerr << "[MapManager] Only CSV-encoded TMX layers are supported." << std::endl;
        return false;
    }

    const char* rawText = dataEl->GetText();
    if (!rawText) { std::cerr << "[MapManager] Layer data is empty." << std::endl; return false; }

    // Parse the CSV GID list into m_mapData AND m_rawGids
    m_mapData.clear();
    m_mapData.reserve(mapHeight);
    m_rawGids.clear();
    m_rawGids.reserve(mapHeight);

    std::vector<TileType> currentRow;
    std::vector<int>      currentGids;
    currentRow.reserve(mapWidth);
    currentGids.reserve(mapWidth);

    std::istringstream stream(rawText);
    std::string token;
    int col = 0;

    while (std::getline(stream, token, ',')) {
        // Strip whitespace / newlines
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        if (token.empty()) continue;

        int gid = std::stoi(token);
        currentRow.push_back(gidToTileType(gid));
        currentGids.push_back(gid);
        ++col;

        if (col >= mapWidth) {
            m_mapData.push_back(std::move(currentRow));
            m_rawGids.push_back(std::move(currentGids));
            currentRow.clear();
            currentGids.clear();
            currentRow.reserve(mapWidth);
            currentGids.reserve(mapWidth);
            col = 0;
        }
    }
    // Push any partial last row
    if (!currentRow.empty()) {
        m_mapData.push_back(std::move(currentRow));
        m_rawGids.push_back(std::move(currentGids));
    }

    std::cout << "[MapManager] TMX loaded: " << tmxPath
              << "  (" << (m_mapData.empty() ? 0 : m_mapData[0].size())
              << "x" << m_mapData.size() << ")" << std::endl;
    return true;
}

// =============================================================================
//  UPDATE
// =============================================================================

void MapManager::update(float deltaTime) {
    // ── MULTI_COIN countdowns ─────────────────────────────────────────────────
    for (auto it = m_multiCoinStates.begin(); it != m_multiCoinStates.end(); ) {
        MultiCoinState& state = it->second;
        if (state.active) {
            state.timer -= deltaTime;
            if (state.timer <= 0.f) {
                // Countdown expired → convert tile to exhausted solid brick
                int gx = it->first.first;
                int gy = it->first.second;
                if (gy >= 0 && gy < (int)m_mapData.size() &&
                    gx >= 0 && gx < (int)m_mapData[0].size()) {
                    m_mapData[gy][gx] = TileType::SOLID_BRICK;
                }
                it = m_multiCoinStates.erase(it);
                continue;
            }
        }
        ++it;
    }

    // ── Brick debris particles ────────────────────────────────────────────────
    for (auto& d : m_brickDebris) {
        d.vel.y += DEBRIS_GRAVITY * deltaTime;
        d.pos   += d.vel * deltaTime;
        d.life  -= deltaTime;
    }
    m_brickDebris.erase(
        std::remove_if(m_brickDebris.begin(), m_brickDebris.end(),
            [](const BrickDebris& d){ return d.life <= 0.f; }),
        m_brickDebris.end());

    // ── Coin pop animations ───────────────────────────────────────────────────
    for (auto& c : m_coinPopAnims) {
        c.velY += COINPOP_GRAVITY * deltaTime;
        c.pos.y += c.velY * deltaTime;
        c.life  -= deltaTime;
    }
    m_coinPopAnims.erase(
        std::remove_if(m_coinPopAnims.begin(), m_coinPopAnims.end(),
            [](const CoinPopAnim& c){ return c.life <= 0.f; }),
        m_coinPopAnims.end());
}

// =============================================================================
//  RENDER
// =============================================================================

void MapManager::render(sf::RenderWindow& window) const {
    // ── Tile rendering ────────────────────────────────────────────────────────
    // When the tileset texture is available we render each tile as a sprite
    // cropped from the sprite-sheet.  Otherwise we fall back to a solid-colour
    // rectangle so the game is still playable without the asset.

    // Fallback shape (used only when m_textureLoaded == false)
    sf::RectangleShape tileShape(sf::Vector2f((float)m_tileSize, (float)m_tileSize));

    const bool hasRawGids = !m_rawGids.empty();

    for (size_t y = 0; y < m_mapData.size(); ++y) {
        for (size_t x = 0; x < m_mapData[y].size(); ++x) {
            TileType type = m_mapData[y][x];

            // HIDDEN_BLOCK: invisible & non-solid until hit from below.
            // After reveal it behaves like SOLID_BRICK visually, but we keep
            // the type as HIDDEN_BLOCK in m_mapData so the GID in m_rawGids
            // still points to its correct sprite-sheet region.
            if (type == TileType::HIDDEN_BLOCK) {
                auto key = std::make_pair((int)x, (int)y);
                if (m_revealedHiddenBlocks.find(key) == m_revealedHiddenBlocks.end())
                    continue; // not yet revealed → invisible
                // revealed → fall through and render like any other tile
            }

            const float worldX = (float)x * m_tileSize;
            const float worldY = (float)y * m_tileSize;

            if (m_textureLoaded && hasRawGids &&
                y < m_rawGids.size() && x < m_rawGids[y].size()) {

                int gid = m_rawGids[y][x];
                // GID == 0 means "no tile placed here" in Tiled → truly skip
                // GID > 0 → always draw from sprite-sheet, including EMPTY-typed tiles
                //           that still have a visual in the tileset
                if (gid > 0 && m_tilesetColumns > 0) {
                    // Convert GID → local tile index → (col, row) on the sprite-sheet
                    int localId  = gid - m_tilesetFirstGid; // 0-based index
                    int tileCol  = localId % m_tilesetColumns;
                    int tileRow  = localId / m_tilesetColumns;

                    // SFML 3: IntRect takes two Vector2i (position, size)
                    m_tileSprite->setTextureRect(sf::IntRect(
                        {tileCol * m_tileSize, tileRow * m_tileSize},
                        {m_tileSize, m_tileSize}));
                    // SFML 3: setPosition takes a single Vector2f
                    m_tileSprite->setPosition({worldX, worldY});
                    window.draw(*m_tileSprite);
                    continue; // sprite drawn, skip fallback
                }
            }

            // ── Fallback: solid colour (texture not available) ────────────────
            // EMPTY tiles have no meaningful colour fallback → skip
            if (type == TileType::EMPTY) continue;
            tileShape.setPosition(sf::Vector2f(worldX, worldY));
            switch (type) {
                case TileType::GROUND:           tileShape.setFillColor(sf::Color(139,  69,  19)); break;
                case TileType::PIPE:             tileShape.setFillColor(sf::Color::Green);          break;
                case TileType::BRICK_NORMAL:
                case TileType::HIDDEN_BLOCK:     tileShape.setFillColor(sf::Color(205, 133,  63)); break;
                case TileType::SOLID_BRICK:      tileShape.setFillColor(sf::Color(160, 110,  80)); break;
                case TileType::MULTI_COIN:       tileShape.setFillColor(sf::Color(205, 133,  63)); break;
                case TileType::QUESTION_COIN:
                case TileType::QUESTION_POWERUP: tileShape.setFillColor(sf::Color::Yellow);         break;
                case TileType::COIN:             tileShape.setFillColor(sf::Color::White);           break;
                case TileType::DEATH_ZONE:       tileShape.setFillColor(sf::Color::Red);             break;
                case TileType::FLAGPOLE:         tileShape.setFillColor(sf::Color::Cyan);            break;
                default:                         tileShape.setFillColor(sf::Color::Magenta);         break;
            }
            window.draw(tileShape);
        }
    }

    // ── Brick debris ──────────────────────────────────────────────────────────
    for (const auto& d : m_brickDebris) {
        float alpha = std::max(0.f, d.life / d.maxLife);
        sf::Color c = d.color;
        c.a = static_cast<uint8_t>(alpha * 255.f);
        sf::RectangleShape piece(d.size);
        piece.setPosition(d.pos);
        piece.setFillColor(c);
        window.draw(piece);
    }

    // ── Coin pop animations ───────────────────────────────────────────────────
    for (const auto& coin : m_coinPopAnims) {
        float alpha = std::max(0.f, coin.life / COINPOP_LIFE);
        uint8_t a = static_cast<uint8_t>(alpha * 255.f);
        sf::RectangleShape coinShape(sf::Vector2f(8.f, 8.f));
        coinShape.setPosition(coin.pos);
        coinShape.setFillColor(sf::Color(255, 215, 0, a)); // gold
        window.draw(coinShape);
    }
}

// =============================================================================
//  SOLID QUERY
// =============================================================================

TileType MapManager::getTileType(float x, float y) const {
    int gridX = static_cast<int>(x) / m_tileSize;
    int gridY = static_cast<int>(y) / m_tileSize;

    if (gridY < 0 || gridY >= (int)m_mapData.size() || 
        gridX < 0 || gridX >= (int)m_mapData[0].size()) {
        return TileType::GROUND;
    }

    return m_mapData[gridY][gridX];
}

bool MapManager::isSolid(float x, float y) const {
    int gridX = static_cast<int>(x) / m_tileSize;
    int gridY = static_cast<int>(y) / m_tileSize;

    if (gridY < 0 || gridY >= (int)m_mapData.size() ||
        gridX < 0 || gridX >= (int)m_mapData[0].size()) {
        return true; // treat out-of-bounds as wall
    }

    TileType type = m_mapData[gridY][gridX];

    switch (type) {
        case TileType::GROUND:
        case TileType::PIPE:
        case TileType::BRICK_NORMAL:
        case TileType::SOLID_BRICK:
        case TileType::QUESTION_COIN:
        case TileType::QUESTION_POWERUP:
        case TileType::MULTI_COIN:
            return true;

        case TileType::HIDDEN_BLOCK: {
            // Solid only after being revealed from below
            auto key = std::make_pair(gridX, gridY);
            return m_revealedHiddenBlocks.count(key) > 0;
        }

        default:
            return false;
    }
}

void MapManager::setTileType(float x, float y, TileType newType) {
    int gridX = static_cast<int>(x) / m_tileSize;
    int gridY = static_cast<int>(y) / m_tileSize;
    
    if (gridY >= 0 && gridY < (int)m_mapData.size() && 
        gridX >= 0 && gridX < (int)m_mapData[0].size()) {
        m_mapData[gridY][gridX] = newType;
    }
}

// =============================================================================
//  ON HIT FROM BELOW  (main dispatcher)
// =============================================================================

void MapManager::onHitFromBelow(int gx, int gy, int formType) {
    if (gy < 0 || gy >= (int)m_mapData.size() ||
        gx < 0 || gx >= (int)m_mapData[0].size()) return;

    TileType type = m_mapData[gy][gx];

    switch (type) {
        case TileType::BRICK_NORMAL:
            handleBrickNormal(gx, gy);
            break;
        case TileType::QUESTION_COIN:
            handleQuestionCoin(gx, gy);
            break;
        case TileType::QUESTION_POWERUP:
            handleQuestionPowerup(gx, gy, formType);
            break;
        case TileType::MULTI_COIN:
            handleMultiCoin(gx, gy);
            break;
        case TileType::HIDDEN_BLOCK:
            handleHiddenBlock(gx, gy);
            break;
        default:
            break; // GROUND, PIPE, DEATH_ZONE, EMPTY — no reaction
    }
}

// =============================================================================
//  TILE HANDLERS
// =============================================================================

void MapManager::handleBrickNormal(int gx, int gy) {
    // 1. Spawn 4 debris particles
    spawnBrickDebris(gx, gy);
    // 2. Destroy the tile
    m_mapData[gy][gx] = TileType::EMPTY;
}

void MapManager::handleQuestionCoin(int gx, int gy) {
    // 1. Pop coin animation + award coin
    spawnCoinPopAt(gx, gy);
    if (m_itemManager)
        m_itemManager->spawnCoinPop(
            (float)gx * m_tileSize,
            (float)gy * m_tileSize);
    // 2. Block becomes exhausted solid brick
    m_mapData[gy][gx] = TileType::SOLID_BRICK;
}

void MapManager::handleQuestionPowerup(int gx, int gy, int formType) {
    // Block becomes exhausted solid brick first
    m_mapData[gy][gx] = TileType::SOLID_BRICK;

    float worldX = (float)gx * m_tileSize;
    float worldY = (float)(gy - 1) * m_tileSize; // spawn one tile above

    if (m_itemManager) {
        if (formType == 0) {
            // NormalForm → Mushroom
            m_itemManager->spawnMushroom(worldX, worldY);
        } else {
            // SuperForm or FireForm → FireFlower
            m_itemManager->spawnFireFlower(worldX, worldY);
        }
    }
}

void MapManager::handleMultiCoin(int gx, int gy) {
    auto key = std::make_pair(gx, gy);
    MultiCoinState& state = m_multiCoinStates[key];

    if (!state.active) {
        // First hit: start the countdown
        state.active = true;
        state.timer  = MULTI_COIN_DURATION;
    }
    // Whether just started or still running, give a coin
    // (if timer already expired it will have been converted in update())
    spawnCoinPopAt(gx, gy);
    if (m_itemManager)
        m_itemManager->spawnCoinPop(
            (float)gx * m_tileSize,
            (float)gy * m_tileSize);
}

void MapManager::handleHiddenBlock(int gx, int gy) {
    // Mark as revealed — render will now show it and isSolid() will return true.
    // We deliberately keep m_mapData as HIDDEN_BLOCK so that m_rawGids[gy][gx]
    // still contains the original GID and the correct tileset sprite is drawn.
    auto key = std::make_pair(gx, gy);
    m_revealedHiddenBlocks.insert(key);
    // Do NOT overwrite m_mapData here — isSolid() already handles revealed
    // HIDDEN_BLOCKs via the m_revealedHiddenBlocks set.
}

// =============================================================================
//  ANIMATION SPAWNERS
// =============================================================================

void MapManager::spawnBrickDebris(int gx, int gy) {
    // Brick colour – same orange-brown as the tile
    const sf::Color brickColor(205, 133, 63);

    float tileX = (float)gx * m_tileSize;
    float tileY = (float)gy * m_tileSize;
    float half  = (float)m_tileSize / 2.f;

    // 4 quarter-pieces: top-left, top-right, bottom-left, bottom-right
    // Each piece starts at the corner of its quarter and flies outward + upward
    struct PieceDesc { float ox, oy, vx, vy; };
    const PieceDesc pieces[4] = {
        { 0.f,  0.f,  -90.f, -280.f },   // top-left    → flies left + up
        { half, 0.f,   90.f, -280.f },   // top-right   → flies right + up
        { 0.f,  half, -60.f, -180.f },   // bottom-left → flies left + less up
        { half, half,  60.f, -180.f },   // bottom-right→ flies right + less up
    };

    for (auto& p : pieces) {
        BrickDebris d;
        d.pos     = { tileX + p.ox, tileY + p.oy };
        d.vel     = { p.vx, p.vy };
        d.color   = brickColor;
        d.life    = DEBRIS_LIFE;
        d.maxLife = DEBRIS_LIFE;
        d.size    = { half, half };
        m_brickDebris.push_back(d);
    }
}

void MapManager::spawnCoinPopAt(int gx, int gy) {
    CoinPopAnim c;
    // Centre the coin horizontally over the tile, start at its top
    c.pos  = {
        (float)gx * m_tileSize + m_tileSize / 2.f - 4.f,
        (float)gy * m_tileSize - (float)m_tileSize
    };
    c.velY = COINPOP_INIT_VY;
    c.life = COINPOP_LIFE;
    m_coinPopAnims.push_back(c);
}