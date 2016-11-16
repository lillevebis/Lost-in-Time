#ifndef LOST_IN_TIME_HEALTH_H
#define LOST_IN_TIME_HEALTH_H


#include <SFML/Graphics.hpp>
#include "../Framework/config.h"

class Health {
public:
    Health() : config(config) { };
    void init();

    float GetActualLifePoints();
    void SetActualLifePoints(int value);
    float GetVisibleLifePoints();
    void SetVisibleLifePoints(int value);
    float GetOriginalLifePoints();
    void SetOriginalLifePoints(int value);

    void ResetHealth();
    void Hit(int damage);
    void DrawMe();

    float GetWidth() { return width; }
    void SetWidth(float width) { this->width = width; }
    float GetHeight() { return height; }
    void SetHeight(float height) { this->height = height; }

    sf::Texture GetgreenHealth() { return greenHealth; }
    void SetgreenHealth(sf::Texture texture) { this->greenHealth = texture; }
    sf::Texture GetredHealth() { return redHealth; }
    void SetredHealth(sf::Texture texture) { this->redHealth = texture; }
    sf::Texture GetHealthBorder() { return HealthBorder; }
    void SetHealthBorder(sf::Texture texture) { this->HealthBorder = texture; }

    sf::RectangleShape GetgreenHealthbar() { return greenHealthbar; }
    void SetgreenHealthbar(sf::RectangleShape healthbar) { this->greenHealthbar = healthbar; }
    sf::RectangleShape GetredHealthbar() { return redHealthbar; }
    void SetredHealthbar(sf::RectangleShape healthbar) { this->redHealthbar = healthbar; }
    sf::RectangleShape GetHealthBorderbar() { return HealthbarBorder; }
    void SetHealthBorderbar(sf::RectangleShape healthbar) { this->HealthbarBorder = healthbar; }

    sf::IntRect GetgreenHealthbarFrame() { return *greenHealthbarFrame; }
    void SetgreenHealthbar(sf::IntRect *frame) { this->greenHealthbarFrame = frame; }
    sf::IntRect GetredHealthbarFrame() { return *redHealthbarFrame; }
    void SetredHealthbar(sf::IntRect *frame) { this->redHealthbarFrame = frame; }
    sf::IntRect GetHealthBorderbarFrame() { return *HealthbarBorderFrame; }
    void SetHealthBorderbar(sf::IntRect *frame) { this->HealthbarBorderFrame = frame; }

protected:
    float actuallifepoints;
    float visiblelifepoints;
    float originalhealth;

    sf::Texture greenHealth;
    sf::RectangleShape greenHealthbar;
    sf::IntRect *greenHealthbarFrame;

    sf::Texture redHealth;
    sf::RectangleShape redHealthbar;
    sf::IntRect *redHealthbarFrame;

    sf::Texture HealthBorder;
    sf::RectangleShape HealthbarBorder;
    sf::IntRect *HealthbarBorderFrame;

    float width = 100;
    float height = 20;

    const Config& config;
    sf::RenderWindow window;
};


#endif
