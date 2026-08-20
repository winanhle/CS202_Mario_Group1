#include "UINavigator.h"

UINavigator::UINavigator(int itemCount)
    : m_itemCount(itemCount)
    , m_selectedIndex(0)
    , m_axis(Axis::Vertical)
{
}

void UINavigator::setSelectedIndex(int index)
{
    if (m_selectedIndex != index)
    {
        int old = m_selectedIndex;
        m_selectedIndex = index;
        if (onSelectionChanged)
        {
            onSelectionChanged(old, m_selectedIndex);
        }
    }
}

sf::Vector2f UINavigator::toViewCoords(sf::Vector2i pixelPos, const sf::Vector2u& windowSize) const
{
    // The menu layout lives in the fixed 800x600 view space. 
    // Scale window-pixel mouse positions back into view space.
    if (windowSize.x == 0 || windowSize.y == 0) return {0.f, 0.f};

    const float scaleX = 800.0f / static_cast<float>(windowSize.x);
    const float scaleY = 600.0f / static_cast<float>(windowSize.y);
    return { static_cast<float>(pixelPos.x) * scaleX,
             static_cast<float>(pixelPos.y) * scaleY };
}

void UINavigator::handleInput(const sf::Event& event, const sf::Vector2u& windowSize)
{
    if (m_itemCount <= 0) return;

    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        if (getHitbox)
        {
            sf::Vector2f mouse = toViewCoords(mouseMoved->position, windowSize);
            for (int i = 0; i < m_itemCount; ++i)
            {
                if (getHitbox(i).contains(mouse))
                {
                    setSelectedIndex(i);
                    break;
                }
            }
        }
    }
    else if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left && getHitbox)
        {
            sf::Vector2f mouse = toViewCoords(mousePressed->position, windowSize);
            for (int i = 0; i < m_itemCount; ++i)
            {
                if (getHitbox(i).contains(mouse))
                {
                    setSelectedIndex(i);
                    if (onActivate) onActivate(i);
                    break;
                }
            }
        }
    }
    else if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (m_axis == Axis::Vertical)
        {
            if (keyEvent->code == sf::Keyboard::Key::Up || keyEvent->code == sf::Keyboard::Key::W)
            {
                setSelectedIndex((m_selectedIndex - 1 + m_itemCount) % m_itemCount);
            }
            else if (keyEvent->code == sf::Keyboard::Key::Down || keyEvent->code == sf::Keyboard::Key::S)
            {
                setSelectedIndex((m_selectedIndex + 1) % m_itemCount);
            }
        }
        else // Horizontal
        {
            if (keyEvent->code == sf::Keyboard::Key::Left || keyEvent->code == sf::Keyboard::Key::A)
            {
                setSelectedIndex((m_selectedIndex - 1 + m_itemCount) % m_itemCount);
            }
            else if (keyEvent->code == sf::Keyboard::Key::Right || keyEvent->code == sf::Keyboard::Key::D)
            {
                setSelectedIndex((m_selectedIndex + 1) % m_itemCount);
            }
        }

        if (keyEvent->code == sf::Keyboard::Key::Enter || keyEvent->code == sf::Keyboard::Key::Space)
        {
            if (onActivate) onActivate(m_selectedIndex);
        }
    }
}
