#pragma once

#include <memory>

// Forward declarations for all modules
class IPlayerManager;
class IEnemyManager;
class IItemManager;
class IHUDManager;
class ISaveManager;

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @class GameWorld
 * @brief Central coordinator for all game modules
 * 
 * GameWorld acts as the mediator/facade that coordinates all major systems.
 * It provides extension points for each module without requiring them to know about each other.
 * 
 * Ownership:
 * - Owns all manager instances (Player, Enemy, Item, HUD, SaveManager)
 * - Provides controlled access to each system
 * 
 * Dependencies:
 * - Depends on module interfaces (IPlayerManager, IEnemyManager, etc.)
 * - Does NOT depend on concrete implementations (injected via setters)
 * 
 * Extension Points:
 * - setPlayerManager() - for Player module integration
 * - setEnemyManager() - for Enemy module integration
 * - setItemManager() - for Item module integration
 * - setHUDManager() - for UI module integration
 * - setSaveManager() - for SaveManager module integration
 */
class GameWorld
{
public:
    GameWorld();
    ~GameWorld() = default;

    // Delete copy operations - GameWorld is unique
    GameWorld(const GameWorld&) = delete;
    GameWorld& operator=(const GameWorld&) = delete;

    /**
     * @brief Initialize the game world
     * Called once at startup after all managers are set
     */
    void initialize();

    /**
     * @brief Update all game systems
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime);

    /**
     * @brief Render all game systems
     * @param window The SFML window to render to
     */
    void render(sf::RenderWindow& window) const;

    /**
     * @brief Handle input events
     * @param event The SFML event
     */
    void handleInput(const sf::Event& event);

    // ==================== EXTENSION POINTS ====================
    // Module setters - called during initialization
    // Each module is optional and can be nullptr

    /**
     * @brief Set the player manager implementation
     * EXTENSION POINT: Le Tran - Character/Player module
     */
    void setPlayerManager(std::shared_ptr<IPlayerManager> playerManager);

    /**
     * @brief Set the enemy manager implementation
     * EXTENSION POINT: Dinh Anh - Enemy module
     */
    void setEnemyManager(std::shared_ptr<IEnemyManager> enemyManager);

    /**
     * @brief Set the item manager implementation
     * EXTENSION POINT: Dinh Anh - Item module
     */
    void setItemManager(std::shared_ptr<IItemManager> itemManager);

    /**
     * @brief Set the HUD manager implementation
     * EXTENSION POINT: Nguyen Phuc - UI/HUD module
     */
    void setHUDManager(std::shared_ptr<IHUDManager> hudManager);

    /**
     * @brief Set the save manager implementation
     * EXTENSION POINT: Nguyen Phuc - SaveManager module
     */
    void setSaveManager(std::shared_ptr<ISaveManager> saveManager);

    // ==================== ACCESSORS ====================
    // Provide read-only access to modules for inter-module communication

    /**
     * @brief Get the player manager
     * Used by other systems that need to query player state
     */
    IPlayerManager* getPlayerManager();

    /**
     * @brief Get the enemy manager
     * Used by collision detection, UI, etc.
     */
    IEnemyManager* getEnemyManager();

    /**
     * @brief Get the item manager
     * Used by collision detection, player, etc.
     */
    IItemManager* getItemManager();

    /**
     * @brief Get the HUD manager
     * Used to update displayed information
     */
    IHUDManager* getHUDManager();

    /**
     * @brief Get the save manager
     * Used for save/load operations
     */
    ISaveManager* getSaveManager();

private:
    // Module manager instances (all optional)
    std::shared_ptr<IPlayerManager> m_playerManager;
    std::shared_ptr<IEnemyManager> m_enemyManager;
    std::shared_ptr<IItemManager> m_itemManager;
    std::shared_ptr<IHUDManager> m_hudManager;
    std::shared_ptr<ISaveManager> m_saveManager;

    // Internal state
    bool m_isInitialized;
};
