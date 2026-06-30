#include <SFML/Graphics.hpp>
#include <optional>
#include "src/states/PlayState.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "PlayState Test");
    window.setFramerateLimit(60);

    PlayState playState;
    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            playState.handleInput(*event);
        }

        playState.update(deltaTime);

        window.clear(sf::Color::Black);
        playState.render(window);
        
        window.display();
    }

    return 0;
}
