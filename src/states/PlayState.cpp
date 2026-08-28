#include "PlayState.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "WinState.h"
#include "IntermissionState.h"
#include "../core/StateManager.h"
#include "../core/GameConfig.h"
#include "../world/GameWorld.h"
#include "../entities/player/Mario.h"
#include "../entities/player/Luigi.h"
#include "../entities/player/input/KeyBindingPresets.h"
#include "../entities/map/MapManager.h"
#include "../entities/camera/CameraManager.h"
#include "../entities/player/PlayerManager.h"
#include "../entities/enemy/EnemyManager.h"
#include "../entities/item/ItemManager.h"
#include "../ui/HUDManager.h"
#include "../ui/SaveManager.h"
#include <SFML/Graphics.hpp>
#include <memory>

// ──────────────────────────────────────────────────────────────────
// Helper: tạo đúng subclass theo CharacterType
// ──────────────────────────────────────────────────────────────────
static std::shared_ptr<PlayerManager> makePlayer(CharacterType type)
{
    if (type == CharacterType::Luigi)
        return std::make_shared<Luigi>();
    return std::make_shared<Mario>();
}

// ──────────────────────────────────────────────────────────────────
// setup() – logic khởi tạo chung, dùng bởi cả 2 constructor
// ──────────────────────────────────────────────────────────────────
void PlayState::setup(const GameConfig& config)
{
    m_config = config;
    m_gameWorld = std::make_unique<GameWorld>();

    // ── Map / Camera ─────────────────────────────────────────────
    m_gameWorld->setMapManager(std::make_shared<MapManager>());
    m_gameWorld->setCameraManager(std::make_shared<CameraManager>());

    // ── Player 1 ─────────────────────────────────────────────────
    auto p1 = makePlayer(config.player1Character);
    if (config.mode == GameMode::SinglePlayer)
        p1->setKeyBinding(KeyBindingPresets::singlePlayer());
    else
        p1->setKeyBinding(KeyBindingPresets::player1TwoPlayer());

    m_gameWorld->setPlayerManager(p1);

    // ── Player 2 (chỉ ở 2P mode) ─────────────────────────────────
    if (config.mode == GameMode::TwoPlayer)
    {
        auto p2 = makePlayer(config.player2Character);
        p2->setKeyBinding(KeyBindingPresets::player2TwoPlayer());
        m_gameWorld->setPlayerManager2(p2);
    }

    // ── Enemies / Items / HUD / Save ─────────────────────────────
    m_gameWorld->setEnemyManager(std::make_shared<EnemyManager>());
    m_gameWorld->setItemManager(std::make_shared<ItemManager>());
    auto hud = std::make_shared<HUDManager>();
    hud->setCharacter(config.player1Character);
    m_gameWorld->setHUDManager(hud);
    if (!m_saveManager)
        m_saveManager = std::make_shared<SaveManager>();
    m_gameWorld->setSaveManager(m_saveManager);

    // Inject shared settings (key bindings for the player) before init
    m_gameWorld->setSettings(m_settings);

    // Initialize the game world
    m_gameWorld->initialize();

    // When starting from a save (Continue in the main menu), apply the
    // saved player state after the world is fully initialized
    if (m_loadSave)
    {
        if (auto* save = m_gameWorld->getSaveManager())
        {
            if (save->loadGame())
            {
                int savedLevel = save->getSavedLevel();
                m_gameWorld->setStage(savedLevel);
                m_gameWorld->setSharedLives(save->getSavedLives());
                if (auto* player = m_gameWorld->getPlayerManager())
                {
                    player->restoreState(save->getSavedScore(), save->getSavedLives(),
                                         player->getPositionX(), player->getPositionY());
                    player->setCoins(save->getSavedCoins());
                }
                if (auto* player2 = m_gameWorld->getPlayerManager2())
                {
                    player2->restoreState(0, save->getSavedLives(),
                                          player2->getPositionX(), player2->getPositionY());
                    player2->setCoins(0);
                }
                if (auto* hud = m_gameWorld->getHUDManager())
                {
                    hud->updateScore(save->getSavedScore());
                    hud->updateItemCount(save->getSavedCoins());
                    hud->updateLives(save->getSavedLives());
                    hud->updateWorld(savedLevel);
                }
            }
        }
    }

    m_levelStartScore = m_gameWorld->getTotalScore();
    m_levelStartLives = m_gameWorld->getSharedLives();
    m_levelStartCoins = m_gameWorld->getTotalCoins();
}

// ──────────────────────────────────────────────────────────────────
// Constructor nhận config từ ModeSelectState
// ──────────────────────────────────────────────────────────────────
PlayState::PlayState(const GameConfig& config, std::shared_ptr<ISettingsManager> settings, std::shared_ptr<ISaveManager> saveManager, bool loadSave)
    : m_settings(std::move(settings))
    , m_saveManager(std::move(saveManager))
    , m_loadSave(loadSave)
{
    setup(config);
}

// ──────────────────────────────────────────────────────────────────
// Constructor mặc định: 1P với Mario (dùng cho quick-test / debug)
// ──────────────────────────────────────────────────────────────────
PlayState::PlayState()
    : m_settings(nullptr)
    , m_saveManager(nullptr)
    , m_loadSave(false)
{
    GameConfig defaultConfig;
    defaultConfig.player1Character = CharacterType::Mario;
    defaultConfig.mode             = GameMode::SinglePlayer;
    setup(defaultConfig);
}

PlayState::~PlayState()
{
    // GameWorld cleanup via unique_ptr
}

void PlayState::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::Escape)
        {
            auto* manager = getStateManager();
            if (manager)
            {
                // pause menu can save it if the player picks "Save & Quit".
                // We use level start values to prevent farming lives/coins mid-level.
                if (auto* save = m_gameWorld->getSaveManager())
                {
                    save->setSaveData(m_levelStartScore,
                                      m_levelStartLives,
                                      m_gameWorld->getCurrentStageNumber(),
                                      m_levelStartCoins);
                    save->setGameConfig(static_cast<int>(m_config.player1Character),
                                        static_cast<int>(m_config.player2Character),
                                        static_cast<int>(m_config.mode));
                }
                manager->pushState(std::make_unique<PauseState>(m_settings, m_saveManager, m_gameWorld->getPlayerManager(), m_gameWorld->getPlayerManager2()));
            }
            return; // don't forward the pause key to the game world
        }
    }

    m_gameWorld->handleInput(event);
}

void PlayState::update(float deltaTime)
{
    m_gameWorld->update(deltaTime);

    if (m_gameWorld->isGameWon())
    {
        auto* manager = getStateManager();
        if (manager)
        {
            // Game completed successfully: clear save file
            m_gameWorld->deleteSaveData();

            int finalScore = m_gameWorld->getTotalScore();
            int livesLeft  = m_gameWorld->getSharedLives();
            auto winState  = std::make_unique<WinState>(m_settings, m_saveManager, m_config, finalScore, livesLeft);
            manager->changeState(std::move(winState));
        }
    }
    else if (m_gameWorld->isStageClear())
    {
        auto* manager = getStateManager();
        if (manager)
        {
            int cur = m_gameWorld->getCurrentStageNumber();
            int next = m_gameWorld->getNextStageNumber();
            int lives = m_gameWorld->getSharedLives();
            auto intermission = std::make_unique<IntermissionState>(
                m_config, cur, next, lives,
                [this]() {
                    if (m_gameWorld)
                    {
                        m_gameWorld->advanceStage();
                        
                        m_levelStartScore = m_gameWorld->getTotalScore();
                        m_levelStartLives = m_gameWorld->getSharedLives();
                        m_levelStartCoins = m_gameWorld->getTotalCoins();

                        if (m_saveManager)
                        {
                            m_saveManager->setSaveData(m_levelStartScore,
                                                       m_levelStartLives,
                                                       m_gameWorld->getCurrentStageNumber(),
                                                       m_levelStartCoins);
                            m_saveManager->setGameConfig(static_cast<int>(m_config.player1Character),
                                                         static_cast<int>(m_config.player2Character),
                                                         static_cast<int>(m_config.mode));
                            m_saveManager->saveGame();
                        }
                    }
                }
            );
            manager->pushState(std::move(intermission));
        }
    }
    else if (m_gameWorld->isGameOver())
    {
        auto* manager = getStateManager();
        if (manager)
        {
            // Delete save file — the player lost, no game to continue
            m_gameWorld->deleteSaveData();

            int finalScore = m_gameWorld->getTotalScore();
            auto gameOverState = std::make_unique<GameOverState>(m_settings, m_saveManager, m_config);
            gameOverState->setFinalScore(finalScore);
            manager->changeState(std::move(gameOverState));
        }
    }
}

void PlayState::render(sf::RenderWindow& window) const
{
    m_gameWorld->render(window);
}
