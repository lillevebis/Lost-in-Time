#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include "statepause.h"
#include "machine.h"

void StatePause::GoNext(Machine &machine)
{
    std::cout << "Paused" << std::endl;
    while(1)
    {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        {
            machine.SetState(Machine::StateId::MAINMENU);
            break;
        }
    }

}