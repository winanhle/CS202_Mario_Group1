#pragma once

#include "../interfaces/ISettingsManager.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

/**
 * @class SettingsMenu
 * @brief Reusable settings/pause menu UI component.
 *
 * Developer: Nguyen Phuc
 *
 * Single Responsibility: menu UI + navigation only. No persistence
 * (SettingsManager owns that), no state transitions (the owning state
 * interprets the Request returned from handleInput).
 *
 * Two usage contexts:
 * - Pause context (PauseState): shows a root menu (Resume / Settings /
 *   Quit to Menu) and a settings sub-screen.
 * - Main menu context (MenuState): opens directly on the settings
 *   sub-screen; ESC / Back requests ExitSettings.
 *
 * Input:
 * - Up/Down: navigate items
 * - Left/Right: adjust the volume slider
 * - Enter: activate the selected item (open settings, start rebind, etc.)
 * - ESC: go back / resume / exit settings
 * - While rebinding: any key is assigned to the selected action
 */
class SettingsMenu
{
public:
    /**
     * @brief Requests the owning state can act on after handleInput()
     */
    enum class Request
    {
        None,
        Resume,       // pause context: return to the game
        QuitToMenu,   // pause context: leave the game, go to the main menu
        SaveAndQuit,  // pause context: save the game, then go to the main menu
        ExitSettings  // main menu context: leave settings mode
    };

    /**
     * @param settings Reference to the shared settings manager (injected)
     * @param pauseContext True when shown from PauseState (root menu shown first)
     */
    SettingsMenu(ISettingsManager& settings, bool pauseContext);

    void openRoot();
    void openSettings();

    Request handleInput(const sf::Event& event);
    void update(float deltaTime);
    void render(sf::RenderWindow& window) const;

private:
    enum class Screen
    {
        Root,     // pause context only: Resume / Settings / Quit to Menu
        Settings, // volume + key bindings + back
        Rebind    // waiting for a key press
    };

    enum class ItemKind
    {
        Activate, // Enter triggers an action (Resume, Quit, Settings, Back)
        Slider,   // Left/Right adjusts the value
        Rebind    // Enter starts key-capture
    };

    struct MenuItem
    {
        std::string label;
        ItemKind kind;
        GameAction action; // used by Rebind items
        Request request;   // used by Activate items
    };

    void setupText(sf::Text& text, const std::string& str,
                   unsigned int size, sf::Color color) const;
    void moveSelection(int dx, int dy);
    Request activateSelected();
    Request goBack();
    void buildScreens();

    std::string keyName(sf::Keyboard::Key key) const;
    std::string actionLabel(GameAction action) const;

    // Mouse support: hover moves the selection, left click activates
    void handleMouseMove(sf::Vector2i position);
    Request handleMouseClick(sf::Vector2i position);
    std::vector<sf::FloatRect> computeItemRects() const;
    std::string itemDisplayText(const MenuItem& item) const;
    const std::vector<MenuItem>* currentItems() const;
    sf::Vector2f getItemPosition(int index) const;

    // Convert a mouse position from window pixels into the 800x600 view
    // space used by the menu layout (the window may be resized/maximized,
    // which stretches the fixed-size view)
    sf::Vector2f toViewCoords(sf::Vector2i position) const;

    // Shared layout constants (used by render and hit-testing)
    static constexpr float ITEM_CENTER_X = 400.0f;
    static constexpr float ITEM_START_Y = 200.0f;
    static constexpr float ITEM_SPACING = 55.0f;

    ISettingsManager& m_settings;
    bool m_pauseContext = false;

    Screen m_screen = Screen::Root;
    int m_selectedIndex = 0;
    GameAction m_rebindingAction = GameAction::P1Jump;
    bool m_rebindReserved = false; // true while showing "key reserved" hint

    sf::Font m_font;
    bool m_fontLoaded = false;

    // Window size tracked via render() so that mouse positions can be correctly
    // mapped into the fixed 800x600 view space when the window is resized or maximized.
    // mutable because render() (const) refreshes it every frame.
    mutable sf::Vector2u m_windowSize{800u, 600u};

    std::vector<MenuItem> m_rootItems;
    std::vector<MenuItem> m_settingsItems;
};