#include "PlayState.h"
#include "PauseState.h"
#include "GameOverState.h"
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
    m_gameWorld->setHUDManager(std::make_shared<HUDManager>());
    m_gameWorld->setSaveManager(std::make_shared<SaveManager>());

    m_gameWorld->initialize();
}

// ──────────────────────────────────────────────────────────────────
// Constructor nhận config từ ModeSelectState
// ──────────────────────────────────────────────────────────────────
PlayState::PlayState(const GameConfig& config)
{
    setup(config);
}

// ──────────────────────────────────────────────────────────────────
// Constructor mặc định: 1P với Mario (dùng cho quick-test / debug)
// ──────────────────────────────────────────────────────────────────
PlayState::PlayState()
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
        if (keyEvent->code == sf::Keyboard::Key::P)
        {
            auto* manager = getStateManager();
            if (manager)
                manager->pushState(std::make_unique<PauseState>());
            return;
        }
    }

    m_gameWorld->handleInput(event);
}

void PlayState::update(float deltaTime)
{
    m_gameWorld->update(deltaTime);

    if (m_gameWorld->isGameOver())
    {
        auto* manager = getStateManager();
        if (manager)
        {
            int finalScore = m_gameWorld->getTotalScore();
            auto gameOverState = std::make_unique<GameOverState>();
            gameOverState->setFinalScore(finalScore);
            manager->changeState(std::move(gameOverState));
        }
    }
}

void PlayState::render(sf::RenderWindow& window) const
{
    m_gameWorld->render(window);
}
