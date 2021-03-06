#include <iostream>
#include <fstream>
#include "stateloadgame.h"
#include "machine.h"


sf::Texture *StateLoadGame::LoadTexture( std::string path)
{
    //temp texture
    sf::Texture *newTexture = new sf::Texture();
    //newTexture->loadFromFile(path);

    if(!newTexture->loadFromFile(path))
    {
        std::cout << "Could not load image on path: " << path << std::endl;
    }
    menuAmount++;
    return newTexture;
}
bool StateLoadGame::loadMedia()
{
    save1 = LoadTexture("data/loadgame/save1.png");
    save2 = LoadTexture("data/loadgame/save2.png");
    save3 = LoadTexture("data/loadgame/save3.png");
    save1Selected = LoadTexture("data/loadgame/save1selected.png");
    save2Selected = LoadTexture("data/loadgame/save2selected.png");
    save3Selected = LoadTexture("data/loadgame/save3selected.png");
    menuAmount -= 3;
    save1Sprite = new sf::Sprite;
    save1Sprite->setTexture(*save1);
    save1Sprite->setPosition(-70, -20);
    save2Sprite = new sf::Sprite;
    save2Sprite->setTexture(*save2);
    save2Sprite->setPosition(-70, 80);
    save3Sprite = new sf::Sprite;
    save3Sprite->setTexture(*save3);
    save3Sprite->setPosition(-70, 180);
    save1SelectedSprite = new sf::Sprite;
    save1SelectedSprite->setTexture(*save1Selected);
    save1SelectedSprite->setPosition(-70, -20);
    save2SelectedSprite = new sf::Sprite;
    save2SelectedSprite->setTexture(*save2Selected);
    save2SelectedSprite->setPosition(-70, 80);
    save3SelectedSprite = new sf::Sprite;
    save3SelectedSprite->setTexture(*save3Selected);
    save3SelectedSprite->setPosition(-70, 180);


    return true;
}
/**
 * Loads the selected game:
 * Player name
 * Player hp
 * Player position
 * Player elapsed time
 * Alive enemies position
 * @param selected The selected int value of a save
 */
void StateLoadGame::LoadGame(int selected, sf::RenderWindow& window, SavedObject& so)
{
    std::ifstream saveFile;
    std::string temp;
    std::string playerName;
    int playerHP;
    int playerX;
    int playerY;
    int timeElapsed;
    if(selected == 1)
        saveFile.open("SaveFiles/save1.txt");
    else if(selected == 2)
        saveFile.open("SaveFiles/save2.txt");
    else if(selected == 3)
        saveFile.open("SaveFiles/save3.txt");


    // Read information from file
    if(saveFile.is_open())
    {

        // Load player first
        std::getline(saveFile, playerName);
        std::getline(saveFile, temp);
        std::cout << temp << std::endl;
        playerHP = std::stoul(temp);
        std::getline(saveFile, temp);
        playerX = std::stoul(temp);
        std::getline(saveFile, temp);
        playerY = std::stoul(temp);
        std::getline(saveFile, temp);
        timeElapsed = std::stoul(temp);
        so.LoadPlayer(playerName, playerHP, playerX, playerY, timeElapsed);

        // Then load AI
        while(!saveFile.eof())
        {
            std::getline(saveFile, temp);
            int x = std::stoul(temp);
            std::getline(saveFile, temp);
            int y = std::stoul(temp);
            std::getline(saveFile, temp);
            int patrol = std::stoul(temp);
            std::getline(saveFile, temp);
            bool boss = false;
            if(!temp.compare("1"))
                boss = true;
            so.LoadAI(x, y, patrol, boss, window);
        }
    }
    saveFile.close();
    std::cout << "Loaded save file 1" << std::endl;
}

void StateLoadGame::GoNext(Machine &machine, sf::RenderWindow& window, SavedObject& so, Highscore&)
{
    while(sf::Keyboard::isKeyPressed(sf::Keyboard::Return));
    loadMedia();
    while (inLoad) {
        while (window.pollEvent(event)) {
            //If X clicked on window
            if (event.type == sf::Event::Closed) {
                window.close();
                exit(0);
            }
        }

        //If Escape pressed on keyboard
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
            machine.SetState(Machine::StateId::MAINMENU);
            inLoad = false;
            return;
        }

        /*****************************
         * If you press down or up key
         ****************************/
        if (!keyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            selected++;
            if (selected > menuAmount)
                selected = 1;
            keyPressed = true;
        }
        if (!keyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            selected--;
            if (selected <= 0)
                selected = menuAmount;
            keyPressed = true;
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            keyPressed = false;

        // Clear window
        window.clear(sf::Color::Black);

        window.draw(*save1Sprite); // Change to save1SpriteSelected later
        window.draw(*save2Sprite);
        window.draw(*save3Sprite);

        if(selected == 1)
            window.draw(*save1SelectedSprite);
        else if(selected == 2)
            window.draw(*save2SelectedSprite);
        else if(selected == 3)
            window.draw(*save3SelectedSprite);

        window.display();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && !keyPressed)
        {
            LoadGame(selected, window, so);
            machine.SetState(Machine::StateId::GAME);
            return;
        }

    }
}

