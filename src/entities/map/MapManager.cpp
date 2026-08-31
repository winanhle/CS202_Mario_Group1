#include "MapManager.h"
#include "block/BlockBehavior.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

// ── Flagpole & Flag Slide Constants ──────────────────────────────────────────
static constexpr int   GID_FLAGPOLE_TOP_BALL = 621;
static constexpr int   GID_FLAGPOLE_FLAG     = 831;
static constexpr int   GID_FLAGPOLE_ATTACH   = 832;
static constexpr int   GID_FLAGPOLE_SHAFT    = 1043;
static constexpr int   GID_FLAGPOLE_BASE     = 2667;
static constexpr float FLAG_SLIDE_SPEED      = 90.f;

MapManager::MapManager() {}

void MapManager::initialize() {
    m_mapData.clear();
    m_rawGids.clear();
    m_bgGids.clear();
    m_objectData = MapObjectData{};
    m_multiCoinStates.clear();
    m_brickDebris.clear();
    m_coinPopAnims.clear();
    m_blockBumpAnims.clear();
    m_tilesets.clear();
    m_undoStack.clear();
    m_redoStack.clear();
    // Bản đồ được tải bởi GameWorld thông qua LevelManager (loadCurrentLevel()).
}

// =============================================================================
//  HELPER: stringToTileType — OCP-compliant unordered_map registry
//  Thêm TileType mới = thêm một entry vào bảng, không sửa if-else nào.
// =============================================================================

TileType MapManager::stringToTileType(const std::string& s) {
    static const std::unordered_map<std::string, TileType> table = {
        {"GROUND",          TileType::GROUND},
        {"PIPE",            TileType::PIPE},
        {"BRICK_NORMAL",    TileType::BRICK_NORMAL},
        {"BRICK_EMPTY",     TileType::BRICK_EMPTY},
        {"BRICK_SOLID",     TileType::BRICK_SOLID},
        {"QUESTION_COIN",   TileType::QUESTION_COIN},
        {"QUESTION_POWERUP",TileType::QUESTION_POWERUP},
        {"MULTI_COIN",      TileType::MULTI_COIN},
        {"MULTI_COIN2",     TileType::MULTI_COIN2},
        {"HIDDEN_BLOCK",    TileType::HIDDEN_BLOCK},
        {"DEATH_ZONE",      TileType::DEATH_ZONE},
        {"FLAGPOLE",        TileType::FLAGPOLE},
        {"SOLID_BRICK",     TileType::SOLID_BRICK},
        {"QUESTION_USED",   TileType::QUESTION_USED},
        // FIRE_BAR aliases — normalised to one enum value
        {"FIRE_BAR",        TileType::FIRE_BAR},
        {"FIREBAR",         TileType::FIRE_BAR},
        {"FireBar",         TileType::FIRE_BAR},
        {"Firebar",         TileType::FIRE_BAR},
        {"fire_bar",        TileType::FIRE_BAR},
        {"firebar",         TileType::FIRE_BAR},
        {"BACKGROUND",      TileType::BACKGROUND},
        {"BACK_GROUND",     TileType::BACKGROUND},
        {"COIN",            TileType::COIN},
    };
    auto it = table.find(s);
    return (it != table.end()) ? it->second : TileType::EMPTY;
}

std::string MapManager::resolvePath(const std::string& baseDir, const std::string& relativePath) {
    fs::path resolved = fs::path(baseDir) / fs::path(relativePath);
    return resolved.lexically_normal().string();
}

TileType MapManager::gidToTileType(int gid) const {
    if (gid == 0) return TileType::EMPTY;
    auto it = m_gidTypeMap.find(gid);
    if (it != m_gidTypeMap.end()) return it->second;
    return TileType::EMPTY;
}

// =============================================================================
//  TSX & INLINE TILESET PARSER
// =============================================================================

bool MapManager::parseTilesetElement(tinyxml2::XMLElement* root, const std::string& baseDir, int firstGid) {
    if (!root) return false;

    int columns = 0;
    root->QueryIntAttribute("columns", &columns);
    m_tilesetFirstGid = firstGid;
    m_tilesetColumns  = columns;
    m_tilesets.push_back(LoadedTileset{firstGid, columns});

    // ── Load the sprite-sheet image declared in <image source="..."/> ─────────
    auto* imgEl = root->FirstChildElement("image");
    if (imgEl) {
        const char* imgSrc = imgEl->Attribute("source");
        if (imgSrc) {
            std::string imgPath = resolvePath(baseDir, imgSrc);
            if (!m_textureLoaded) {
                if (m_tilesetTexture.loadFromFile(imgPath)) {
                    m_tilesetTexture.setSmooth(false);
                    m_tileSprite.emplace(m_tilesetTexture);
                    m_textureLoaded = true;
                    std::cout << "[MapManager] Tileset texture loaded: " << imgPath << std::endl;
                } else {
                    std::cerr << "[MapManager] Failed to load tileset image: " << imgPath << std::endl;
                }
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
                    resolved = stringToTileType(proptype);
                    found = true;
                    break;
                }

                // FALLBACK A: property named "type" with string value
                const char* propname = prop->Attribute("name");
                if (!found && propname && std::string(propname) == "type") {
                    const char* val = prop->Attribute("value");
                    if (val) {
                        resolved = stringToTileType(val);
                        found = true;
                        break;
                    }
                }

                // FALLBACK B: property name IS the type string
                if (!found && propname) {
                    TileType candidate = stringToTileType(propname);
                    if (candidate != TileType::EMPTY) {
                        resolved = candidate;
                        found = true;
                        break;
                    }
                }
            }
        }

        if (found && resolved != TileType::EMPTY) {
            m_gidTypeMap[gid] = resolved;
        }

        // Reverse lookup: TileType → GID (first occurrence wins).
        if (found) {
            if (m_typeToGid.find(resolved) == m_typeToGid.end()) {
                m_typeToGid[resolved] = gid;
            }
        }
    }

    // Fallback: ensure TileType::EMPTY maps to firstGid (usually 1, the top-left sky tile)
    if (m_typeToGid.find(TileType::EMPTY) == m_typeToGid.end()) {
        m_typeToGid[TileType::EMPTY] = firstGid;
    }

    std::cout << "[MapManager] Tileset parsed (firstgid=" << firstGid
              << ", cols=" << columns << ", " << m_gidTypeMap.size()
              << " typed tiles total)" << std::endl;
    return true;
}

bool MapManager::loadTileset(const std::string& tsxPath, int firstGid) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(tsxPath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "[MapManager] Cannot open TSX: " << tsxPath << std::endl;
        return false;
    }

    auto* root = doc.FirstChildElement("tileset");
    if (!root) { std::cerr << "[MapManager] No <tileset> root in " << tsxPath << std::endl; return false; }

    std::string tsxDir = fs::path(tsxPath).parent_path().string();
    return parseTilesetElement(root, tsxDir, firstGid);
}

// =============================================================================
//  TMX MAP PARSER
// =============================================================================

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
    m_tileSize = tileW;

    std::string baseDir = fs::path(tmxPath).parent_path().string();

    // ── Load tileset(s) (supports both external .tsx and inline <tileset>) ─
    for (auto* tsEl = mapEl->FirstChildElement("tileset"); tsEl;
         tsEl = tsEl->NextSiblingElement("tileset")) {

        int firstGid = 1;
        tsEl->QueryIntAttribute("firstgid", &firstGid);

        const char* src = tsEl->Attribute("source");
        if (src) {
            m_tsxRelativePath = src; // store for saveToTMX
            std::string tsxPath = resolvePath(baseDir, src);
            loadTileset(tsxPath, firstGid);
        } else {
            // Inline tileset declared directly in the .tmx
            parseTilesetElement(tsEl, baseDir, firstGid);
        }
    }

    // ── Parse layer data ──────────────────────────────────────────────────
    // Merge ALL tile layers. GID 0 = transparent → keep lower layer value.
    // While merging, capture the background GID (m_bgGids) for each cell:
    // the GID that existed before a higher-layer block overwrote it.
    // When a brick is later broken (setTile EMPTY), m_bgGids is restored.
    m_mapData.clear();
    m_rawGids.clear();
    m_bgGids.clear();
    m_flagAnim = {};
    m_mapData.assign(mapHeight, std::vector<TileType>(mapWidth, TileType::EMPTY));
    m_rawGids.assign(mapHeight, std::vector<int>(mapWidth, 0));
    m_bgGids.assign(mapHeight, std::vector<int>(mapWidth, 0));

    bool anyLayerLoaded = false;
    for (auto* layerEl = mapEl->FirstChildElement("layer"); layerEl;
         layerEl = layerEl->NextSiblingElement("layer")) {

        const char* layerName = layerEl->Attribute("name");
        auto* dataEl = layerEl->FirstChildElement("data");
        if (!dataEl) {
            std::cerr << "[MapManager] No <data> in layer." << std::endl;
            continue;
        }

        const char* encoding = dataEl->Attribute("encoding");
        if (!encoding || std::string(encoding) != "csv") {
            std::cerr << "[MapManager] Only CSV-encoded TMX layers are supported "
                      << "(layer '" << (layerName ? layerName : "?") << "' skipped)." << std::endl;
            continue;
        }

        const char* rawText = dataEl->GetText();
        if (!rawText) { std::cerr << "[MapManager] Layer data is empty." << std::endl; continue; }

        std::istringstream stream(rawText);
        std::string token;
        int row = 0, col = 0;

        while (std::getline(stream, token, ',')) {
            token.erase(0, token.find_first_not_of(" \t\r\n"));
            token.erase(token.find_last_not_of(" \t\r\n") + 1);
            if (token.empty()) continue;

            int gid = std::stoi(token);
            if (gid > 0 && row < mapHeight && col < mapWidth) {
                // Before overwriting: save the current visual as background.
                // This captures the sky/terrain beneath interactive blocks.
                if (m_rawGids[row][col] > 0) {
                    m_bgGids[row][col] = m_rawGids[row][col];
                }
                m_rawGids[row][col] = gid;
                m_mapData[row][col] = gidToTileType(gid);
            }
            ++col;
            if (col >= mapWidth) { col = 0; ++row; }
        }
        anyLayerLoaded = true;
    }

    if (!anyLayerLoaded) {
        std::cerr << "[MapManager] No <layer> in TMX." << std::endl;
        return false;
    }

    // ── Post-process: infer background GID for interactive tiles ─────────────
    // On single-layer maps every cell is on the same layer, so interactive
    // tiles (BRICK_NORMAL, ?-blocks, etc.) never had a lower layer beneath
    // them → m_bgGids stays 0 → breaking a brick shows black.
    //
    // Fix: scan each row for the GID used by EMPTY-typed cells (sky, bg, etc.)
    // and store that into m_bgGids for any interactive tile in the same row.
    // Vertical scan and global fallback handle edge cases.
    {
        // Step 1: per-row fallback GID = first EMPTY cell with a real sprite GID
        std::vector<int> rowBgGid(mapHeight, 0);
        for (int r = 0; r < mapHeight; ++r) {
            for (int c = 0; c < mapWidth; ++c) {
                TileType t = m_mapData[r][c];
                if ((t == TileType::EMPTY || t == TileType::BACKGROUND) && m_rawGids[r][c] > 0) {
                    rowBgGid[r] = m_rawGids[r][c];
                    break;
                }
            }
        }

        // Step 2: global fallback — the registered GID for the EMPTY tile type
        int globalFallback = 0;
        {
            auto it = m_typeToGid.find(TileType::EMPTY);
            if (it != m_typeToGid.end()) globalFallback = it->second;
        }

        // Step 3: for each interactive tile that still has no background, fill it
        for (int r = 0; r < mapHeight; ++r) {
            // Use row-level bg; if row is fully solid, search neighboring rows
            int bgGid = rowBgGid[r];
            if (bgGid == 0) {
                for (int delta = 1; delta < mapHeight && bgGid == 0; ++delta) {
                    if (r - delta >= 0)          bgGid = rowBgGid[r - delta];
                    if (bgGid == 0 && r + delta < mapHeight) bgGid = rowBgGid[r + delta];
                }
            }
            if (bgGid == 0) bgGid = globalFallback; // last resort

            for (int c = 0; c < mapWidth; ++c) {
                TileType t = m_mapData[r][c];
                // Only fill cells that are interactive and have no stored background
                if (m_bgGids[r][c] == 0 &&
                    t != TileType::EMPTY &&
                    t != TileType::BACKGROUND) {
                    m_bgGids[r][c] = bgGid;
                }
            }
        }
    }

    // ── Parse object groups (enemy spawns, player spawn, lifts, firebars) ─
    // Note: FireBar spawns come ONLY from object groups now.
    // The tile-scan loop was removed to prevent duplicate FireBar spawns.
    parseObjectGroups(mapEl);

    std::cout << "[MapManager] TMX loaded: " << tmxPath
              << "  (" << (m_mapData.empty() ? 0 : m_mapData[0].size())
              << "x" << m_mapData.size() << ") with "
              << m_objectData.fireBarSpawns.size() << " FireBar(s)" << std::endl;
    return true;
}

// =============================================================================
//  CSV LOADER (fallback)
// =============================================================================

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
//  PUBLIC LOAD API
// =============================================================================

void MapManager::loadMap(const std::string& tmxPath) {
    m_mapData.clear();
    m_rawGids.clear();
    m_bgGids.clear();
    m_objectData = MapObjectData{};
    m_multiCoinStates.clear();
    m_brickDebris.clear();
    m_coinPopAnims.clear();
    m_blockBumpAnims.clear();
    m_tilesets.clear();
    m_gidTypeMap.clear();
    m_typeToGid.clear();
    m_textureLoaded = false;
    m_undoStack.clear();
    m_redoStack.clear();
    m_tmxPath = tmxPath;

    if (!loadMapTMX(tmxPath)) {
        std::cerr << "[MapManager] TMX load failed for: " << tmxPath << std::endl;
    }
}

const MapObjectData& MapManager::getMapObjectData() const {
    return m_objectData;
}

// =============================================================================
//  OBJECT GROUP PARSER
// =============================================================================

void MapManager::parseObjectGroups(tinyxml2::XMLElement* mapElement) {
    for (auto* groupEl = mapElement->FirstChildElement("objectgroup"); groupEl;
         groupEl = groupEl->NextSiblingElement("objectgroup")) {

        for (auto* objEl = groupEl->FirstChildElement("object"); objEl;
             objEl = objEl->NextSiblingElement("object")) {

            const char* typeName  = objEl->Attribute("name");
            const char* typeAttr  = objEl->Attribute("type");
            const char* classAttr = objEl->Attribute("class");

            std::string typeStr;
            if (typeAttr  && typeAttr[0]  != '\0') typeStr = typeAttr;
            else if (classAttr && classAttr[0] != '\0') typeStr = classAttr;
            else if (typeName  && typeName[0]  != '\0') typeStr = typeName;

            // Also check <property name="type" value="..."/>
            if (auto* propsEl = objEl->FirstChildElement("properties")) {
                for (auto* prop = propsEl->FirstChildElement("property"); prop;
                     prop = prop->NextSiblingElement("property")) {
                    const char* pname = prop->Attribute("name");
                    const char* pval  = prop->Attribute("value");
                    if (pname && pval && std::string(pname) == "type" && pval[0] != '\0') {
                        typeStr = pval;
                        break;
                    }
                }
            }

            if (typeStr.empty()) continue;

            float x = 0.f, y = 0.f;
            objEl->QueryFloatAttribute("x", &x);
            objEl->QueryFloatAttribute("y", &y);

            if (typeStr == "PlayerSpawn") {
                m_objectData.playerSpawn = { x, y, true };
                std::cout << "[MapManager] PlayerSpawn at (" << x << ", " << y << ")" << std::endl;
                continue;
            }

            if (typeStr == "Star") {
                EntitySpawnData item;
                item.type = typeStr; item.x = x; item.y = y;
                m_objectData.itemSpawns.push_back(item);
                continue;
            }

            if (typeStr == "STATIC_COIN" || typeStr == "Coin") {
                EntitySpawnData item;
                item.type = "STATIC_COIN"; item.x = x; item.y = y;
                m_objectData.itemSpawns.push_back(item);
                continue;
            }

            if (typeStr == "Lift") {
                LiftSpawnData lift;
                lift.x = x; lift.y = y;
                if (auto* propsEl = objEl->FirstChildElement("properties")) {
                    for (auto* prop = propsEl->FirstChildElement("property"); prop;
                         prop = prop->NextSiblingElement("property")) {
                        const char* pname = prop->Attribute("name");
                        const char* pval  = prop->Attribute("value");
                        if (!pname || !pval) continue;
                        std::string name(pname);
                        if (name == "motionType") lift.motionType = pval;
                        else if (name == "holes")  { try { lift.holes = std::stoi(pval); } catch (...) {} }
                        else if (name == "range")  { try { lift.range = std::stof(pval); } catch (...) {} }
                        else if (name == "speed")  { try { lift.speed = std::stof(pval); } catch (...) {} }
                    }
                }
                m_objectData.liftSpawns.push_back(lift);
                continue;
            }

            // ── FireBar objects ───────────────────────────────────────────
            auto isFireBarType = [](const std::string& str) {
                return (str == "FireBar" || str == "FIRE_BAR" || str == "FIREBAR" ||
                        str == "Firebar" || str == "fire_bar" || str == "firebar");
            };

            if (isFireBarType(typeStr)) {
                FireBarSpawnData fb;
                fb.x = x; fb.y = y;
                float width = 0.f, height = 0.f;
                objEl->QueryFloatAttribute("width",  &width);
                objEl->QueryFloatAttribute("height", &height);
                if (width > 0.f && height > 0.f) { fb.x += width / 2.f; fb.y += height / 2.f; }
                if (auto* propsEl = objEl->FirstChildElement("properties")) {
                    for (auto* prop = propsEl->FirstChildElement("property"); prop;
                         prop = prop->NextSiblingElement("property")) {
                        const char* pname = prop->Attribute("name");
                        const char* pval  = prop->Attribute("value");
                        if (!pname || !pval) continue;
                        std::string name(pname);
                        if (name == "fireCount" || name == "length") {
                            try { fb.fireCount = std::stoi(pval); } catch (...) {}
                        } else if (name == "speed") {
                            try { fb.speed = std::stof(pval); } catch (...) {}
                        } else if (name == "clockwise") {
                            fb.clockwise = (std::string(pval) == "true" || std::string(pval) == "1");
                        } else if (name == "initialAngle") {
                            try { fb.initialAngle = std::stof(pval); } catch (...) {}
                        }
                    }
                }
                m_objectData.fireBarSpawns.push_back(fb);
                std::cout << "[MapManager] Object FireBar at (" << fb.x << ", " << fb.y << ")" << std::endl;
                continue;
            }

            // Otherwise treat as enemy spawn
            EntitySpawnData spawn;
            spawn.type = typeStr; spawn.x = x; spawn.y = y;
            if (auto* propsEl = objEl->FirstChildElement("properties")) {
                for (auto* prop = propsEl->FirstChildElement("property"); prop;
                     prop = prop->NextSiblingElement("property")) {
                    const char* pname = prop->Attribute("name");
                    const char* pval  = prop->Attribute("value");
                    if (!pname || !pval) continue;
                    if (std::string(pname) == "direction") spawn.direction = pval;
                    if (std::string(pname) == "moveSpeed") {
                        try { spawn.moveSpeed = std::stof(pval); } catch (...) {}
                    }
                }
            }
            m_objectData.enemySpawns.push_back(spawn);
        }
    }

    std::cout << "[MapManager] Parsed " << m_objectData.enemySpawns.size()
              << " enemy spawn(s) from object layers." << std::endl;
}

// =============================================================================
//  UPDATE
// =============================================================================

void MapManager::update(float deltaTime) {
    // ── MULTI_COIN countdowns & animation ─────────────────────────────────────
    for (auto it = m_multiCoinStates.begin(); it != m_multiCoinStates.end(); ) {
        MultiCoinState& state = it->second;
        if (state.active) {
            state.timer -= deltaTime;
            state.animTimer += deltaTime;

            // Toggle animation frame every 0.15s between MULTI_COIN and MULTI_COIN2
            if (state.animTimer >= 0.15f) {
                state.animTimer = 0.f;
                state.isFrame2 = !state.isFrame2;
                int gx = it->first.first;
                int gy = it->first.second;
                if (gy >= 0 && gy < (int)m_mapData.size() &&
                    gx >= 0 && gx < (int)m_mapData[0].size()) {
                    TileType currentFrame = state.isFrame2 ? TileType::MULTI_COIN2 : TileType::MULTI_COIN;
                    auto gidIt = m_typeToGid.find(currentFrame);
                    if (gidIt != m_typeToGid.end()) {
                        m_rawGids[gy][gx] = gidIt->second;
                    }
                }
            }

            if (state.timer <= 0.f) {
                int gx = it->first.first;
                int gy = it->first.second;
                if (gy >= 0 && gy < (int)m_mapData.size() &&
                    gx >= 0 && gx < (int)m_mapData[0].size()) {
                    setTileInternal(gx, gy, TileType::QUESTION_USED);
                }
                it = m_multiCoinStates.erase(it);
                continue;
            }
        }
        ++it;
    }

    // ── Brick debris ────────────────────────────────────────────────────────
    for (auto& d : m_brickDebris) {
        d.vel.y += DEBRIS_GRAVITY * deltaTime;
        d.pos   += d.vel * deltaTime;
        d.life  -= deltaTime;
    }
    m_brickDebris.erase(
        std::remove_if(m_brickDebris.begin(), m_brickDebris.end(),
            [](const BrickDebris& d){ return d.life <= 0.f; }),
        m_brickDebris.end());

    // ── Block bump animations ─────────────────────────────────────────────────
    for (auto& b : m_blockBumpAnims) {
        b.time += deltaTime;
    }
    m_blockBumpAnims.erase(
        std::remove_if(m_blockBumpAnims.begin(), m_blockBumpAnims.end(),
            [](const BlockBumpAnim& b){ return b.time >= BlockBumpAnim::DURATION; }),
        m_blockBumpAnims.end());

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

    // ── Flag slide animation ──────────────────────────────────────────────────
    if (m_flagAnim.active) {
        m_flagAnim.pos.y += m_flagAnim.speed * deltaTime;
        if (m_flagAnim.pos.y >= m_flagAnim.targetY) {
            m_flagAnim.pos.y = m_flagAnim.targetY;
            m_flagAnim.finished = true;
        }
    }
}

// =============================================================================
//  RENDER
// =============================================================================

void MapManager::render(sf::RenderWindow& window) const {
    sf::RectangleShape tileShape(sf::Vector2f((float)m_tileSize, (float)m_tileSize));

    const bool hasRawGids = !m_rawGids.empty();

    for (size_t y = 0; y < m_mapData.size(); ++y) {
        for (size_t x = 0; x < m_mapData[y].size(); ++x) {
            TileType type = m_mapData[y][x];

            const float worldX = (float)x * m_tileSize;
            const float originalWorldY = (float)y * m_tileSize;
            float worldY = originalWorldY;

            bool hasBump = false;
            // Check if this block currently has a bump bounce animation
            for (const auto& b : m_blockBumpAnims) {
                if (b.gx == static_cast<int>(x) && b.gy == static_cast<int>(y)) {
                    float progress = std::clamp(b.time / BlockBumpAnim::DURATION, 0.f, 1.f);
                    float offsetY = BlockBumpAnim::MAX_OFFSET * std::sin(progress * 3.14159265f);
                    worldY += offsetY;
                    hasBump = true;
                    break;
                }
            }

            if (m_textureLoaded && hasRawGids &&
                y < m_rawGids.size() && x < m_rawGids[y].size()) {

                // If block is bouncing upward, draw the background underneath at its resting position
                if (hasBump) {
                    int bgGid = (y < m_bgGids.size() && x < m_bgGids[y].size()) ? m_bgGids[y][x] : 0;
                    if (bgGid <= 0) {
                        auto it = m_typeToGid.find(TileType::EMPTY);
                        if (it != m_typeToGid.end()) bgGid = it->second;
                        else bgGid = m_tilesetFirstGid;
                    }
                    if (bgGid > 0) {
                        const LoadedTileset* tsBg = nullptr;
                        for (const auto& candidate : m_tilesets) {
                            if (bgGid >= candidate.firstGid) {
                                if (!tsBg || candidate.firstGid > tsBg->firstGid) {
                                    tsBg = &candidate;
                                }
                            }
                        }
                        int colsBg = tsBg ? tsBg->columns : m_tilesetColumns;
                        int fgidBg = tsBg ? tsBg->firstGid : m_tilesetFirstGid;
                        if (colsBg > 0) {
                            int localIdBg = bgGid - fgidBg;
                            int tileColBg = localIdBg % colsBg;
                            int tileRowBg = localIdBg / colsBg;
                            m_tileSprite->setTextureRect(sf::IntRect(
                                {tileColBg * m_tileSize, tileRowBg * m_tileSize},
                                {m_tileSize, m_tileSize}));
                            m_tileSprite->setPosition({worldX, originalWorldY});
                            window.draw(*m_tileSprite);
                        }
                    }
                }

                int gid = m_rawGids[y][x];
                if (gid > 0) {
                    // Find the best matching tileset for this GID
                    const LoadedTileset* ts = nullptr;
                    for (const auto& candidate : m_tilesets) {
                        if (gid >= candidate.firstGid) {
                            if (!ts || candidate.firstGid > ts->firstGid) {
                                ts = &candidate;
                            }
                        }
                    }

                    int cols = ts ? ts->columns : m_tilesetColumns;
                    int fgid = ts ? ts->firstGid : m_tilesetFirstGid;

                    if (cols > 0) {
                        int localId  = gid - fgid;
                        int tileCol  = localId % cols;
                        int tileRow  = localId / cols;
                        m_tileSprite->setTextureRect(sf::IntRect(
                            {tileCol * m_tileSize, tileRow * m_tileSize},
                            {m_tileSize, m_tileSize}));
                        m_tileSprite->setPosition({worldX, worldY});
                        window.draw(*m_tileSprite);
                        continue;
                    }
                }
            }

            // ── Fallback: solid colour (texture not available) ────────────────
            if (type == TileType::EMPTY || type == TileType::HIDDEN_BLOCK ||
                type == TileType::BACKGROUND) continue;
            tileShape.setPosition(sf::Vector2f(worldX, worldY));
            switch (type) {
                case TileType::GROUND:           tileShape.setFillColor(sf::Color(139,  69,  19)); break;
                case TileType::PIPE:             tileShape.setFillColor(sf::Color::Green);          break;
                case TileType::BRICK_NORMAL:     tileShape.setFillColor(sf::Color(205, 133,  63)); break;
                case TileType::BRICK_SOLID:      tileShape.setFillColor(sf::Color(190, 120,  55)); break;
                case TileType::SOLID_BRICK:      tileShape.setFillColor(sf::Color(160, 110,  80)); break;
                case TileType::QUESTION_USED:    tileShape.setFillColor(sf::Color(130,  90,  60)); break;
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

    // ── Flag slide animation ──────────────────────────────────────────────────
    if (m_flagAnim.active && m_tileSprite) {
        const LoadedTileset* ts = nullptr;
        for (const auto& candidate : m_tilesets) {
            if (GID_FLAGPOLE_FLAG >= candidate.firstGid) {
                if (!ts || candidate.firstGid > ts->firstGid) {
                    ts = &candidate;
                }
            }
        }
        int cols = ts ? ts->columns : m_tilesetColumns;
        int fgid = ts ? ts->firstGid : m_tilesetFirstGid;

        if (cols > 0) {
            int localId = GID_FLAGPOLE_FLAG - fgid;
            int tileCol = localId % cols;
            int tileRow = localId / cols;
            m_tileSprite->setTextureRect(sf::IntRect(
                {tileCol * m_tileSize, tileRow * m_tileSize},
                {m_tileSize, m_tileSize}));
            m_tileSprite->setPosition(m_flagAnim.pos);
            window.draw(*m_tileSprite);

            int localIdAttach = GID_FLAGPOLE_ATTACH - fgid;
            int tileColAttach = localIdAttach % cols;
            int tileRowAttach = localIdAttach / cols;
            m_tileSprite->setTextureRect(sf::IntRect(
                {tileColAttach * m_tileSize, tileRowAttach * m_tileSize},
                {m_tileSize, m_tileSize}));
            m_tileSprite->setPosition({m_flagAnim.pos.x + (float)m_tileSize, m_flagAnim.pos.y});
            window.draw(*m_tileSprite);
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
        coinShape.setFillColor(sf::Color(255, 215, 0, a));
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

TileType MapManager::getTileTypeAt(int gx, int gy) const {
    if (gy < 0 || gy >= (int)m_mapData.size() ||
        gx < 0 || gx >= (int)m_mapData[0].size()) {
        return TileType::GROUND;
    }
    return m_mapData[gy][gx];
}

bool MapManager::isSolid(float x, float y) const {
    int gridX = static_cast<int>(x) / m_tileSize;
    int gridY = static_cast<int>(y) / m_tileSize;

    if (gridY < 0 || gridY >= (int)m_mapData.size() ||
        gridX < 0 || gridX >= (int)m_mapData[0].size()) {
        return true;
    }
    TileType type = m_mapData[gridY][gridX];
    return getBlockBehavior(type).isSolid();
}

bool MapManager::isSolidFromBelow(float x, float y) const {
    int gridX = static_cast<int>(x) / m_tileSize;
    int gridY = static_cast<int>(y) / m_tileSize;

    if (gridY < 0 || gridY >= (int)m_mapData.size() ||
        gridX < 0 || gridX >= (int)m_mapData[0].size()) {
        return true;
    }
    TileType type = m_mapData[gridY][gridX];
    return getBlockBehavior(type).isSolidFromBelow();
}

// =============================================================================
//  TILE WRITE — INTERNAL (game logic, no undo) & IMapContext PUBLIC
// =============================================================================

void MapManager::setTileInternal(int gx, int gy, TileType type) {
    if (gy < 0 || gy >= (int)m_mapData.size() ||
        gx < 0 || gx >= (int)m_mapData[0].size()) return;

    m_mapData[gy][gx] = type;

    if (type == TileType::EMPTY) {
        // Restore the background tile that was visually beneath this block.
        int bg = m_bgGids[gy][gx];
        if (bg <= 0) {
            auto it = m_typeToGid.find(TileType::EMPTY);
            if (it != m_typeToGid.end()) {
                bg = it->second;
            } else {
                bg = m_tilesetFirstGid;
            }
        }
        m_rawGids[gy][gx] = bg;
    } else {
        auto it = m_typeToGid.find(type);
        m_rawGids[gy][gx] = (it != m_typeToGid.end()) ? it->second : 0;
    }
}

// IMapContext::setTile — called by block behaviors.
// Delegates to setTileInternal (game logic, no undo recording).
void MapManager::setTile(int gx, int gy, TileType type) {
    setTileInternal(gx, gy, type);
}

// =============================================================================
//  ON HIT FROM BELOW  (main dispatcher — Strategy Pattern)
// =============================================================================

void MapManager::onHitFromBelow(int gx, int gy, IPlayerManager* player) {
    if (gy < 0 || gy >= (int)m_mapData.size() ||
        gx < 0 || gx >= (int)m_mapData[0].size()) return;

    TileType type = getTileType(static_cast<float>(gx) * m_tileSize,
                                static_cast<float>(gy) * m_tileSize);
    // *this satisfies IMapContext — block behaviors call back through the interface.
    getBlockBehavior(type).onHitFromBelow(*this, gx, gy, player);
}

// =============================================================================
//  IMapContext SIDE-EFFECT API  (called by IBlockBehavior subclasses)
// =============================================================================

void MapManager::spawnCoinPop(int gx, int gy) {
    CoinPopAnim c;
    c.pos  = {
        (float)gx * m_tileSize + m_tileSize / 2.f - 4.f,
        (float)gy * m_tileSize - (float)m_tileSize
    };
    c.velY = COINPOP_INIT_VY;
    c.life = COINPOP_LIFE;
    m_coinPopAnims.push_back(c);

    if (m_itemManager)
        m_itemManager->spawnCoinPop(
            (float)gx * m_tileSize,
            (float)gy * m_tileSize);
}

void MapManager::spawnItemForFormType(int gx, int gy, int formType) {
    float worldX = (float)gx * m_tileSize;
    float worldY = (float)(gy - 1) * m_tileSize; // one tile above

    if (m_itemManager) {
        if (formType == 0) {
            m_itemManager->spawnMushroom(worldX, worldY);
        } else {
            m_itemManager->spawnFireFlower(worldX, worldY);
        }
    }
}

void MapManager::setMultiCoinActive(int gx, int gy) {
    auto key = std::make_pair(gx, gy);
    MultiCoinState& state = m_multiCoinStates[key];
    if (!state.active) {
        state.active = true;
        state.timer  = MULTI_COIN_DURATION;
        state.animTimer = 0.f;
        state.isFrame2  = false;
    }
}

void MapManager::killEnemiesAboveTile(int gx, int gy) {
    if (m_enemyManager)
        m_enemyManager->killEnemiesAboveTile(gx, gy);
}

void MapManager::spawnBlockBump(int gx, int gy, TileType finalType) {
    setTileInternal(gx, gy, finalType);

    // Reset any active bump animation on the same tile
    m_blockBumpAnims.erase(
        std::remove_if(m_blockBumpAnims.begin(), m_blockBumpAnims.end(),
            [gx, gy](const BlockBumpAnim& b) { return b.gx == gx && b.gy == gy; }),
        m_blockBumpAnims.end());

    m_blockBumpAnims.push_back(BlockBumpAnim{gx, gy, 0.f});
}

// =============================================================================
//  ANIMATION SPAWNERS
// =============================================================================

void MapManager::spawnBrickDebris(int gx, int gy) {
    const sf::Color brickColor(205, 133, 63);
    float tileX = (float)gx * m_tileSize;
    float tileY = (float)gy * m_tileSize;
    float half  = (float)m_tileSize / 2.f;

    struct PieceDesc { float ox, oy, vx, vy; };
    const PieceDesc pieces[4] = {
        { 0.f,  0.f,  -90.f, -280.f },
        { half, 0.f,   90.f, -280.f },
        { 0.f,  half, -60.f, -180.f },
        { half, half,  60.f, -180.f },
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

// =============================================================================
//  FLAG SLIDE
// =============================================================================

void MapManager::triggerFlagSlide(int poleGridX) {
    if (m_flagAnim.active) return;

    int topY = -1, bottomY = -1;

    for (int gy = 0; gy < (int)m_rawGids.size(); ++gy) {
        if (poleGridX >= 0 && poleGridX < (int)m_rawGids[gy].size()) {
            int gid = m_rawGids[gy][poleGridX];
            if (gid == GID_FLAGPOLE_TOP_BALL) {
                topY = gy + 1;
            } else if (gid == GID_FLAGPOLE_BASE ||
                       (topY != -1 && m_mapData[gy][poleGridX] == TileType::GROUND)) {
                bottomY = gy - 1;
                break;
            }
        }
    }

    if (topY == -1) topY = 5;
    if (bottomY == -1) bottomY = 12;

    int flagCol = poleGridX - 1;
    if (topY >= 0 && topY < (int)m_rawGids.size() &&
        flagCol >= 0 && flagCol < (int)m_rawGids[0].size()) {
        int skyGid = (topY > 0) ? m_rawGids[topY - 1][flagCol] : 1;
        if (skyGid == 0) skyGid = 1;
        m_rawGids[topY][flagCol] = skyGid;
        m_mapData[topY][flagCol] = TileType::EMPTY;
        if (m_rawGids[topY][poleGridX] == GID_FLAGPOLE_ATTACH) {
            m_rawGids[topY][poleGridX] = GID_FLAGPOLE_SHAFT;
        }
    }

    m_flagAnim.pos      = { (float)flagCol * (float)m_tileSize, (float)topY * (float)m_tileSize };
    m_flagAnim.targetY  = (float)bottomY * (float)m_tileSize;
    m_flagAnim.speed    = FLAG_SLIDE_SPEED;
    m_flagAnim.active   = true;
    m_flagAnim.finished = false;
}

// =============================================================================
//  MAP EDITOR API
// =============================================================================

void MapManager::editTile(int gx, int gy, TileType newType) {
    if (gy < 0 || gy >= (int)m_mapData.size() ||
        gx < 0 || gx >= (int)m_mapData[0].size()) return;

    // Record undo entry before mutating
    TileEdit edit;
    edit.gx      = gx;
    edit.gy      = gy;
    edit.oldType = m_mapData[gy][gx];
    edit.oldGid  = m_rawGids[gy][gx];
    edit.newType = newType;
    auto it      = m_typeToGid.find(newType);
    edit.newGid  = (it != m_typeToGid.end()) ? it->second : 0;

    m_undoStack.push_back(edit);
    m_redoStack.clear(); // branching edit invalidates redo history

    setTileInternal(gx, gy, newType);
}

bool MapManager::undoEdit() {
    if (m_undoStack.empty()) return false;

    TileEdit edit = m_undoStack.back();
    m_undoStack.pop_back();
    m_redoStack.push_back(edit);

    // Restore old state directly (bypass setTileInternal's bgGid logic)
    m_mapData[edit.gy][edit.gx]  = edit.oldType;
    m_rawGids[edit.gy][edit.gx]  = edit.oldGid;
    return true;
}

bool MapManager::redoEdit() {
    if (m_redoStack.empty()) return false;

    TileEdit edit = m_redoStack.back();
    m_redoStack.pop_back();
    m_undoStack.push_back(edit);

    m_mapData[edit.gy][edit.gx]  = edit.newType;
    m_rawGids[edit.gy][edit.gx]  = edit.newGid;
    return true;
}

bool MapManager::saveToTMX(const std::string& path) const {
    if (m_mapData.empty() || m_mapData[0].empty()) return false;

    const int mapH = (int)m_mapData.size();
    const int mapW = (int)m_mapData[0].size();

    std::ofstream out(path);
    if (!out.is_open()) {
        std::cerr << "[MapManager] saveToTMX: cannot open " << path << std::endl;
        return false;
    }

    // ── XML header & <map> ─────────────────────────────────────────────────
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<map version=\"1.10\" tiledversion=\"1.12.2\""
        << " orientation=\"orthogonal\" renderorder=\"right-down\""
        << " width=\""      << mapW << "\""
        << " height=\""     << mapH << "\""
        << " tilewidth=\""  << m_tileSize << "\""
        << " tileheight=\"" << m_tileSize << "\""
        << " infinite=\"0\">\n";

    // ── <tileset> reference ────────────────────────────────────────────────
    if (!m_tsxRelativePath.empty()) {
        out << " <tileset firstgid=\"" << m_tilesetFirstGid
            << "\" source=\"" << m_tsxRelativePath << "\"/>\n";
    }

    // ── <layer> with merged GID grid in CSV encoding ───────────────────────
    out << " <layer id=\"1\" name=\"Terrain\""
        << " width=\"" << mapW << "\" height=\"" << mapH << "\">\n";
    out << "  <data encoding=\"csv\">\n";
    for (int r = 0; r < mapH; ++r) {
        for (int c = 0; c < mapW; ++c) {
            out << m_rawGids[r][c];
            if (c < mapW - 1) out << ',';
        }
        if (r < mapH - 1) out << ',';
        out << '\n';
    }
    out << "  </data>\n";
    out << " </layer>\n";

    // ── <objectgroup> with spawn data ──────────────────────────────────────
    out << " <objectgroup id=\"2\" name=\"Entities\">\n";
    int objId = 1;

    if (m_objectData.playerSpawn.found) {
        out << "  <object id=\"" << objId++ << "\" name=\"PlayerSpawn\""
            << " x=\"" << m_objectData.playerSpawn.x
            << "\" y=\"" << m_objectData.playerSpawn.y << "\"/>\n";
    }

    for (const auto& s : m_objectData.enemySpawns) {
        out << "  <object id=\"" << objId++ << "\" name=\"" << s.type << "\""
            << " x=\"" << s.x << "\" y=\"" << s.y << "\"";
        bool hasProps = (s.direction != "left" || s.moveSpeed >= 0.f);
        if (hasProps) {
            out << ">\n   <properties>\n";
            if (s.direction != "left")
                out << "    <property name=\"direction\" value=\"" << s.direction << "\"/>\n";
            if (s.moveSpeed >= 0.f)
                out << "    <property name=\"moveSpeed\" value=\"" << s.moveSpeed << "\"/>\n";
            out << "   </properties>\n  </object>\n";
        } else {
            out << "/>\n";
        }
    }

    for (const auto& s : m_objectData.itemSpawns) {
        out << "  <object id=\"" << objId++ << "\" name=\"" << s.type << "\""
            << " x=\"" << s.x << "\" y=\"" << s.y << "\"/>\n";
    }

    for (const auto& lift : m_objectData.liftSpawns) {
        out << "  <object id=\"" << objId++ << "\" name=\"Lift\""
            << " x=\"" << lift.x << "\" y=\"" << lift.y << "\">\n"
            << "   <properties>\n"
            << "    <property name=\"motionType\" value=\"" << lift.motionType << "\"/>\n"
            << "    <property name=\"holes\" value=\""      << lift.holes      << "\"/>\n"
            << "    <property name=\"range\" value=\""      << lift.range      << "\"/>\n"
            << "    <property name=\"speed\" value=\""      << lift.speed      << "\"/>\n"
            << "   </properties>\n"
            << "  </object>\n";
    }

    for (const auto& fb : m_objectData.fireBarSpawns) {
        out << "  <object id=\"" << objId++ << "\" name=\"FireBar\""
            << " x=\"" << fb.x << "\" y=\"" << fb.y << "\">\n"
            << "   <properties>\n"
            << "    <property name=\"fireCount\"    value=\"" << fb.fireCount    << "\"/>\n"
            << "    <property name=\"speed\"        value=\"" << fb.speed        << "\"/>\n"
            << "    <property name=\"clockwise\"    value=\"" << (fb.clockwise ? "true" : "false") << "\"/>\n"
            << "    <property name=\"initialAngle\" value=\"" << fb.initialAngle << "\"/>\n"
            << "   </properties>\n"
            << "  </object>\n";
    }

    out << " </objectgroup>\n";
    out << "</map>\n";

    std::cout << "[MapManager] Saved TMX: " << path << std::endl;
    return true;
}

void MapManager::addEnemySpawn(const EntitySpawnData& spawn) {
    m_objectData.enemySpawns.push_back(spawn);
}

void MapManager::removeEnemySpawn(int index) {
    if (index < 0 || index >= (int)m_objectData.enemySpawns.size()) return;
    m_objectData.enemySpawns.erase(m_objectData.enemySpawns.begin() + index);
}