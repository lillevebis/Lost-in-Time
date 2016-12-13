#include <iostream>
#include <fstream>
#include "app.h"
#include "Framework/physics.h"

TestApp::TestApp(sf::RenderWindow& window, SavedObject so)
{

    this->config = config;
    this->window = &window;

    /*****************
     * Load in images
     ****************/
    LoadImages();

    /****************************************************************************
     *                            LOADED FROM SAVE
     ***************************************************************************/
    if(so.LoadFromSave())
    {
        /********************|
         * Create the player
         *******************/
        player = new PlayerObject(so.GetPlayerX(), so.GetPlayerY(), &window);
        player->health.SetActualLifePoints(so.GetPlayerHP());
        playerName = so.GetPlayerName();
        playerNamed = true;

        /***********************
         *  Create the princess
         **********************/
        princess = new PrincessObject(4862, 558, 50, &window);

        /***********************************
         * Creating AI
         * Using vector to keep track on AIs
         **********************************/
        for(int i = 0; i<so.GetAIVectorPointer()->size(); i++)
        {
            int x = so.GetAIVectorPointer()->at(i)->GetPositionX();
            int y = so.GetAIVectorPointer()->at(i)->GetPositionY();
            int patrol = so.GetAIVectorPointer()->at(i)->GetPositionX()-so.GetAIVectorPointer()->at(i)->GetPatrolLeft();
            bool boss = so.GetAIVectorPointer()->at(i)->GetBoss();
            AIVectorPointer->push_back(new AIEnemies(x, y, patrol, boss, &window));
        }
        music->playMusic("/Jungle Theme 2.ogg");

    }
    /****************************************************************************
     *                             NEW GAME
     ***************************************************************************/
    else
    {
        /********************|
         * Create the player
         *******************/
        // 160, 398
        //4450, 558
        player = new PlayerObject(4000, 558, &window);

        /***********************
         *  Create the princess
         **********************/
        princess = new PrincessObject(4862, 558, 50, &window);

        /***********************************
         * Creating AI
         * Using vector to keep track on AIs
         **********************************/
        AIVectorPointer->push_back(new AIEnemies(4450, 500, 400, true, &window));
        AIVectorPointer->push_back(new AIEnemies(354, 1230, 200, false, &window));
        AIVectorPointer->push_back(new AIEnemies(1950, 1220, 150, false, &window));
        AIVectorPointer->push_back(new AIEnemies(2340, 250, 260, false, &window));
        //AIVectorPointer->push_back(new AIEnemies(Start X, Start Y, Patrol Each Way From Start, IsBoss?, *config, &window));
        AIVectorPointer->push_back(new AIEnemies(1530, 510, 400, false, &window));
        AIVectorPointer->push_back(new AIEnemies(4562, 878, 10, false, &window));
        AIVectorPointer->push_back(new AIEnemies(4917, 1134, 2, false, &window));
        AIVectorPointer->push_back(new AIEnemies(4157, 1358, 0, false, &window));

    }

    /*************************************************
     * Making 2D array for collidable tiles
     ************************************************/
    collidableArray = new int*[ArraySize];
    collidableArray[0] = new int[ArraySize * ArraySize];
    for (int i = 1; i < ArraySize; i++)
        collidableArray[i] = collidableArray[i - 1] + ArraySize;

    /*************************************************
     * Load map information from JSON into object list
     ************************************************/
    if (!Map::load("data/map.json", objects, collidableArray))
        std::cout << "Failed to load map data." << std::endl << std::endl;

    /*****************************************
     * Set camera position in middle of screen
     ****************************************/
    sf::View view = window.getDefaultView();
    view.setSize(view.getSize().x, view.getSize().y);
    view.setCenter(view.getCenter().x, view.getCenter().y);
    window.setView(view);

    /********************
        * Create the clock
        * Used to find delta
        *******************/
    clock = new sf::Clock;
    clock->restart();

    //TEST
    currentView = new sf::View;


    /********************
     * Play Music
     *******************/
    music = new Music();
    music->music.setLoop(true);


    /***********************************
     * Creating Timer and text for timer
     **********************************/
    font = new sf::Font;
    if(!font->loadFromFile("data/Fonts/Arial/arial.ttf"))
        std::cout << "Could not load font from directory 'data/font.ttf'" << std::endl;
    timer = new sf::Clock();
    timerInText = new sf::Text("hi", *font);
    timerInText->setCharacterSize(20);
    timerInText->setStyle(sf::Text::Regular);
    timerInText->setColor(sf::Color::Black);
    victoryText = new sf::Text("Score: ", *font);
    victoryText->setCharacterSize(50);
    victoryText->setStyle(sf::Text::Regular);
    victoryText->setColor(sf::Color::Red);
    victoryText->setOutlineThickness(4);
    victoryText->setOutlineColor(sf::Color::Black);

}

bool TestApp::Tick(Machine& machine, Highscore& highscore)
{
    while(!playerNamed)
    {
        keyPressed = true;
        sf::Event event;
        std::string nameString;
        window->clear(sf::Color::Black);
        name = new sf::Text("Please type in name", *font);
        name->setCharacterSize(50);
        name->setStyle(sf::Text::Regular);
        name->setColor(sf::Color::White);
        name->setPosition(250, 150);
        window->draw(*name);
        window->display();
        while(1)
        {
            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && !sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                keyPressed = false;

            while(window->pollEvent(event))
            {
                if(event.type == sf::Event::TextEntered)
                {
                    if(event.text.unicode < 123 && event.text.unicode >47 )
                    {
                        nameString += static_cast<char>(event.text.unicode);
                        name->setString(nameString);
                    }
                }
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace) && !keyPressed)
            {
                nameString = nameString.substr(0,nameString.size()-1);
                keyPressed = true;
                name->setString(nameString);
            }
            window->draw(*name);
            window->display();
            window->clear(sf::Color::Black);

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && !keyPressed)
            {
                playerName = nameString;
                playerNamed = true;
                break;
            }
        }
        clock->restart();
        timer->restart();
        music->playMusic("/Jungle Theme 2.ogg");
    }

    sf::Event event;
    float delta = clock->restart().asSeconds();

    /************************
     * Gets time elapsed and
     * place it in text form
     ***********************/

    if(winTime > 9)
        timerX = currentView->getCenter().x + 157;
    else if(winTime > 99)
        timerX = currentView->getCenter().x + 143;
    else
        timerX = currentView->getCenter().x + 163;
    timerY = currentView->getCenter().y - 229;
    int timeelapsed = timer->getElapsedTime().asSeconds() + penaltyTime - EscMenuTime;
    winTime = timer->getElapsedTime().asSeconds() + penaltyTime - EscMenuTime;
    std::string tempForTime = std::to_string(timeelapsed);
    timerInText->setString(tempForTime);
    timerInText->setPosition(timerX, timerY);



    /*************
     * KILL ALL AI
     ************/
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::K))
    {   int count = AIVectorPointer->size();

        while(count != 0)
        {
            AIVectorPointer->at(count-1)->Death();
            count--;
        }
        while(sf::Keyboard::isKeyPressed(sf::Keyboard::K));
    }

    /*********************
     * CHECK IF PLAYER WON
     ********************/
    if(princessSpawn &&
            player->GetPositionY() == princess->GetPositionY() &&
            player->GetPositionX() - princess->GetPositionX() >= 30 &&
            player->GetPositionX() - princess->GetPositionX() <= 50)
    {
        int score = 1000 / winTime;
        VictoryHandler(highscore, score, delta);

        sf::View mainMenuView = window->getDefaultView();
        mainMenuView.setCenter(512, 290);
        *currentView = mainMenuView;
        window->setView(mainMenuView);
        music->music.stop();
        machine.SetState(Machine::StateId::MAINMENU);
        return false;
    }


    // Get events from OS
    while (window->pollEvent(event))
    {
        // Close the window if the user clicks the x button
        if (event.type == sf::Event::Closed)
        {
            delete[] collidableArray[0];
            delete[] collidableArray;
            window->close();
            machine.SetRunning(false);
        }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        penaltyTime += 10;

    /*****************************************************************
     *        KEYBOARD EVENTS ( but not for physical actions )
     ****************************************************************/

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        int tempTime = timer->getElapsedTime().asSeconds();
        if(!EscMenu(machine))
        {
            sf::View mainMenuView = window->getDefaultView();
            mainMenuView.setCenter(512, 290);
            *currentView = mainMenuView;
            window->setView(mainMenuView);
            music->music.stop();
            machine.SetState(Machine::StateId::MAINMENU);
            return false;
        }

        EscMenuTime = EscMenuTime + timer->getElapsedTime().asSeconds() - tempTime;
        clock->restart();
    }
        /********************************
         * If player press P Pause game
         * If player press R Resume game
         *******************************/
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
    {
        music->music.pause();
        while(!sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            clock->restart();
        music->music.play();
    }
        // For testing only, prints out player position
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        std::cout << player->GetPositionX() << " " << player->GetPositionY() << std::endl;

        // When player presses G, player character is damaged. For testing purposes
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G) && player->health.GetActualLifePoints() > 0)
        player->health.Hit(5);

        // When player presses H, player character is healed. For testing purposes
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H) && player->health.GetActualLifePoints() < 100)
        player->health.Healed(5);

        // If player press M, mute music
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
    {
        music->music.pause();
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::T))
        std::cout << timeelapsed << std::endl;

    /* If player hits the bottom of map,
     * the player's lifepoint is reduced to 0 and player death function is called */
    if (player->GetPositionY() >= 2500)
    {
        penaltyTime = penaltyTime + 20;
        player->health.DeathHandle();
        player->PlayerDead();
        timer->restart();
    }

    if (player->health.Dead())
    {
        penaltyTime = penaltyTime + 20;
        player->health.DeathHandle();
        player->PlayerDead();
        timer->restart();
    }

    player->PlayerAnimation(delta);
    if(princessSpawn)
        princess->PrincessAnimation(delta);

    /***********************
     * Handles player and AI
     * movements
     **********************/
    Move(delta);

    /********************
     * POSITION HEALTHBAR
     *******************/
    player->health.SetPosition(currentView->getCenter().x - 70, currentView->getCenter().y - 231);
    HudSprite.setPosition(currentView->getCenter().x - 119, currentView->getCenter().y - 268);

    /***********************
     * Draws background images
     **********************/

    window->clear(sf::Color::Black);

    window->draw(Junglebackground1Sprite);
    window->draw(Cavebackground1Sprite);
    window->draw(Cavebackground2Sprite);
    window->draw(Cavebackground3Sprite);
    window->draw(Treebackground1Sprite);


    // Draw the map
    for (Object *object : objects)
    {
        //object->process(deltaTime);
        object->draw(*window);
    }

    player->DrawMe();
    if(princessSpawn)
        princess->DrawMe();
    /***********************
     * Draws HUD sprite
     **********************/
    window->draw(HudSprite);
    player->health.DrawMe(*window);


    AIHandler(delta);
    window->draw(*timerInText);

    // Update health before displaying it ( Will not be in use in this version )
    // p->health.UpdateHealth();


    window->display();

    /*****************************************
     * Set camera to follow player position
     ****************************************/

    if (player->GetPositionX() >= 512)
    {
        if (player->GetPositionY() <= 1290)
        {
            sf::View view2 = window->getDefaultView();
            view2.setCenter(player->GetPositionX(), player->GetPositionY());
            *currentView = view2;
            window->setView(view2);

            // Positions background movement
            Junglebackground1Sprite.setPosition((player->GetPositionX()*1.5/8)-195,-300);
            Cavebackground2Sprite.setPosition((player->GetPositionX()*1.5/8)-195,600);
        }

        if (player->GetPositionX() >= 5238)
        {
            if (player->GetPositionY() <= 1290) {
                sf::View view5 = window->getDefaultView();
                view5.setCenter(5238, player->GetPositionY());
                *currentView = view5;
                window->setView(view5);
            }
        }
        else if(player->GetPositionY() >= 1290)
        {
            sf::View view2 = window->getDefaultView();
            view2.setCenter(player->GetPositionX(), 1290);
            *currentView = view2;
            window->setView(view2);

            // Positions background movement
            Cavebackground2Sprite.setPosition((player->GetPositionX()*1.5/8)-195,600);
            Junglebackground6Sprite.setPosition(player->GetPositionX()*1.5/8,200);
        }
    }
    else if (player->GetPositionX() <=  512)
    {
        if (player->GetPositionY() <= 1290)
        {
            sf::View view3 = window->getDefaultView();
            view3.setCenter(512, player->GetPositionY());
            *currentView = view3;
            window->setView(view3);

            // Positions background movement
            Junglebackground1Sprite.setPosition(-100,-300);
            Cavebackground2Sprite.setPosition(-100,600);

        }
        else if (player->GetPositionY() >= 1290)
        {
            sf::View view3 = window->getDefaultView();
            view3.setCenter(512, 1290);
            *currentView = view3;
            window->setView(view3);

            // Positions background movement
            Cavebackground2Sprite.setPosition(-100,600);
        }
    }

    return true;
}
/*****************************************************
 * This function will make the object able
 * to move and fall,
 * with correlation to any hinderings such as gravity.
 * The delta| float is there for smoothness
 ****************************************************/
void TestApp::Move(float delta)
{
    Physics::Movement(player, collidableArray, delta);
    Physics::Gravity(player, collidableArray, delta);
    princess->PrincessAI(princess,player);
    Physics::PrincessMovement(princess, collidableArray, delta);
    Physics::PrincessGravity(princess, collidableArray, delta);
}

/**
 * AIHandler handles all the AI, and sends each of them
 * through the same process as the player(animation, drawing, physics
 * and AI ofcourse).
 * @param delta
 */
void TestApp::AIHandler(float delta)
{
    for(unsigned int i = 0; i < AIVector.size(); i++)
    {
        /* If AI hits the bottom of map,
        * the AI's lifepoint is reduced to 0 and AI death function is called */
        if (AIVectorPointer->at(i)->GetPositionY() >= 2500)
        {
            AIVectorPointer->at(i)->health.Hit(AIVectorPointer->at(i)->health.GetOriginalLifePoints());
            AIVectorPointer->at(i)->health.DeathHandle();
            AIVectorPointer->erase(AIVectorPointer->begin() + i);
        }

        if (AIVectorPointer->at(i)->health.GetActualLifePoints() <= 0)//Dead())
        {
            if(AIVectorPointer->at(i)->GetBoss())
                princessSpawn = true;
            AIVectorPointer->at(i)->health.DeathHandle();
            AIVectorPointer->erase(AIVectorPointer->begin() + i);
        }
        else
        {
            if(AIVectorPointer->at(i)->GetBoss())
                AIVectorPointer->at(i)->AnimationBoss(delta);
            else if(AIVectorPointer->at(i)->GetMiniBoss())
                AIVectorPointer->at(i)->AnimationAIMiniBoss(delta);
            else
                AIVectorPointer->at(i)->AnimationAI(delta);
            AIVectorPointer->at(i)->DrawMe();
            if(AIVectorPointer->at(i)->GetBoss())
                AIVectorPointer->at(i)->MonkeyAI2(AIVectorPointer->at(i), player);
            else
                AIVectorPointer->at(i)->MonkeyAI1(AIVectorPointer->at(i), player);
            Physics::AIMovement(AIVectorPointer->at(i), player, AIVectorPointer, i, collidableArray, delta);
            Physics::AIGravity(AIVectorPointer->at(i), collidableArray, delta);
        }
    }
}

bool TestApp::VictoryHandler(Highscore& highscore, int score, float delta)
{
    if( AIVectorPointer->size() == 0/*BOSS DEAD*/)
    {
        window->clear(sf::Color::Black);
        window->draw(Treebackground1Sprite);
        // Process and render each object
        for (Object *object : objects)
        {
            //object->process(deltaTime);
            object->draw(*window);
        }
        sf::View victoryView = window->getDefaultView();

        sf::Clock end;
        victoryView.setCenter(player->GetPositionX() - 200, player->GetPositionY());
        *currentView = victoryView;
        window->setView(*currentView);

        victoryText = highscore.SaveNewHighscore(playerName, score);
        std::string temp = victoryText->getString();
        std::string tempForScore = std::to_string(score);
        if(!temp.compare("high"))
            victoryText->setString(victoryText->getString() + " New score: " + tempForScore);
        else if(!temp.compare("new"))
            victoryText->setString("Welcome newcomer! Score: " + tempForScore);
        else
            victoryText->setString("Score: " + tempForScore);

        victoryText->setPosition(player->GetPositionX() - 500, player->GetPositionY() - 200);

        // Int to make heart fly
        int makeLoveFly = 0;
        // just a helper to make things go slow
        int count = 0;
        sf::Vector2f scale = heartSprite.getScale();
        player->DrawMe();
        princess->DrawMe();
        // MAKE PRINCESS SHOW UP
        window->display();
        // if player nearby princess
        if(1)
        {
            // True love never ends when finally found the perfect girl
            bool truelove = true;
            // Victory speach
            // Then make a heart come up from hell
            while (truelove)
            {
                princess->PrincessAnimation(delta);
                player->PlayerAnimation(delta);
                end.restart();
                if(heartSprite.getPosition().y == player->GetPositionY() - 290)
                {
                    heartSprite.scale(scale.x * 1.01, scale.y * 1.01);
                    if(count == 100)
                    {
                        // Save highscore
                        // end game
                        return true;

                    }
                    count++;
                }
                else
                {
                    count++;
                    if(count == 1)
                    {
                        count = 0;
                        makeLoveFly++;
                        heartSprite.setPosition(currentView->getCenter().x - 510, currentView->getCenter().y + 350 - makeLoveFly);
                    }
                }
                window->clear(sf::Color::Black);
                window->draw(Treebackground1Sprite);
                // Process and render each object
                for (Object *object : objects)
                {
                    //object->process(deltaTime);
                    object->draw(*window);
                }
                window->draw(heartSprite);
                player->DrawMe();
                princess->DrawMe();
                window->draw(*victoryText);
                window->display();
            }
        }

    }
    return false;
}

/**
 * Menu for pressing Esc
 * Options:
 * ResumeGame: Resumes the game
 * SaveGame: Opens new window with saves
 * MainMenu: Returns to mainmenu
 * ExitGame: Exits the game
 * @param machine so we can handle the machine
 *          if you go back to mainmenu og exit game
 */
bool TestApp::EscMenu(Machine& machine)
{
    music->music.pause();
    // Show up a menu

    int selected = menuSelected("EscMenu");

    std::cout << "Selected: " << selected << std::endl;

    // Resume game
    if(selected == 1)
    {
        music->music.play();
        return true;
    }

        // Save game
    else if(selected == 2)
    {
        selected = menuSelected("SaveGameMenu");
        SaveGame(selected);
    }

    else if(selected == 3)
    {
        music->music.stop();
        return false;
    }

        // Exit game
    else if(selected == 4)
    {
        delete[] collidableArray[0];
        delete[] collidableArray;
        window->close();
        machine.SetRunning(false);
        exit(0);
    }
    clock->restart();
    music->music.play();
    return true;
}

bool TestApp::SaveGame(int selectedSave)
{

    int enemyCount = (AIVector.size());
    enemyCount = AIVectorPointer->size();


    std::ofstream savefile;
    if(selectedSave == 1)
        savefile.open("SaveFiles/save1.txt");
    else if(selectedSave == 2)
        savefile.open("SaveFiles/save2.txt");
    else if(selectedSave == 3)
        savefile.open("SaveFiles/save3.txt");

    savefile << playerName << std::endl;
    savefile << player->health.GetActualLifePoints() << std::endl;
    savefile << player->GetPositionX() << std::endl;
    savefile << player->GetPositionY() << std::endl;
    savefile << timer->getElapsedTime().asSeconds() + penaltyTime;
    while(enemyCount != 0)
    {
        //int positionX = AIVector->at(enemyCount-1)->GetOriginalX();
        int positionX = AIVectorPointer->at(enemyCount-1)->GetPositionX();
        int positionY = AIVectorPointer->at(enemyCount-1)->GetPositionY();
        int patrol = AIVectorPointer->at(enemyCount-1)->GetPositionX() - AIVectorPointer->at(enemyCount-1)->GetPatrolRight();
        bool boss = AIVectorPointer->at(enemyCount-1)->GetBoss();
        savefile << std::endl << positionX;
        savefile << std::endl << positionY;
        savefile << std::endl << patrol;
        savefile << std::endl << boss;
        enemyCount--;
    }
    savefile.close();
    return true;
}

int TestApp::menuSelected(std::string menu)
{
    // Go until Return is not pressed, to prevent program going through several "Menuselected" functions on 1 return press
    while(sf::Keyboard::isKeyPressed(sf::Keyboard::Return));
    int amountOfChoices = 0;
    int choice = 1;
    int MenuPositionX = player->GetPositionX() -200;
    int MenuPositionY = player->GetPositionY() - 200;

    while(1){
        if(!menu.compare("EscMenu"))
        {
            resumeGameSprite.setPosition(MenuPositionX, MenuPositionY);
            saveGameSprite.setPosition(MenuPositionX, MenuPositionY + 100);
            mainMenuSprite.setPosition(MenuPositionX, MenuPositionY + 200);
            exitGameSprite.setPosition(MenuPositionX, MenuPositionY + 300);
            window->clear(sf::Color::Black);
            // Load images for esc menu
            window->draw(resumeGameSprite);
            window->draw(saveGameSprite);
            window->draw(mainMenuSprite);
            window->draw(exitGameSprite);
            amountOfChoices = amountOfEscOptions;
        }

        else if(!menu.compare("SaveGameMenu")) // If menu equals "SaveGameMenu";
        {
            window->clear(sf::Color::Black);
            save1Sprite.setPosition(MenuPositionX, MenuPositionY);
            save2Sprite.setPosition(MenuPositionX, MenuPositionY + 100);
            save3Sprite.setPosition(MenuPositionX, MenuPositionY + 200);
            // Load images for savegame menu
            window->draw(save1Sprite);
            window->draw(save2Sprite);
            window->draw(save3Sprite);
            amountOfChoices = amountOfSaves;
        }


        if(!keyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            choice++;
            if (choice > amountOfChoices)
                choice = 1;
            keyPressed = true;
        }
        else if(!keyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            choice--;
            if (choice <= 0)
                choice = amountOfChoices;
            keyPressed = true;
        }
        else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            keyPressed = false;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
            return choice;

        selectedSprite.setPosition(player->GetPositionX() + 250, player->GetPositionY() - 300 + choice*100);
        window->draw(selectedSprite);
        window->display();
        if(amountOfChoices == amountOfEscOptions)
        {
            // Handle esc options
        }
        else if (amountOfChoices == amountOfSaves)
        {
            // Handle save options
        }
    }
}

void TestApp::LoadImages()
{
    int nothing;
    /*
     * TIL VETLE!!!
     * du gjør nesten akkurat som på skolen, men legger inn ", nothing" bak pathen
     * background1 = LoadTexture("pathen", nothing)
     */
    heart = LoadTexture("data/victory/heart.png",nothing);

    save1 = LoadTexture("data/loadgame/save1.png", amountOfSaves);
    save2 = LoadTexture("data/loadgame/save2.png", amountOfSaves);
    save3 = LoadTexture("data/loadgame/save3.png", amountOfSaves);

    selected = LoadTexture("data/EscMenu/selected.png", nothing);

    resumeGame = LoadTexture("data/EscMenu/ResumeGame.png", amountOfEscOptions);
    mainMenu = LoadTexture("data/EscMenu/MainMenu.png", amountOfEscOptions);
    saveGame = LoadTexture("data/EscMenu/SaveGame.png", amountOfEscOptions);
    exitGame = LoadTexture("data/EscMenu/ExitGame.png", amountOfEscOptions);

    heartSprite.setTexture(*heart);

    save1Sprite.setTexture(*save1);
    save2Sprite.setTexture(*save2);
    save3Sprite.setTexture(*save3);

    selectedSprite.setTexture(*selected);
    resumeGameSprite.setTexture(*resumeGame);
    saveGameSprite.setTexture(*saveGame);
    mainMenuSprite.setTexture(*mainMenu);
    exitGameSprite.setTexture(*exitGame);

    Junglebackground1 = LoadTexture("data/Backgrounds/Jungle11.png", nothing);
    Cavebackground1 = LoadTexture("data/Backgrounds/Cave3.png", nothing);
    Treebackground1 = LoadTexture("data/Backgrounds/Tree.png", nothing);
    Hud = LoadTexture("data/Backgrounds/Hud.png", nothing);

    Cavebackground1Sprite.setTexture(*Cavebackground1);
    Cavebackground1Sprite.setPosition(0,600);

    Cavebackground2Sprite.setTexture(*Cavebackground1);
    Cavebackground2Sprite.setPosition(1200,600);

    Treebackground1Sprite.setTexture(*Treebackground1);
    Treebackground1Sprite.setPosition(3150,-10);

    Junglebackground1Sprite.setTexture(*Junglebackground1);
    Junglebackground1Sprite.setPosition(0,-300);

    HudSprite.setTexture(*Hud);
    HudSprite.setPosition(0,300);

}

sf::Texture *TestApp::LoadTexture(std::string path, int &menuAmount)
{
    //temp texture
    sf::Texture *newTexture = new sf::Texture();
    //newTexture->loadFromFile(path);

    if(!newTexture->loadFromFile(path))
        std::cout << "Could not load image on path: " << path << std::endl;
    menuAmount++;
    return newTexture;
}