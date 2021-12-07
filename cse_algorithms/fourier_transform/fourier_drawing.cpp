#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <math.h>
#include <chrono>
#include <atomic>
#include <complex>

#include <SFML/Graphics.hpp>

#include "util.hpp"

/********************************************************************************
 *      Draw sawtooth and/or step function
 *******************************************************************************/

struct Epicycle {
    double freq_{};
    double ampl_{};
    double phase_{};

    Epicycle(double freq, double ampl, double phase) : freq_(freq), ampl_(ampl), phase_(phase) {}

    friend std::ostream& operator<<(std::ostream& os, const Epicycle& epicycle)
    {
        return os << "{ freq: " << epicycle.freq_ << ", amp: " << epicycle.ampl_ << ", phase: " << epicycle.phase_
                  << "} ";
    }
};

std::vector<Epicycle> computeDFT(const std::vector<double>& discreteFunction)
{
    const std::size_t N = discreteFunction.size();
    // std::vector<std::complex<double>> fourierSignal;
    std::vector<Epicycle> epicyles;
    for (std::size_t k = 0; k < N; ++k) {
        std::complex<double> zk{};
        for (std::size_t n = 0; n < N; ++n) {
            const double phi = (2. * M_PI * k * n) / N;
            zk += discreteFunction[n] * std::complex<double>(cos(phi), -sin(phi));
        }
        zk /= N;
        const double freq = k;
        const double amp = sqrt(zk.real() * zk.real() + zk.imag() * zk.imag());
        const double phase = atan2(zk.imag(), zk.real());

        epicyles.emplace_back(freq, amp, phase);
        // fourierSignal.push_back(std::move(zk));
    }

    // for (const std::complex<double>& z : fourierSignal) std::cout << z << " -- ";
    // std::cout << std::endl;
    // for (const Epicycle& epicyle : epicyles) std::cout << epicyle << " -- ";
    // std::cout << std::endl;

    return epicyles;
}

std::pair<sf::Vector2<double>, sf::Vector2<double>> drawEpicycle(sf::RenderWindow* window, const std::vector<Epicycle>& fourierSignal,
                                 std::deque<sf::CircleShape>& wave,
                                 std::deque<std::array<sf::Vertex, 2>>& waveLines, double time, double originX,
                                 double originY, const std::vector<sf::Color>& colors, double rotation)
{
    std::vector<sf::CircleShape> circles{};
    std::vector<sf::CircleShape> runningPoints{};
    std::vector<std::array<sf::Vertex, 2>> lines{};

    // Compute the polar coordinates
    double x_sum = 0;
    double y_sum = 0;

    double x = 0;
    double y = 0;

    sf::CircleShape runningPoint(4);
    // TODO: create a slider to select number of N
    for (size_t i = 0; i < fourierSignal.size(); ++i) {
        const double freq = fourierSignal[i].freq_;
        const double radius = fourierSignal[i].ampl_;
        const double phase = fourierSignal[i].phase_;

        sf::CircleShape circle(radius);
        circle.setOutlineColor(colors[i % colors.size()]);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineThickness(1);
        if (circles.size() == 0) {
            circle.setPosition(originX, originY); // if it is first point
        } else {
            circle.setPosition(runningPoint.getPosition().x, runningPoint.getPosition().y);
        }
        circle.setOrigin(circle.getRadius(), circle.getRadius());

        x = radius * cos(freq * time + phase + rotation);
        y = radius * sin(freq * time + phase + rotation);
        x_sum += x;
        y_sum += y;

        runningPoint.setOutlineColor(colors[i % colors.size()]);
        runningPoint.setFillColor(colors[i % colors.size()]);
        runningPoint.setPosition(circle.getPosition().x + x, circle.getPosition().y + y);
        runningPoint.setOrigin(runningPoint.getRadius(), runningPoint.getRadius());

        const std::array<sf::Vertex, 2> line = {
                    sf::Vertex(sf::Vector2f(circle.getPosition().x, circle.getPosition().y),
                               colors[i % colors.size()]),
                    sf::Vertex(sf::Vector2f(circle.getPosition().x + x, circle.getPosition().y + y),
                               colors[i % colors.size()])};

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

    for (std::array<sf::Vertex, 2>& waveLine : waveLines) {
        waveLine[0].position = sf::Vector2f(waveLine[0].position.x + 1, waveLine[0].position.y);
        waveLine[1].position = sf::Vector2f(waveLine[1].position.x + 1, waveLine[1].position.y);
    }

    // Generate wave point
    sf::CircleShape newWavePoint(0.5);
    newWavePoint.setOutlineColor(sf::Color::Blue);
    newWavePoint.setFillColor(sf::Color::Blue);
    newWavePoint.setOrigin(newWavePoint.getRadius(), newWavePoint.getRadius());
    newWavePoint.setPosition(originX + 250, originY + y_sum);

    if (wave.size() != 0) {
        const std::array<sf::Vertex, 2> newWaveLine = std::array<sf::Vertex, 2>{
                    sf::Vertex(sf::Vector2f(wave.front().getPosition().x, wave.front().getPosition().y),
                               newWavePoint.getFillColor()),
                    sf::Vertex(sf::Vector2f(newWavePoint.getPosition().x, newWavePoint.getPosition().y),
                               newWavePoint.getFillColor())};

        waveLines.emplace_front(newWaveLine);
    }
    wave.emplace_front(newWavePoint);
    if (wave.rbegin()->getPosition().x > window->getSize().x) {
        waveLines.pop_back();
        wave.pop_back();
    }
    return {{runningPoint.getPosition().x, runningPoint.getPosition().y}, {x, y}};
}

void processEvents(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Event> event,
                   std::atomic<bool>* stopProgram)
{
    const double midX = window->getSize().x / 2.;
    const double midY = window->getSize().y / 2.;
    double time = 0;
    std::deque<sf::CircleShape> waveY{};
    std::deque<std::array<sf::Vertex, 2>> waveLinesY{};
    std::deque<sf::CircleShape> waveX{};
    std::deque<std::array<sf::Vertex, 2>> waveLinesX{};

    // TODO: builder pattern to build points
    const std::vector<sf::Color> colors{sf::Color::Red,    sf::Color::Green,   sf::Color::Blue,
                                        sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan};

    // constexpr double mainX = 250;
    // const double mainY = midY;

    // number of points for the discrete function
    std::size_t N = 10;
    // fourierSignal = fourier(signal)

    std::vector<double> funcY{};
    std::vector<double> funcX{};
    for (std::size_t i = 0; i < N; ++i) {
        funcY.emplace_back(150 * cos(i * 2. * M_PI / N));
        funcX.emplace_back(150 * sin(i * 2. * M_PI / N));
        // if(i / 10 % 10 == 0){
        // 	funcY.emplace_back(100);
        // }else{
        // 	funcY.emplace_back(-100);

        // }
    }
    const std::vector<Epicycle> fourierSignalY = computeDFT(funcY);
    const std::vector<Epicycle> fourierSignalX = computeDFT(funcX);

    std::deque<sf::Vector2<double>> path{};

    while (!stopProgram->load(std::memory_order_relaxed) && window->isOpen()) {
        window->clear(sf::Color::Black);

        const double dt = M_PI * 0.01 / N;
        time += dt;

        auto [runningPointY, vy] =
                    drawEpicycle(window.get(), fourierSignalY, waveY, waveLinesY, time, 1000, 200, colors, M_PI_2);
        auto [runningPointX, vx]  =
                    drawEpicycle(window.get(), fourierSignalX, waveX, waveLinesX, time, 200, 500, colors, 0);

        path.emplace_front(1000 + vx.x, 500 + vy.y);

        for (const auto& pathPoint : path) {
            sf::CircleShape circle(0.5);
            circle.setOutlineColor(sf::Color::Blue);
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineThickness(1);
            circle.setPosition(pathPoint.x, pathPoint.y);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
			window->draw(circle);
        }


        const sf::Vertex line1[] = {sf::Vertex(sf::Vector2f(runningPointX.x, runningPointX.y), sf::Color::White),
                                    sf::Vertex(sf::Vector2f(path.front().x, path.front().y), sf::Color::White)};

        window->draw(line1, 2, sf::Lines);

        const sf::Vertex line2[] = {sf::Vertex(sf::Vector2f(runningPointY.x, runningPointY.y), sf::Color::White),
                                    sf::Vertex(sf::Vector2f(path.front().x, path.front().y), sf::Color::White)};

        window->draw(line2, 2, sf::Lines);

        window->display();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main()
{
    std::atomic<bool> stopProgram{false};

    constexpr double sizeX = 1500.;
    constexpr double sizeY = 700.;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    const std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(
                sf::VideoMode(sizeX, sizeY, 64), " Fourier Transform ", sf::Style::Default, settings));
    const std::shared_ptr<sf::Event> event(new sf::Event());

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