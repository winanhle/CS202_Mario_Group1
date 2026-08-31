#include "../ui/UIUtils.h"
#include "SettingsMenu.h"
#include "../core/GameState.h"
#include "../ui/UIUtils.h"
#include <SFML/Window/Event.hpp>
#include <algorithm>

SettingsMenu::SettingsMenu(ISettingsManager& settings, bool pauseContext)
    : m_settings(settings)
    , m_pauseContext(pauseContext)
    , m_screen(pauseContext ? Screen::Root : Screen::Settings)
    , m_selectedIndex(0)
    , m_rebindingAction(GameAction::P1Jump)
    , m_fontLoaded(false)
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");
    buildScreens();
}

void SettingsMenu::buildScreens()
{
    m_rootItems.clear();
    m_settingsItems.clear();

    if (m_pauseContext)
    {
        m_rootItems.push_back({ "RESUME", ItemKind::Activate, GameAction::P1Jump, Request::Resume });
        m_rootItems.push_back({ "SETTINGS", ItemKind::Activate, GameAction::P1Jump, Request::None });
        m_rootItems.push_back({ "SAVE & QUIT", ItemKind::Activate, GameAction::P1Jump, Request::SaveAndQuit });
        m_rootItems.push_back({ "QUIT TO MENU", ItemKind::Activate, GameAction::P1Jump, Request::QuitToMenu });
    }

    m_settingsItems.push_back({ "VOLUME", ItemKind::Slider, GameAction::P1Jump, Request::None });
    m_settingsItems.push_back({ "P1 JUMP", ItemKind::Rebind, GameAction::P1Jump, Request::None });
    m_settingsItems.push_back({ "P1 LEFT", ItemKind::Rebind, GameAction::P1MoveLeft, Request::None });
    m_settingsItems.push_back({ "P1 RIGHT", ItemKind::Rebind, GameAction::P1MoveRight, Request::None });
    m_settingsItems.push_back({ "P1 SHOOT", ItemKind::Rebind, GameAction::P1Shoot, Request::None });
    m_settingsItems.push_back({ "P2 JUMP", ItemKind::Rebind, GameAction::P2Jump, Request::None });
    m_settingsItems.push_back({ "P2 LEFT", ItemKind::Rebind, GameAction::P2MoveLeft, Request::None });
    m_settingsItems.push_back({ "P2 RIGHT", ItemKind::Rebind, GameAction::P2MoveRight, Request::None });
    m_settingsItems.push_back({ "P2 SHOOT", ItemKind::Rebind, GameAction::P2Shoot, Request::None });
    m_settingsItems.push_back({ "RESET TO DEFAULTS", ItemKind::Activate, GameAction::P1Jump, Request::None });
    m_settingsItems.push_back({ "BACK", ItemKind::Activate, GameAction::P1Jump, Request::None });
}

void SettingsMenu::openRoot()
{
    m_screen = Screen::Root;
    m_selectedIndex = 0;
}

void SettingsMenu::openSettings()
{
    m_screen = Screen::Settings;
    m_selectedIndex = 0;
}

void SettingsMenu::setupText(sf::Text& text, const std::string& str,
                             unsigned int size, sf::Color color) const
{
    text.setFont(m_font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.0f);
}

std::string SettingsMenu::keyName(sf::Keyboard::Key key) const
{
    // Convert the logical key to a scancode so the OS can localize it
    sf::Keyboard::Scancode scancode = sf::Keyboard::delocalize(key);
    if (scancode == sf::Keyboard::Scan::Unknown)
        return "?";
    return sf::Keyboard::getDescription(scancode).toAnsiString();
}

std::string SettingsMenu::actionLabel(GameAction action) const
{
    switch (action)
    {
    case GameAction::P1Jump: return "P1 JUMP";
    case GameAction::P1MoveLeft: return "P1 MOVE LEFT";
    case GameAction::P1MoveRight: return "P1 MOVE RIGHT";
    case GameAction::P1Shoot: return "P1 SHOOT";
    case GameAction::P2Jump: return "P2 JUMP";
    case GameAction::P2MoveLeft: return "P2 MOVE LEFT";
    case GameAction::P2MoveRight: return "P2 MOVE RIGHT";
    case GameAction::P2Shoot: return "P2 SHOOT";
    default: return "?";
    }
}

const std::vector<SettingsMenu::MenuItem>* SettingsMenu::currentItems() const
{
    switch (m_screen)
    {
    case Screen::Root: return &m_rootItems;
    case Screen::Settings: return &m_settingsItems;
    default: return nullptr;
    }
}

std::string SettingsMenu::itemDisplayText(const MenuItem& item) const
{
    std::string text = item.label;
    if (m_screen == Screen::Settings)
    {
        if (item.kind == ItemKind::Slider)
        {
            text += ": " + std::to_string(static_cast<int>(m_settings.getVolume()));
        }
        else if (item.kind == ItemKind::Rebind)
        {
            text += ": " + keyName(m_settings.getKey(item.action));
        }
    }
    return text;
}

sf::Vector2f SettingsMenu::getItemPosition(int i) const
{
    if (m_screen == Screen::Root)
    {
        return { ITEM_CENTER_X, ITEM_START_Y + i * ITEM_SPACING };
    }
    else if (m_screen == Screen::Settings)
    {
        if (i == 0) return { ITEM_CENTER_X, ITEM_START_Y }; // Volume
        if (i >= 1 && i <= 4) return { ITEM_CENTER_X - 180.0f, ITEM_START_Y + i * ITEM_SPACING }; // P1
        if (i >= 5 && i <= 8) return { ITEM_CENTER_X + 180.0f, ITEM_START_Y + (i - 4) * ITEM_SPACING }; // P2
        if (i == 9) return { ITEM_CENTER_X, ITEM_START_Y + 5 * ITEM_SPACING }; // Reset
        if (i == 10) return { ITEM_CENTER_X, ITEM_START_Y + 6 * ITEM_SPACING }; // Back
    }
    return { ITEM_CENTER_X, ITEM_START_Y };
}

std::vector<sf::FloatRect> SettingsMenu::computeItemRects() const
{
    std::vector<sf::FloatRect> rects;
    const std::vector<MenuItem>* items = currentItems();
    if (!items)
        return rects;

    for (int i = 0; i < static_cast<int>(items->size()); ++i)
    {
        sf::Vector2f pos = getItemPosition(i);

        if (m_screen == Screen::Settings && (*items)[i].kind == ItemKind::Slider)
        {
            // The volume slider encompasses label, slider track (160px), and percentage text
            const float leftX = pos.x - 155.0f;
            const float totalW = 375.0f;
            rects.push_back(sf::FloatRect({ leftX, pos.y - ITEM_SPACING / 2.0f }, { totalW, ITEM_SPACING }));
        }
        else
        {
            sf::Text itemText{m_font};
            setupText(itemText, itemDisplayText((*items)[i]), 28, sf::Color::White);

            UIUtils::centerOrigin(itemText);
            itemText.setPosition(pos);

            sf::FloatRect bounds = itemText.getLocalBounds();
            sf::Vector2f topLeft = itemText.getPosition() - itemText.getOrigin();
            // The hitbox height is ITEM_SPACING, centered on the text Y
            topLeft.y = pos.y - ITEM_SPACING / 2.0f;
            rects.push_back(sf::FloatRect(topLeft, { bounds.size.x, ITEM_SPACING }));
        }
    }
    return rects;
}

sf::Vector2f SettingsMenu::toViewCoords(sf::Vector2i position) const
{
    // The menu layout lives in the fixed 800x600 view space. When the
    // window is resized or maximized, SFML stretches that view to fill the
    // window, so raw window-pixel mouse positions must be scaled back into
    // view space before hit-testing.
    const float scaleX = 800.0f / static_cast<float>(m_windowSize.x);
    const float scaleY = 600.0f / static_cast<float>(m_windowSize.y);
    return { static_cast<float>(position.x) * scaleX,
             static_cast<float>(position.y) * scaleY };
}

void SettingsMenu::handleMouseMove(sf::Vector2i position)
{
    if (m_screen == Screen::Rebind)
        return;

    sf::Vector2f mouse = toViewCoords(position);
    std::vector<sf::FloatRect> rects = computeItemRects();
    for (int i = 0; i < static_cast<int>(rects.size()); ++i)
    {
        if (rects[i].contains(mouse))
        {
            m_selectedIndex = i;
            return;
        }
    }
}

SettingsMenu::Request SettingsMenu::handleMouseClick(sf::Vector2i position)
{
    if (m_screen == Screen::Rebind)
        return Request::None;

    sf::Vector2f mouse = toViewCoords(position);
    std::vector<sf::FloatRect> rects = computeItemRects();
    const std::vector<MenuItem>* items = currentItems();
    if (!items)
        return Request::None;

    for (int i = 0; i < static_cast<int>(rects.size()); ++i)
    {
        if (rects[i].contains(mouse))
        {
            m_selectedIndex = i;

            // Volume slider: direct track click or label/percent nudging
            if ((*items)[i].kind == ItemKind::Slider)
            {
                sf::Vector2f pos = getItemPosition(i);
                const float trackStartX = pos.x - 10.0f;
                const float trackW = 160.0f;

                if (mouse.x < trackStartX)
                {
                    // Clicked left of track (VOLUME label): decrease by 5%
                    m_settings.setVolume(m_settings.getVolume() - 5.0f);
                }
                else if (mouse.x > trackStartX + trackW)
                {
                    // Clicked right of track (% text): increase by 5%
                    m_settings.setVolume(m_settings.getVolume() + 5.0f);
                }
                else
                {
                    // Clicked directly on the slider track: set volume proportionally snapped to 5% increments
                    float ratio = (mouse.x - trackStartX) / trackW;
                    float newVol = std::round(std::clamp(ratio, 0.0f, 1.0f) * 20.0f) * 5.0f;
                    m_settings.setVolume(newVol);
                }
                m_settings.save();
                return Request::None;
            }

            return activateSelected();
        }
    }
    return Request::None;
}

SettingsMenu::Request SettingsMenu::handleInput(const sf::Event& event)
{
    // Mouse support: hover moves the selection, left click activates
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        handleMouseMove(mouseMoved->position);
        return Request::None;
    }
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left)
            return handleMouseClick(mousePressed->position);
        return Request::None;
    }

    const auto* keyEvent = event.getIf<sf::Event::KeyPressed>();
    if (!keyEvent)
        return Request::None;

    // Rebind capture: any key press assigns the action (ESC cancels)
    if (m_screen == Screen::Rebind)
    {
        if (keyEvent->code == sf::Keyboard::Key::Escape)
        {
            m_rebindReserved = false;
            m_screen = Screen::Settings;
            return Request::None;
        }
        // Reject reserved / invalid keys and stay in capture mode
        if (keyEvent->code == sf::Keyboard::Key::Unknown)
        {
            m_rebindReserved = true;
            return Request::None;
        }
        m_rebindReserved = false;
        m_settings.setKey(m_rebindingAction, keyEvent->code);
        m_settings.save();
        m_screen = Screen::Settings;
        return Request::None;
    }

    switch (keyEvent->code)
    {
    case sf::Keyboard::Key::Up:
        moveSelection(0, -1);
        break;
    case sf::Keyboard::Key::Down:
        moveSelection(0, 1);
        break;
    case sf::Keyboard::Key::Left:
        if (m_screen == Screen::Settings && m_selectedIndex == 0)
        {
            m_settings.setVolume(m_settings.getVolume() - 5.0f);
            m_settings.save();
        }
        else
        {
            moveSelection(-1, 0);
        }
        break;
    case sf::Keyboard::Key::Right:
        if (m_screen == Screen::Settings && m_selectedIndex == 0)
        {
            m_settings.setVolume(m_settings.getVolume() + 5.0f);
            m_settings.save();
        }
        else
        {
            moveSelection(1, 0);
        }
        break;
    case sf::Keyboard::Key::Enter:
        return activateSelected();
    case sf::Keyboard::Key::Escape:
        return goBack();
    default:
        break;
    }
    return Request::None;
}

void SettingsMenu::moveSelection(int dx, int dy)
{
    const std::vector<MenuItem>* items = currentItems();
    if (!items || items->empty())
        return;

    int n = static_cast<int>(items->size());

    if (m_screen != Screen::Settings)
    {
        if (dy != 0)
        {
            m_selectedIndex += dy;
            if (m_selectedIndex < 0) m_selectedIndex = n - 1;
            if (m_selectedIndex >= n) m_selectedIndex = 0;
        }
        return;
    }

    // 2D Spatial Navigation for Settings Screen
    if (dy == -1) // Up
    {
        if (m_selectedIndex == 0) m_selectedIndex = 10; // Vol -> Back
        else if (m_selectedIndex >= 1 && m_selectedIndex <= 4) m_selectedIndex = (m_selectedIndex == 1) ? 0 : m_selectedIndex - 1;
        else if (m_selectedIndex >= 5 && m_selectedIndex <= 8) m_selectedIndex = (m_selectedIndex == 5) ? 0 : m_selectedIndex - 1;
        else if (m_selectedIndex == 9) m_selectedIndex = 4; // Reset -> P1 Shoot
        else if (m_selectedIndex == 10) m_selectedIndex = 9; // Back -> Reset
    }
    else if (dy == 1) // Down
    {
        if (m_selectedIndex == 0) m_selectedIndex = 1; // Vol -> P1 Jump
        else if (m_selectedIndex >= 1 && m_selectedIndex <= 4) m_selectedIndex = (m_selectedIndex == 4) ? 9 : m_selectedIndex + 1;
        else if (m_selectedIndex >= 5 && m_selectedIndex <= 8) m_selectedIndex = (m_selectedIndex == 8) ? 9 : m_selectedIndex + 1;
        else if (m_selectedIndex == 9) m_selectedIndex = 10; // Reset -> Back
        else if (m_selectedIndex == 10) m_selectedIndex = 0; // Back -> Vol
    }
    else if (dx == -1 || dx == 1) // Left or Right
    {
        if (m_selectedIndex >= 1 && m_selectedIndex <= 4) m_selectedIndex += 4;
        else if (m_selectedIndex >= 5 && m_selectedIndex <= 8) m_selectedIndex -= 4;
    }
}

SettingsMenu::Request SettingsMenu::activateSelected()
{
    if (m_screen == Screen::Root)
    {
        if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_rootItems.size()))
            return Request::None;

        const MenuItem& item = m_rootItems[m_selectedIndex];
        if (item.label == "SETTINGS")
        {
            openSettings();
            return Request::None;
        }
        return item.request; // Resume or QuitToMenu
    }

    if (m_screen == Screen::Settings)
    {
        if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_settingsItems.size()))
            return Request::None;

        const MenuItem& item = m_settingsItems[m_selectedIndex];
        if (item.kind == ItemKind::Rebind)
        {
            m_rebindingAction = item.action;
            m_screen = Screen::Rebind;
            return Request::None;
        }
        if (item.label == "RESET TO DEFAULTS")
        {
            m_settings.resetToDefault();
            m_settings.save();
            return Request::None;
        }
        if (item.label == "BACK")
        {
            return goBack();
        }
    }
    return Request::None;
}

SettingsMenu::Request SettingsMenu::goBack()
{
    if (m_screen == Screen::Settings)
    {
        if (m_pauseContext)
        {
            openRoot();
            return Request::None;
        }
        return Request::ExitSettings; // main menu context: leave settings
    }

    if (m_screen == Screen::Root)
    {
        return Request::Resume; // ESC in pause root resumes the game
    }
    return Request::None;
}

void SettingsMenu::update(float deltaTime)
{
    (void)deltaTime;
}

void SettingsMenu::render(sf::RenderWindow& window) const
{
    if (!m_fontLoaded)
        return;

    // Keep the stored size in sync so mouse positions can be mapped into
    // the fixed 800x600 view space (see toViewCoords)
    m_windowSize = window.getSize();

    // Title
    std::string title;
    switch (m_screen)
    {
    case Screen::Root:
        title = "PAUSED";
        break;
    case Screen::Settings:
        title = "SETTINGS";
        break;
    case Screen::Rebind:
        title = "PRESS A KEY";
        break;
    }

    sf::Text titleText{m_font};
    setupText(titleText, title, 48, sf::Color::White);
    UIUtils::centerOrigin(titleText);
    titleText.setPosition({ 400.0f, 120.0f });
    window.draw(titleText);

    if (m_screen == Screen::Rebind)
    {
        sf::Text hint{m_font};
        setupText(hint,
                  "Press any key for " + actionLabel(m_rebindingAction) +
                      "   (ESC to cancel)",
                  24, sf::Color::Yellow);
        UIUtils::centerOrigin(hint);
        hint.setPosition({ 400.0f, 300.0f });
        window.draw(hint);

        if (m_rebindReserved)
        {
            sf::Text warn{m_font};
            setupText(warn, "KEY RESERVED - try another", 20, sf::Color::Red);
            UIUtils::centerOrigin(warn);
            warn.setPosition({ 400.0f, 340.0f });
            window.draw(warn);
        }
        return;
    }

    const std::vector<MenuItem>* items = currentItems();
    if (!items)
        return;

    for (int i = 0; i < static_cast<int>(items->size()); ++i)
    {
        const MenuItem& item = (*items)[i];

        if (m_screen == Screen::Settings && item.kind == ItemKind::Slider)
        {
            sf::Vector2f pos = getItemPosition(i);
            
            sf::Text labelText{m_font};
            setupText(labelText, item.label, 28, (i == m_selectedIndex) ? sf::Color::Yellow : sf::Color::White);
            UIUtils::centerOrigin(labelText);
            labelText.setPosition({ pos.x - 100.0f, pos.y });
            window.draw(labelText);
            
            const float trackStartX = pos.x - 10.0f;
            const float trackW = 160.0f;
            const float trackH = 10.0f;
            
            sf::RectangleShape track({ trackW, trackH });
            track.setFillColor(sf::Color(100, 100, 100));
            track.setOutlineColor(sf::Color::White);
            track.setOutlineThickness(1.5f);
            track.setPosition({ trackStartX, pos.y - trackH / 2.0f });
            window.draw(track);
            
            float volRatio = m_settings.getVolume() / 100.0f;
            if (volRatio > 0.0f) {
                sf::RectangleShape fill({ trackW * volRatio, trackH });
                fill.setFillColor((i == m_selectedIndex) ? sf::Color::Yellow : sf::Color::White);
                fill.setPosition({ trackStartX, pos.y - trackH / 2.0f });
                window.draw(fill);
            }
            
            sf::Text pctText{m_font};
            setupText(pctText, std::to_string(static_cast<int>(m_settings.getVolume())) + "%", 28, (i == m_selectedIndex) ? sf::Color::Yellow : sf::Color::White);
            UIUtils::centerOrigin(pctText);
            pctText.setPosition({ trackStartX + trackW + 50.0f, pos.y });
            window.draw(pctText);
        }
        else
        {
            std::string text = itemDisplayText(item);

            sf::Text itemText{m_font};
            setupText(itemText, text, 28,
                      (i == m_selectedIndex) ? sf::Color::Yellow : sf::Color::White);

            UIUtils::centerOrigin(itemText);
            itemText.setPosition(getItemPosition(i));
            window.draw(itemText);
        }
    }
}
