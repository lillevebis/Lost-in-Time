#ifndef LOST_IN_TIME_TESTAPP_H
#define LOST_IN_TIME_TESTAPP_H


#include "Objects/playertest.h"
#include <list>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics.hpp>
#include "Framework/config.h"
#include "Objects/playertest.h"
#include "Map/map.h"
#include "Framework/Camera.h"
#include "Enemy/AIEnemies.h"
#include "Framework/Music.h"
#include "State/machine.h"

class TestApp
{
public:
    TestApp(sf::RenderWindow& window, SavedObject);
    bool Tick(Machine& machine);
    void Move(float delta);
    void AIHandler(float delta);


    void EscMenu(Machine& machine); // Menu when u press Esc
    bool SaveGame(int selectedSave); // Function under the Esc menu, savegame
    int menuSelected(std::string menu); // Function to choose in menus
    void LoadImages();
    sf::Texture* LoadTexture( std::string path, int &menuAmount); // Function to load in textures
protected:
    int count;
    bool keyPressed = false;
    bool saveMenuUp = false;
    bool menuUp = false;


    /***********************
     * Variables for EscMenu
     **********************/
    // The amount of saves
    int amountOfSaves = 0;
    // The amount of options in EscMenu
    int amountOfEscOptions = 0;

    sf::Texture* save1 = nullptr;
    sf::Texture* save2 = nullptr;
    sf::Texture* save3 = nullptr;

    sf::Texture* selected = nullptr;

    sf::Texture* resumeGame = nullptr;
    sf::Texture* saveGame = nullptr;
    sf::Texture* mainMenu = nullptr;
    sf::Texture* exitGame = nullptr;

    sf::Sprite save1Sprite;
    sf::Sprite save2Sprite;
    sf::Sprite save3Sprite;

    sf::Sprite selectedSprite;

    sf::Sprite resumeGameSprite;
    sf::Sprite saveGameSprite;
    sf::Sprite mainMenuSprite;
    sf::Sprite exitGameSprite;

    /**********************/


    const int ArraySize = 1000;
    int** collidableArray;

    //Dependensies
    PlayerTest* p;

    std::vector<AIEnemies*>* AIVectorPointer = &AIVector;
    std::vector<AIEnemies*> AIVector;

    Config* config;
    std::list<Object*> objects;
    sf::Clock* clock;
    sf::RenderWindow* window;

    // Stuff for timer
    sf::Clock* timer;
    sf::Text* timerInText;
    sf::Font* font;
    int timerX;
    int timerY;
    int penaltyTime = 0;
    int EscMenuTime = 0;

    sf::View* currentView;
    Camera camera;
    Music* music;
};


#endif