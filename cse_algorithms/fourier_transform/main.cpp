#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <math.h>
#include <chrono>
#include <atomic>

#include <SFML/Graphics.hpp>

#include "util.hpp"

void calculateFFT()
{
    // DEB_SHORT(elapsedTime);
    // DEB_SHORT(time);
    // NEWLINE();
}

void processEvents(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Event> event,
                   std::atomic<bool>* stopProgram)
{
    float midX = window->getSize().x / 2.f;
    float midY = window->getSize().y / 2.f;
    sf::Clock clock{};
    double time = 0;
    std::deque<sf::CircleShape> wave{};

    // TODO: builder pattern to build points

    // Number of waves (circles) stacked (added)
    std::size_t N = 4;

    std::vector<sf::Color> colors{sf::Color::Red,    sf::Color::Green,   sf::Color::Blue,
                                  sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan};

    std::vector<sf::CircleShape> circles{};
    std::vector<sf::CircleShape> runningPoints{};
    std::vector<std::array<sf::Vertex, 2>> lines{};

    constexpr std::size_t baseRadius = 100;
    float mainX = 250;
    float mainY = midY;

    while (!stopProgram->load(std::memory_order_relaxed) && window->isOpen()) {
        double elapsedTime = clock.getElapsedTime().asSeconds();
        time += elapsedTime * 2;
        clock.restart();

        window->clear(sf::Color::Black);

        // Compute the polar coordinates
        double x_sum = 0;
        double y_sum = 0;

        sf::CircleShape runningPoint(4);
        for (size_t i = 0; i < N; ++i) {
            std::size_t n = i * 2 + 1;
            std::size_t radius = baseRadius * (4 / (n * M_PI));

            sf::CircleShape circle(radius);
            circle.setOutlineColor(colors[i]);
            circle.setFillColor(sf::Color::Black);
            circle.setOutlineThickness(1);
            // mainCircle.setOrigin(midX, midY);
            // mainCircle.setOrigin(midX, midY);
            //! I still don't understand. this makes more sense but for sf::ConvexShape in asteroids_game::main.cpp
            //! seemed to be other way; we make origin the centre and then move to the middle so that every op is
            //! in respect to mainCircle's centre
            if (circles.size() == 0) {
                // if it is first point
                circle.setPosition(mainX, mainY);
            } else {
                circle.setPosition(runningPoint.getPosition().x, runningPoint.getPosition().y);
            }
            circle.setOrigin(circle.getRadius(), circle.getRadius());

            x_sum += radius * cos(n * time);
            y_sum += radius * sin(n * time);

            double x = radius * cos(n * time);
            double y = radius * sin(n * time);

            runningPoint.setOutlineColor(colors[i]);
            runningPoint.setFillColor(colors[i]);
            runningPoint.setPosition(circle.getPosition().x + x, circle.getPosition().y + y);
            runningPoint.setOrigin(runningPoint.getRadius(), runningPoint.getRadius());
            // runningPoint.setOrigin(runningPoint.getRadius(), runningPoint.getRadius());

            std::array<sf::Vertex, 2> line = {
                        sf::Vertex(sf::Vector2f(circle.getPosition().x, circle.getPosition().y), colors[i]),
                        sf::Vertex(sf::Vector2f(circle.getPosition().x + x, circle.getPosition().y + y),
                                   colors[i])};

            circles.push_back(std::move(circle));
            runningPoints.push_back(runningPoint);
            lines.push_back(std::move(line));
        }

        for (const auto& circle : circles) window->draw(circle);
        circles.clear();
        for (const auto& point : runningPoints) window->draw(point);
        runningPoints.clear();
        for (const auto& line : lines) window->draw(line.data(), 2, sf::Lines);
        lines.clear();

        // TODO: make lines between wave points
        for (sf::CircleShape& wavePoint : wave) {
            wavePoint.setPosition(wavePoint.getPosition().x + 1, wavePoint.getPosition().y);
        }

        // Generate wave point
        sf::CircleShape newWavePoint(2.5);
        newWavePoint.setOutlineColor(sf::Color::Blue);
        newWavePoint.setFillColor(sf::Color::Blue);
        newWavePoint.setOrigin(newWavePoint.getRadius(), newWavePoint.getRadius());
        newWavePoint.setPosition(mainX + 250, mainY + y_sum);
        wave.emplace_front(newWavePoint);
        if (wave.rbegin()->getPosition().x > window->getSize().x) wave.pop_back();

        sf::Vertex connectLine[] = {
                    sf::Vertex(sf::Vector2f(runningPoint.getPosition().x, runningPoint.getPosition().y),
                               sf::Color::White),
                    sf::Vertex(sf::Vector2f(newWavePoint.getPosition().x, newWavePoint.getPosition().y),
                               sf::Color::White)};

        window->draw(connectLine, 2, sf::Lines);

        // DEB_SHORT(wave.size());
        // NEWLINE();

        for (const sf::CircleShape& wavePoint : wave) window->draw(wavePoint);

        window->display();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main()
{
    std::atomic<bool> stopProgram{false};

    constexpr float sizeX = 1500.f;
    constexpr float sizeY = 700.f;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(
                sf::VideoMode(sizeX, sizeY, 64), " Fourier Transform ", sf::Style::Default, settings));
    std::shared_ptr<sf::Event> event(new sf::Event());

    std::thread th(processEvents, window, event, &stopProgram);

    /**
     * @brief polling of events should be done in the main thread. However, remember to deactivate the
     * window before you use it in another thread. If you close the window in a thread and then poll from
     * another it result in segmentation fault as the underlying GL resources are released by close();
     */
    while (!stopProgram.load(std::memory_order_relaxed) && window->isOpen()) {
        window->pollEvent(*event);

        if (event->type == sf::Event::Closed) {
            stopProgram.store(true, std::memory_order_relaxed);
            window->close();
        }
        if (event->type == sf::Event::KeyPressed && event->key.code == sf::Keyboard::Escape) {
            stopProgram.store(true, std::memory_order_relaxed);
            window->close();
        }
    }
    stopProgram.store(true, std::memory_order_relaxed);

    th.join();

    return 0;
}