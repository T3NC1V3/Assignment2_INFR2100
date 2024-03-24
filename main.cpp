// Vincent Yang - 100895411

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>

const int windowWidth = 800;
const int windowHeight = 600;

// Paddle class
class Paddle {
public:
    Paddle(float x, float y) : x(x), y(y) {}

    void moveUp() {
        if (y > 0)
            y -= paddleSpeed;
    }

    void moveDown() {
        if (y + height < windowHeight)
            y += paddleSpeed;
    }

    float getX() const { return x; }

    float getY() const { return y; }

    float getWidth() const { return width; }

    float getHeight() const { return height; }

protected:
    float x, y;
    const float width = 10.0f;
    const float height = 70.0f;
    const float paddleSpeed = 2.0f;
};

// Player Paddle class
class PlayerPaddle : public Paddle {
public:
    PlayerPaddle(float x, float y) : Paddle(x, y) {}

    void update() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            moveUp();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            moveDown();
    }
};

// CPU Paddle class
class CPUPaddle : public Paddle {
public:
    CPUPaddle(float x, float y) : Paddle(x, y) {}

    void update(float ballY) {
        if (ballY > y + height / 2)
            moveDown();
        else if (ballY < y + height / 2)
            moveUp();
    }
};

// Ball class
class Ball {
public:
    Ball(float x, float y, float velX, float velY) : x(x), y(y), velX(velX), velY(velY) {}

    void update() {
        x += velX;
        y += velY;

        if (y <= 0 || y >= windowHeight)
            velY = -velY;
    }

    void reset() {
        x = windowWidth / 2;
        y = windowHeight / 2;
        velX = (rand() % 2 == 0 ? 1 : -1) * ballSpeed;
        velY = (rand() % 2 == 0 ? 1 : -1) * ballSpeed;
    }

    float getX() const { return x; }

    float getY() const { return y; }

    float getRadius() const { return radius; }

    float getVelX() const { return velX; }

    void reflect() { velX = -velX; }

private:
    float x, y;
    float velX, velY;
    const float radius = 5.0f;
    const float ballSpeed = 2.0f;
};

void countdown(sf::RenderWindow& window) {
    for (int i = 3; i > 0; --i) {
        window.clear();
        sf::Font font;
        font.loadFromFile("arial.ttf");
        sf::Text text(std::to_string(i), font);
        text.setCharacterSize(100);
        text.setFillColor(sf::Color::White);
        text.setPosition(windowWidth / 2 - 20, windowHeight / 2 - 50);
        window.draw(text);
        window.display();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    window.clear();
    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text text("GO!", font);
    text.setCharacterSize(100);
    text.setFillColor(sf::Color::White);
    text.setPosition(windowWidth / 2 - 40, windowHeight / 2 - 50);
    window.draw(text);
    window.display();
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main() {

    bool start = false;
    int button;
    std::cout << "\n1 - Start game\n";
    std::cout << "2 - View High score\n";
    std::cout << "3 - Quit game\n";
    std::cin >> button;
    switch (button) {
    case 1:
        start = true;
        break;
    case 2:
        std::cout << "\nNot implemented.";
        return 0;
        break;
    case 3:
        std::cout << "\nSee ya.";
        return 0;
        break;
    default:
        std::cout << "\nInvalid Input.";
        return 0;
    }

    if (start == true) {
        sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Pong");

        PlayerPaddle playerPaddle(10, windowHeight / 2 - 15);
        CPUPaddle cpuPaddle(windowWidth - 20, windowHeight / 2 - 15);
        Ball ball(windowWidth / 2, windowHeight / 2, 1, 1);

        int playerScore = 0;

        countdown(window);

        sf::Clock clock;

        while (window.isOpen()) {
            sf::Time elapsed = clock.restart();

            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            playerPaddle.update();
            cpuPaddle.update(ball.getY());
            ball.update();

            // Ball collision with paddles
            if (ball.getX() + ball.getRadius() >= playerPaddle.getX() && ball.getX() - ball.getRadius() <= playerPaddle.getX() + playerPaddle.getWidth()) {
                if (ball.getY() + ball.getRadius() >= playerPaddle.getY() && ball.getY() - ball.getRadius() <= playerPaddle.getY() + playerPaddle.getHeight()) {
                    ball.reflect();
                }
            }

            if (ball.getX() + ball.getRadius() >= cpuPaddle.getX() && ball.getX() - ball.getRadius() <= cpuPaddle.getX() + cpuPaddle.getWidth()) {
                if (ball.getY() + ball.getRadius() >= cpuPaddle.getY() && ball.getY() - ball.getRadius() <= cpuPaddle.getY() + cpuPaddle.getHeight()) {
                    ball.reflect();
                }
            }

            // Ball collision with window edges
            if (ball.getX() - ball.getRadius() <= 0) {
                std::cout << "Game Over! Final Score: " << playerScore << std::endl;
                break;
            }

            if (ball.getX() + ball.getRadius() >= windowWidth) {
                playerScore++;
                ball.reset();
            }

            window.clear();

            // Draw paddles
            sf::RectangleShape playerRect(sf::Vector2f(playerPaddle.getWidth(), playerPaddle.getHeight()));
            playerRect.setPosition(playerPaddle.getX(), playerPaddle.getY());
            playerRect.setFillColor(sf::Color::White);
            window.draw(playerRect);

            sf::RectangleShape cpuRect(sf::Vector2f(cpuPaddle.getWidth(), cpuPaddle.getHeight()));
            cpuRect.setPosition(cpuPaddle.getX(), cpuPaddle.getY());
            cpuRect.setFillColor(sf::Color::White);
            window.draw(cpuRect);

            // Draw ball
            sf::CircleShape ballShape(ball.getRadius());
            ballShape.setPosition(ball.getX() - ball.getRadius(), ball.getY() - ball.getRadius());
            ballShape.setFillColor(sf::Color::White);
            window.draw(ballShape);

            window.display();

            // Limit game speed(it was too fast)
            sf::Time sleepTime = sf::milliseconds(10) - elapsed;
            if (sleepTime.asMilliseconds() > 0)
            {
                sf::sleep(sleepTime);
            }
        }
    }
}
