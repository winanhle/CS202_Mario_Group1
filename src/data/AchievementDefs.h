#pragma once

#include <string>
#include <vector>

/**
 * @file AchievementDefs.h
 * @brief Single source of truth for all achievement metadata.
 *
 * Fixes OCP/DRY: SaveManager::evaluateAchievements(),
 * SaveManager::getAchievementStatuses(), and StatsState::setupAchievements()
 * all reference this list instead of maintaining separate copies.
 */

// ==================== THRESHOLD CONSTANTS ====================

namespace Achv {
    constexpr int COIN_HOARDER       = 100;
    constexpr int PYROMANCER         = 25;
    constexpr int CENTURION          = 100;
    constexpr int TREASURE_HUNTER    = 500;
    constexpr int PERSEVERANCE       = 50;
    constexpr int HIGH_ROLLER        = 50000;
    constexpr int MARATHON_RUNNER    = 10;
    constexpr int BLOCK_BREAKER      = 50;
    constexpr int DEMOLITION         = 200;
    constexpr int ARCHITECT          = 1;
    constexpr int COIN_MAGNET        = 2000;
    constexpr int VETERAN            = 25;
    constexpr int CHAMPION           = 5;
    constexpr int MUSHROOM_ADDICT    = 25;
    constexpr int FIRE_LORD          = 20;
    constexpr int STAR_POWER         = 10;
    constexpr int GRIZZLED_VETERAN   = 50;
    constexpr int SPEEDRUNNER_TIME   = 250;  // seconds remaining on Stage 1
}

// ==================== METADATA ====================

struct AchievementDef {
    std::string id;
    std::string displayName;
    std::string description;
    std::string icon;
    
    // Evaluation rules: ALL of these stats must meet or exceed the given value.
    std::vector<std::pair<std::string, int>> requirements;
};

/**
 * @brief Returns the complete list of achievement definitions.
 *        Ordered the same way they appear in the trophy cabinet.
 */
inline const std::vector<AchievementDef>& getAchievementDefinitions()
{
    static const std::vector<AchievementDef> defs = {
        {"coin_hoarder",    "Coin Hoarder",     "Collect 100 lifetime coins",         "[COIN]",  {{"coins_collected", Achv::COIN_HOARDER}}},
        {"pyromancer",      "Pyromancer",        "Defeat 25 enemies with fireballs",   "[FIRE]",  {{"fireball_kills", Achv::PYROMANCER}}},
        {"power_trio",      "Power Trio",        "Get Mushroom, Fire Flower, Star",    "[STAR]",  {{"mushrooms_collected", 1}, {"fire_flowers_collected", 1}, {"stars_collected", 1}}},
        {"brotherhood",     "Brotherhood",       "Clear a stage in 2-Player Co-op",    "[2P]",    {{"coop_stages_cleared", 1}}},
        {"kingdom_savior",  "Kingdom Savior",    "Rescue Princess Peach",              "[WIN]",   {{"games_won", 1}}},
        {"speedrunner",     "Speedrunner",       "Clear Stage 1 with >250s left",      "[TIME]",  {{"speedrun_stage1", 1}}},
        {"centurion",       "Centurion",         "Defeat 100 enemies total",           "[SLAY]",  {{"enemies_killed", Achv::CENTURION}}},
        {"treasure_hunter", "Treasure Hunter",   "Collect 500 lifetime coins",         "[GOLD]",  {{"coins_collected", Achv::TREASURE_HUNTER}}},
        {"perseverance",    "Perseverance",      "Die 50 times",                       "[BONE]",  {{"total_deaths", Achv::PERSEVERANCE}}},
        {"high_roller",     "High Roller",       "Reach 50,000 lifetime score",        "[RICH]",  {{"total_score", Achv::HIGH_ROLLER}}},
        {"marathon_runner", "Marathon Runner",   "Play 10 games",                      "[RUN]",   {{"games_played", Achv::MARATHON_RUNNER}}},
        {"block_breaker",   "Block Breaker",     "Smash 50 brick blocks",              "[HULK]",  {{"blocks_broken", Achv::BLOCK_BREAKER}}},
        {"demolition",      "Demolition Expert", "Smash 200 brick blocks",             "[BOOM]",  {{"blocks_broken", Achv::DEMOLITION}}},
        {"architect",       "Architect",         "Play a custom Map Editor map",       "[EDIT]",  {{"editor_maps_played", Achv::ARCHITECT}}},
        {"coin_magnet",     "Coin Magnet",       "Collect 2,000 lifetime coins",       "[MAG]",   {{"coins_collected", Achv::COIN_MAGNET}}},
        {"veteran",         "Veteran",           "Play 25 games",                      "[MIL]",   {{"games_played", Achv::VETERAN}}},
        {"champion",        "Champion",          "Win 5 games",                        "[TRO]",   {{"games_won", Achv::CHAMPION}}},
        {"mushroom_addict", "Mushroom Addict",   "Collect 25 mushrooms",               "[SHRM]",  {{"mushrooms_collected", Achv::MUSHROOM_ADDICT}}},
        {"fire_lord",       "Fire Lord",         "Collect 20 fire flowers",            "[FLAM]",  {{"fire_flowers_collected", Achv::FIRE_LORD}}},
        {"star_power",      "Star Power",        "Collect 10 stars",                   "[GALAX]", {{"stars_collected", Achv::STAR_POWER}}},
        {"grizzled_veteran","Grizzled Veteran",  "Play 50 games",                      "[OLD]",   {{"games_played", Achv::GRIZZLED_VETERAN}}},
    };
    return defs;
}

/**
 * @brief Returns just the achievement IDs in display order.
 */
inline const std::vector<std::string>& getAchievementIds()
{
    static const std::vector<std::string> ids = [] {
        std::vector<std::string> result;
        for (const auto& def : getAchievementDefinitions())
            result.push_back(def.id);
        return result;
    }();
    return ids;
}
