#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <math.h>
#include <chrono>
#include <atomic>

#include <SFML/Graphics.hpp>

#include "util.h"

class Ship
{
public:
    Ship(sf::RenderWindow *window) : window_(window)
    {
        float midX = window->getSize().x / 2.f;
        float midY = window->getSize().y / 2.f;
        window->clear();

        float base = 30;

        float xA = midX + 20;
        float yA = midY;

        float xB = midX - 15;
        float yB = midY - base / 2;

        float xC = midX - 15;
        float yC = midY + base / 2;

        ship_.setPointCount(3);
        ship_.setPoint(0, sf::Vector2f(xA, yA));
        ship_.setPoint(1, sf::Vector2f(xB, yB));
        ship_.setPoint(2, sf::Vector2f(xC, yC));
        ship_.setOutlineColor(sf::Color::Cyan);
        ship_.setFillColor(sf::Color::Cyan);
        ship_.setOutlineThickness(2);
        /**
         * It's the expected behaviour. The origin is the center point of all transformations, including the
         * translation (position). And since the position is still (0, 0), the consequence is that the whole shape
         * moves by an offset of (-mdX, -midY bla bla), and goes back to the left-most corner of the screen.
         */
        setPosition(midX, midY);
        ship_.setOrigin(midX, midY);
        window->draw(ship_);
        window->display();
    }

    sf::ConvexShape &getShapeObject()
    {
        return ship_;
    }

    void setPosition(sf::Vector2f currPos)
    {
        checkAndWrapLocation();
        ship_.setPosition(currPos);
    }

    void setPosition(float x, float y)
    {
        checkAndWrapLocation();
        ship_.setPosition(x, y);
    }

    void checkAndWrapLocation()
    {
        if (ship_.getPosition().x < 0) {
            ship_.setPosition(window_->getSize().x, ship_.getPosition().y);
        } else if (ship_.getPosition().x > window_->getSize().x) {
            ship_.setPosition(0, ship_.getPosition().y);
        }

        if (ship_.getPosition().y < 0) {
            ship_.setPosition(ship_.getPosition().y, window_->getSize().y);
        } else if (ship_.getPosition().y > window_->getSize().y) {
            ship_.setPosition(ship_.getPosition().x, 0);
        }
    }

private:
    sf::ConvexShape ship_;
    sf::RenderWindow *window_;

public:
    struct Speed {
        float vX;
        float vY;
    } speed;

    float acc = 250.f;
};

void computeAndSetNewPosition(Ship &ship, float dt)
{
    // take the same time; correct time would be 0.020226s (time between frames)
    // float dt = 0.08;
    // ship.oldTimePoint = presentTimePoint;s
    // DEB(dt);
    double angle = (360 - ship.getShapeObject().getRotation()) * M_PI / 180;
    float aX = ship.acc * cos(angle);
    float aY = ship.acc * sin(angle);

    ship.speed.vX += aX * dt;
    ship.speed.vY += aY * dt;
    // DEB(angle);
    // DEB(cos(angle));
    // DEB(sin(angle));
    sf::Vector2f currPos = ship.getShapeObject().getPosition();
    currPos.x += dt * ship.speed.vX + aX * dt * dt / 2;
    currPos.y -= dt * ship.speed.vY + aY * dt * dt / 2; // has to be minus as y axis is inverted
    // DEB(currPos.x + ship.speed.vX * dt / 10e6);
    // DEB(currPos.y + ship.speed.vY * dt / 10e6);

    ship.setPosition(currPos);
}

void processEvents(sf::RenderWindow *window, std::shared_ptr<sf::Event> event, std::atomic_bool *stopProgram)
{
    Ship ship(window);

    enum class SpeedState { Accelerate, None, Braking };
    SpeedState speedState = SpeedState::None;

    enum class RotationState { RotatingLeft, None, RotatingRight };
    RotationState rotationState = RotationState::None;

    sf::Clock Clock;
    while (!stopProgram->load(std::memory_order_relaxed) && window->isOpen()) {
        float ElapsedTime = Clock.getElapsedTime().asSeconds();
        Clock.restart();
        switch (event->type) {
        case sf::Event::KeyPressed:
            if (event->key.code == sf::Keyboard::Right)
                rotationState = RotationState::RotatingRight;
            else if (event->key.code == sf::Keyboard::Down)
                speedState = SpeedState::Braking;
            else if (event->key.code == sf::Keyboard::Up) {
                speedState = SpeedState::Accelerate;
            } else if (event->key.code == sf::Keyboard::Left)
                rotationState = RotationState::RotatingLeft;
            break;
        case sf::Event::KeyReleased:
            if (event->key.code == sf::Keyboard::Right)
                rotationState = RotationState::None;
            else if (event->key.code == sf::Keyboard::Down)
                speedState = SpeedState::None;
            else if (event->key.code == sf::Keyboard::Up)
                speedState = SpeedState::None;
            else if (event->key.code == sf::Keyboard::Left)
                rotationState = RotationState::None;
            break;
        default: break;
        }

        if (speedState == SpeedState::Accelerate) {
            // TODO: build a separate function; three times copied
            ship.acc = 250;
            if (rotationState == RotationState::RotatingRight) {
                ship.getShapeObject().rotate(10);
            } else if (rotationState == RotationState::RotatingLeft) {
                ship.getShapeObject().rotate(-10);
            }
            computeAndSetNewPosition(ship, ElapsedTime);
        } else if (speedState == SpeedState::Braking) {
            // TODO: build a separate function; three times copied
            ship.acc = -250;
            if (rotationState == RotationState::RotatingRight) {
                ship.getShapeObject().rotate(10);
            } else if (rotationState == RotationState::RotatingLeft) {
                ship.getShapeObject().rotate(-10);
            }
            computeAndSetNewPosition(ship, ElapsedTime);
        } else {
            // TODO: build a separate function; three times copied
            ship.acc = -0;
            if (rotationState == RotationState::RotatingRight) {
                ship.getShapeObject().rotate(10);
            } else if (rotationState == RotationState::RotatingLeft) {
                ship.getShapeObject().rotate(-10);
            }
            computeAndSetNewPosition(ship, ElapsedTime);
        }
        window->clear();
        window->draw(ship.getShapeObject());
        window->display();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

int main()
{
    std::atomic<bool> stopProgram{false};

    constexpr float sizeX = 1000.f;
    constexpr float sizeY = 700.f;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    std::unique_ptr<sf::RenderWindow> window(new sf::RenderWindow(
                sf::VideoMode(sizeX, sizeY, 64), " Asteroids Game ", sf::Style::Default, settings));
    std::shared_ptr<sf::Event> event(new sf::Event());

    std::thread th(processEvents, window.get(), event, &stopProgram);

    /**
     * @brief polling of events should be done in the main thread. However, remember to deactivate the window
     * before you use it in another thread. If you close the window in a thread and then poll from another it
     * result in segmentation fault as the underlying GL resources are released by close();
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