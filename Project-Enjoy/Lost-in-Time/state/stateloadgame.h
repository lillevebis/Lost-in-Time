#ifndef LOST_IN_TIME_LOADGAMESTATE_H
#define LOST_IN_TIME_LOADGAMESTATE_H


#include "state.h"

class StateLoadGame : public State
{
public:
    //Functions to load image
    sf::Texture *LoadTexture( std::string path);
    bool loadMedia();

    void LoadGame(int selected);

    void GoNext(Machine &machine, sf::RenderWindow& window);

protected:
    // Flag for keypressed
    bool keyPressed = false;
    // Flag for while loop
    bool inLoad = true;

    // Int values to keep on track of amount of elements in menu
    // And what is selected on the menu
    int selected = 1;
    int menuAmount = 0;

    // Textures and sprites for different images for save files
    sf::Texture* save1 = nullptr;
    sf::Texture* save2 = nullptr;
    sf::Texture* save3 = nullptr;
    sf::Sprite* save1Sprite = nullptr;
    sf::Sprite* save2Sprite = nullptr;
    sf::Sprite* save3Sprite = nullptr;
};


#endif