#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>

#include <SFML/Graphics.hpp>

#include "util.hpp"

void drawVectorToScreen(const std::vector<int> &vec, sf::RenderWindow &window)
{
    window.clear();
    size_t idx = 0;
    for (const int &val : vec) {
        sf::RectangleShape rect({5, (float)(val * 10)});
        rect.setPosition((float)(idx * 10), window.getSize().y - (float)(val * 10));
        rect.setFillColor(sf::Color::Green);
        window.draw(rect);
        idx++;
    }
    window.display();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void bubbleSort(std::vector<int> &vec, sf::RenderWindow &window)
{
    bool finish = true;
    do {
        finish = true;
        for (size_t i = 0; i < vec.size() - 1; i++) {
            if (vec[i] > vec[i + 1]) {
                std::swap(vec[i], vec[i + 1]);
                finish = false;
            }

            drawVectorToScreen(vec, window);
        }
    } while (!finish);

	std::cout << "Done!" << std::endl;
}

int main()
{
    std::vector<int> vec = {49, 45, 23, 36, 14, 28, 41, 6, 23, 6, 1, 9 , 15, 24, 11, 32, 3, 31, 21, 13, 4};

    int max = *std::max_element(vec.begin(), vec.end());

    unsigned int sizeX = vec.size() * 10;
    unsigned int sizeY = max * 10;

    sf::RenderWindow window(sf::VideoMode(sizeX, sizeY, 64), " Bubble Sort");

    drawVectorToScreen(vec, window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::R) {
                bubbleSort(vec, window);
            }

            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) {
                window.close();
            }
        }
    }

    rmg::PrintVector(vec);
    return 0;
}
