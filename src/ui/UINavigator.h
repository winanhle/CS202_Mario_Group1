#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

/**
 * @class UINavigator
 * @brief Decouples menu navigation and hit-testing from rendering.
 */
class UINavigator {
public:
    enum class Axis { Vertical, Horizontal };

    UINavigator(int itemCount);

    // Configuration Callbacks
    std::function<sf::FloatRect(int index)> getHitbox;
    std::function<void(int index)> onActivate;
    std::function<void(int oldIndex, int newIndex)> onSelectionChanged;

    void setAxis(Axis axis) { m_axis = axis; }

    // Call this inside the state's handleInput
    void handleInput(const sf::Event& event, const sf::Vector2u& windowSize);

    int getSelectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int index);

private:
    int m_itemCount;
    int m_selectedIndex = 0;
    Axis m_axis = Axis::Vertical;

    sf::Vector2f toViewCoords(sf::Vector2i pixelPos, const sf::Vector2u& windowSize) const;
};
