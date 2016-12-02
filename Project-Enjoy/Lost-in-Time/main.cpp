#include <SFML/Graphics/RenderWindow.hpp>
#include "state/machine.h"
#include "Framework/config.h"

int main()
{
    // Create Machine
    Machine machine;

    // Creating window
    Config config;
    sf::RenderWindow* window = new sf::RenderWindow;
    window->create(sf::VideoMode(config.GetScreenWidth(), config.GetScreenHeight()), "Lost in Time");
    window->setVerticalSyncEnabled(true);
    window->setFramerateLimit(60);
    window->setMouseCursorVisible(false);

    // run until running flag is false
    while(machine.GetRunning())
        machine.GoNext(*window);

    return 0;
}