#include "SettingsMenu.h"
#include <SFML/Window/Event.hpp>

SettingsMenu::SettingsMenu(ISettingsManager& settings, bool pauseContext)
    : m_settings(settings)
    , m_pauseContext(pauseContext)
    , m_screen(pauseContext ? Screen::Root : Screen::Settings)
    , m_selectedIndex(0)
    , m_rebindingAction(GameAction::Jump)
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
        m_rootItems.push_back({ "RESUME", ItemKind::Activate, GameAction::Jump, Request::Resume });
        m_rootItems.push_back({ "SETTINGS", ItemKind::Activate, GameAction::Jump, Request::None });
        m_rootItems.push_back({ "SAVE & QUIT", ItemKind::Activate, GameAction::Jump, Request::SaveAndQuit });
        m_rootItems.push_back({ "QUIT TO MENU", ItemKind::Activate, GameAction::Jump, Request::QuitToMenu });
    }

    m_settingsItems.push_back({ "VOLUME", ItemKind::Slider, GameAction::Jump, Request::None });
    m_settingsItems.push_back({ "JUMP", ItemKind::Rebind, GameAction::Jump, Request::None });
    m_settingsItems.push_back({ "MOVE LEFT", ItemKind::Rebind, GameAction::MoveLeft, Request::None });
    m_settingsItems.push_back({ "MOVE RIGHT", ItemKind::Rebind, GameAction::MoveRight, Request::None });
    m_settingsItems.push_back({ "BACK", ItemKind::Activate, GameAction::Jump, Request::None });
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
    case GameAction::Jump: return "JUMP";
    case GameAction::MoveLeft: return "MOVE LEFT";
    case GameAction::MoveRight: return "MOVE RIGHT";
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

std::vector<sf::FloatRect> SettingsMenu::computeItemRects() const
{
    std::vector<sf::FloatRect> rects;
    const std::vector<MenuItem>* items = currentItems();
    if (!items)
        return rects;

    for (int i = 0; i < static_cast<int>(items->size()); ++i)
    {
        sf::Text itemText{m_font};
        setupText(itemText, itemDisplayText((*items)[i]), 28, sf::Color::White);

        sf::FloatRect bounds = itemText.getLocalBounds();
        itemText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                             bounds.position.y + bounds.size.y / 2.0f });
        itemText.setPosition({ ITEM_CENTER_X, ITEM_START_Y + i * ITEM_SPACING });

        sf::Vector2f topLeft = itemText.getPosition() - itemText.getOrigin();
        rects.push_back(sf::FloatRect(topLeft, bounds.size));
    }
    return rects;
}

void SettingsMenu::handleMouseMove(sf::Vector2i position)
{
    if (m_screen == Screen::Rebind)
        return;

    sf::Vector2f mouse(static_cast<float>(position.x), static_cast<float>(position.y));
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

    sf::Vector2f mouse(static_cast<float>(position.x), static_cast<float>(position.y));
    std::vector<sf::FloatRect> rects = computeItemRects();
    const std::vector<MenuItem>* items = currentItems();
    if (!items)
        return Request::None;

    for (int i = 0; i < static_cast<int>(rects.size()); ++i)
    {
        if (rects[i].contains(mouse))
        {
            m_selectedIndex = i;

            // Volume slider: click left half to decrease, right half to increase
            if ((*items)[i].kind == ItemKind::Slider)
            {
                float midX = rects[i].position.x + rects[i].size.x / 2.0f;
                if (mouse.x < midX)
                    m_settings.setVolume(m_settings.getVolume() - 5.0f);
                else
                    m_settings.setVolume(m_settings.getVolume() + 5.0f);
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
            m_screen = Screen::Settings;
            return Request::None;
        }
        m_settings.setKey(m_rebindingAction, keyEvent->code);
        m_settings.save();
        m_screen = Screen::Settings;
        return Request::None;
    }

    switch (keyEvent->code)
    {
    case sf::Keyboard::Key::Up:
        moveSelection(-1);
        break;
    case sf::Keyboard::Key::Down:
        moveSelection(1);
        break;
    case sf::Keyboard::Key::Left:
        if (m_screen == Screen::Settings && m_selectedIndex == 0)
        {
            m_settings.setVolume(m_settings.getVolume() - 5.0f);
            m_settings.save();
        }
        break;
    case sf::Keyboard::Key::Right:
        if (m_screen == Screen::Settings && m_selectedIndex == 0)
        {
            m_settings.setVolume(m_settings.getVolume() + 5.0f);
            m_settings.save();
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

void SettingsMenu::moveSelection(int direction)
{
    const std::vector<MenuItem>* items = currentItems();
    if (!items || items->empty())
        return;

    m_selectedIndex += direction;
    if (m_selectedIndex < 0)
        m_selectedIndex = static_cast<int>(items->size()) - 1;
    if (m_selectedIndex >= static_cast<int>(items->size()))
        m_selectedIndex = 0;
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

    setupText(m_title, title, 48, sf::Color::White);
    sf::FloatRect titleBounds = m_title.getLocalBounds();
    m_title.setOrigin({ titleBounds.position.x + titleBounds.size.x / 2.0f,
                        titleBounds.position.y + titleBounds.size.y / 2.0f });
    m_title.setPosition({ 400.0f, 120.0f });
    window.draw(m_title);

    if (m_screen == Screen::Rebind)
    {
        sf::Text hint{m_font};
        setupText(hint,
                  "Press any key for " + actionLabel(m_rebindingAction) +
                      "   (ESC to cancel)",
                  24, sf::Color::Yellow);
        sf::FloatRect hintBounds = hint.getLocalBounds();
        hint.setOrigin({ hintBounds.position.x + hintBounds.size.x / 2.0f,
                         hintBounds.position.y + hintBounds.size.y / 2.0f });
        hint.setPosition({ 400.0f, 300.0f });
        window.draw(hint);
        return;
    }

    const std::vector<MenuItem>* items = currentItems();
    if (!items)
        return;

    for (int i = 0; i < static_cast<int>(items->size()); ++i)
    {
        const MenuItem& item = (*items)[i];
        std::string text = itemDisplayText(item);

        sf::Text itemText{m_font};
        setupText(itemText, text, 28,
                  (i == m_selectedIndex) ? sf::Color::Yellow : sf::Color::White);

        sf::FloatRect bounds = itemText.getLocalBounds();
        itemText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                             bounds.position.y + bounds.size.y / 2.0f });
        itemText.setPosition({ ITEM_CENTER_X, ITEM_START_Y + i * ITEM_SPACING });
        window.draw(itemText);
    }
}
