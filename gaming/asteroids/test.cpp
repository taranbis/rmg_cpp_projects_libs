#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::RectangleShape rect({10,20});
    rect.setFillColor(sf::Color::Blue);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::C) {
                window.clear();
                window.draw(shape);
                window.display();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::A) {
                window.clear();
                window.display();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::R) {
                window.clear();
                window.draw(rect);
                window.display();
            }
        }
    }

    return 0;
}