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
#include "drawing_coords.hpp"

/*************************************************************************************************************
 * Given any number of random points, one could draw the resulting shape, by calculating the Discrete Fourier
 * Transform and decomposing the discrete function into the respective frequencies and amplitudes. 
 *************************************************************************************************************/

static const std::vector<sf::Color> colors{sf::Color::Red,    sf::Color::Green,   sf::Color::Blue,
                                           sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan};

/**
 * @brief Epicycle are the circles that stacked together form the independent functions of which our function is
 * composed
 *
 * - amplitude is the radius;
 * - frequency how many times the cycle rotates;
 * - phase is an offset: where the circular pattern begins
 */
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

/**
 * @brief compute the discrete fourier transform of a discrete function given the points
 *
 * the math behind this is to take the signal and decompose it into a bunch of frequency components
 */
std::vector<Epicycle> computeDFT(const std::vector<double>& discreteFunction)
{
    const std::size_t N = discreteFunction.size();
    std::vector<Epicycle> epicyles;
    for (std::size_t k = 0; k < N; ++k) {
        // zk represents a point in the Fourier transform
        std::complex<double> zk{};
        for (std::size_t n = 0; n < N; ++n) {
            const double phi = (2. * M_PI * k * n) / (double)N;
            zk += discreteFunction[n] * std::complex<double>(cos(phi), -sin(phi));
        }
        zk /= (double)N;
        const double freq = k;
        const double amp = sqrt(zk.real() * zk.real() + zk.imag() * zk.imag());
        const double phase = atan2(zk.imag(), zk.real());

        epicyles.emplace_back(freq, amp, phase);
    }

    // should have the same size as discreteFunction
    return epicyles;
}

/**
 * @brief runningPoints is the centre of the next epicycle
 * @returns coords of last runningPoint and coords of the computed point on the path`
 */
std::pair<sf::Vector2<double>, sf::Vector2<double>> drawEpicycles(sf::RenderWindow* window,
                                                                  const std::vector<Epicycle>& fourierSignal,
                                                                  double time, double startX, double startY,
                                                                  double rotation)
{
    std::vector<sf::CircleShape> circles{};
    std::vector<sf::CircleShape> runningPoints{};
    std::vector<std::array<sf::Vertex, 2>> lines{};

    // Compute the polar coordinates
    double x_sum = 0;
    double y_sum = 0;

    sf::CircleShape runningPoint(4);
    for (size_t i = 0; i < fourierSignal.size(); ++i) {
        const double freq = fourierSignal[i].freq_;
        const double radius = fourierSignal[i].ampl_;
        const double phase = fourierSignal[i].phase_;

        sf::CircleShape circle(radius);
        circle.setOutlineColor(colors[i % colors.size()]);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineThickness(1);
        if (circles.size() == 0) {
            circle.setPosition(startX, startY); // if it is first point
        } else {
            circle.setPosition(runningPoint.getPosition().x, runningPoint.getPosition().y);
        }
        circle.setOrigin(circle.getRadius(), circle.getRadius());

        const double x = radius * cos(freq * time + phase + rotation);
        const double y = radius * sin(freq * time + phase + rotation);
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

    // return last runningPoint on the outside of the last epicycle and the calculated path point
    return {{runningPoint.getPosition().x, runningPoint.getPosition().y}, {x_sum, y_sum}};
}

void processEvents(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Event> event,
                   std::atomic<bool>* stopProgram)
{
    const double midX = window->getSize().x / 2.;
    const double midY = window->getSize().y / 2.;
    double time = 0;

    std::vector<double> funcX{};
    std::vector<double> funcY{};
    for (std::size_t i = 0; i < drawingCoords.size(); i += 5) {
        funcX.emplace_back(drawingCoords[i].x);
        funcY.emplace_back(drawingCoords[i].y);
    }

    // fourierTransform = fourier(function)
    std::vector<Epicycle> fourierSignalX = computeDFT(funcX);
    std::vector<Epicycle> fourierSignalY = computeDFT(funcY);

    sort(fourierSignalY.begin(), fourierSignalY.end(),
         [](const auto& epicycle1, const auto& epicycle2) { return epicycle1.ampl_ > epicycle2.ampl_; });
    sort(fourierSignalX.begin(), fourierSignalX.end(),
         [](const Epicycle& epicycle1, const Epicycle& epicycle2) { return epicycle1.ampl_ > epicycle2.ampl_; });

    // instead of a wave, i now need an array which is the full path
    std::deque<sf::Vector2<double>> path{};
    const double dt = M_PI * 2. / (double)fourierSignalY.size();
    while (!stopProgram->load(std::memory_order_relaxed) && window->isOpen()) {
        window->clear(sf::Color::Black);

        // pathPoint represents the point in our function;
        // runningPoint is the last point on the last epicycle
        auto [runningPointX, pathPointX] = drawEpicycles(window.get(), fourierSignalX, time, 200, 500, 0);
        auto [runningPointY, pathPointY] = drawEpicycles(window.get(), fourierSignalY, time, 1000, 200, M_PI_2);

        path.emplace_front(midX + pathPointX.x, midY + 200 + pathPointY.y);

        // Draw the poth after getting a new point after computing the whole set of epicycles
        std::optional<sf::Vector2<double>> lastPoint;
        for (const auto& pathPoint : path) {
            sf::CircleShape circle(0.5);
            circle.setOutlineColor(sf::Color::Blue);
            circle.setFillColor(sf::Color::Transparent);
            circle.setPosition(pathPoint.x, pathPoint.y);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            window->draw(circle);

            if (lastPoint) {
                const sf::Vertex line[] = {sf::Vertex(sf::Vector2f(lastPoint->x, lastPoint->y), sf::Color::Blue),
                                           sf::Vertex(sf::Vector2f(pathPoint.x, pathPoint.y), sf::Color::Blue)};
                window->draw(line, 2, sf::Lines);
            }
            lastPoint.emplace(pathPoint);
        }

        // Line from the x epicycle to the last point in path
        const sf::Vertex line1[] = {sf::Vertex(sf::Vector2f(runningPointX.x, runningPointX.y), sf::Color::White),
                                    sf::Vertex(sf::Vector2f(path.front().x, path.front().y), sf::Color::White)};
        window->draw(line1, 2, sf::Lines);

        // Line from the y epicycle to the last point in path
        const sf::Vertex line2[] = {sf::Vertex(sf::Vector2f(runningPointY.x, runningPointY.y), sf::Color::White),
                                    sf::Vertex(sf::Vector2f(path.front().x, path.front().y), sf::Color::White)};
        window->draw(line2, 2, sf::Lines);

        window->display();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        time += dt;

        if (time > M_PI * 2.) {
            time = 0;
            path.clear();
        }
    }
}

int main()
{
    std::atomic<bool> stopProgram{false};

    constexpr double sizeX = 20000.;
    constexpr double sizeY = 1000.;

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