#include <map>
#include "physics.h"


/**
 * This function handles movement to the left or to the right. The object will gradually reach its max velocity
 *  which it is limited by, and also gradually slow down at different rates wether one tries to move the
 * object to the other direction or not. It is limited by collision. Known bug: will skip through walls at high
 * velocities.
 * @param p The player character
 * @param collidableArray An array of collidable tiles to be used in collision detection.
 * @param delta tick time.
 */
void Physics::Movement(PlayerObject* p, int** collidableArray, float delta) {
    //Checks if player got hit by enemy
    if(p->GetPlayerHurt() != 5)
    {
        if(p->GetPlayerHurt() == 2)
        {

            p->SetApexCheck(false);
            p->SetJumpSpeed(10);
            p->SetFallSpeed(0);
        }
        else if(p->GetPlayerHurt() == 0)
        {
            p->SetMoveSpeedR(0);
            if(!HorisontalCollision(p, collidableArray))
                p->SetMoveSpeedL(400);
            p->health.Hit(10);
            p->PlayerSoundHurt();
        }
        else if(p->GetPlayerHurt() == 1)
        {
            p->SetMoveSpeedL(0);
            if(!HorisontalCollision(p, collidableArray))
                p->SetMoveSpeedR(400);
            p->health.Hit(10);
            p->PlayerSoundHurt();
        }
        p->SetPlayerHurt(5);
    }


    //Checks for trying to move to the left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        p->SetMovDir(0); //Means to the left
        p->SetSlowDownL(false);
    }
    else
        p->SetSlowDownL(true); //Object does not accelerate to left, should slow down if moving to left

    //Check for trying to move to the right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        p->SetMovDir(1); //Means to the right
        p->SetSlowDownR(false);
    }
    else
        //Object does not accelerate to the right, should slow down if moving to the right
        p->SetSlowDownR(true);

    /*********************
     * Left speed handler
     *********************/
    //If the object is moving to the left OR the movespeed to the left is greater than zero

    if (p->GetMovDir() == 0 || p->GetMoveSpeedL() > 0) {
        if (p->GetSlowDownL()) {
            if (p->GetMoveSpeedL() >= p->GetMovePower() * p->GetHorBrake() && p->GetMovDir() == 0)
            {
                //Trying to move the opposite direction of currently move direction, slows down pretty fast
                p->SetMoveSpeedL(p->GetMoveSpeedL() - p->GetMovePower() * p->GetHorBrake());
            }
            else if (p->GetMoveSpeedL() >= p->GetMovePower() * p->GetRegularBrake())
            {
                //Not accelerating nor trying to stop, slows down slowly
                p->SetMoveSpeedL(p->GetMoveSpeedL() - p->GetMovePower() * p->GetRegularBrake());
            }
            else
            {
                p->SetMoveSpeedL(0);
                p->SetMovDir(2);
                p->SetLastMoveDirection(0);
            }
            //Prevents the object from moving in the opposite direction if it tries to stop
        }
        else if (p->GetMoveSpeedL() < p->GetMaxMoveSpeed())
            //The current speed is increased if the object has not yet reached its max speed
            p->SetMoveSpeedL(p->GetMoveSpeedL() + p->GetMovePower());

        //The object is actually being moved
        if(!HorisontalCollision(p, collidableArray))
            p->SetPositionX(p->GetPositionX() - p->GetMoveSpeedL() * delta);
        else
            p->SetMoveSpeedL(0);
    }

    /*********************
     * Right speed handler
     *********************/
    //If the object is moving to the right OR the movespeed to the right is greater than zero
    if (p->GetMovDir() == 1 || p->GetMoveSpeedR() > 0)
    {
        if (p->GetSlowDownR())
        {
            if (p->GetMoveSpeedR() >= p->GetMovePower() * p->GetHorBrake() && p->GetMovDir() == 0)
            {
                //Trying to move the opposite direction of currently move direction, slows down pretty fast
                p->SetMoveSpeedR(p->GetMoveSpeedR() - p->GetMovePower() * p->GetHorBrake());
            }
            else if (p->GetMoveSpeedR() >= p->GetMovePower() * p->GetRegularBrake())
            {
                //Not accelerating nor trying to stop, slows down slowly
                p->SetMoveSpeedR(p->GetMoveSpeedR() - p->GetMovePower() * p->GetRegularBrake());
            }
            else
            {
                p->SetMoveSpeedR(0);
                p->SetMovDir(2);
                p->SetLastMoveDirection(1);
            }
            //Prevents the object from moving in the opposite direction if it tries to stop
        }
        else if (p->GetMoveSpeedR() < p->GetMaxMoveSpeed())
            //The current speed is increased if the object has not yet reached its max speed
            p->SetMoveSpeedR(p->GetMoveSpeedR() + p->GetMovePower());

        //The object is actually being moved
        if(!HorisontalCollision(p, collidableArray))
            p->SetPositionX(p->GetPositionX() + p->GetMoveSpeedR() * delta);
        else
            p->SetMoveSpeedR(0);
    }
}


/**
 * This function will make the object move vertically. The object is limited by its own
 * jumping strength (specified in the objects variables) and the gravity.
 * It will also stop accelerating beyond a specified maximum falling velocity,
 * which is meant to simulate the wind resistance we have here on earth.
 * The function checks every pixel it moves the object for collision, and
 * avoids moving it through anything collidable.
 * @param p The Object to be manipulated.
 * @param collidableArray An array of collidable tiles to be used in collision detection.
 * @param delta tick time.
 */
void Physics::Gravity(PlayerObject* p, int** collidableArray, float delta)
{
    //Gravity Start-----------------------------------------------------------------------------------------------------

    //Checks if object is in contact with platform,
    //then marks the object as not airborne(jumpcheck) and current fallspeed as 0.
    //Jumpspeed are also resetted for later use.
    if(Grounded(p, collidableArray))
    {
        //Checks if object is landing at a harmfull velocity, if so, harms it.
        if(p->GetFallSpeed() > p->GetMaxFallSpeed()/2.5)
        {
            p->health.Hit(p->GetFallSpeed()*0.2);
            p->PlayerSoundHurt();
        }
        p->SetJumpCheck(false);
        p->SetFallSpeed(0);
        float temp = p->GetOrigJumpSpeed();
        p->SetJumpSpeed(temp);
    }

        //Check if falling off a platform
        // If the object is not in contact with a platform, and it was not caused by jumping, the object is marked
        // as airborne (jumpcheck) and falling (apexcheck, reached the apex, on the way down).
    else if(!p->GetJumpCheck())
    {
        p->SetJumpCheck(true);
        p->SetApexCheck(true);
    }

    //Tries to make the object jump when Up key is pressed,
    //but only succeeds if the object is not already airborne (jumpcheck).
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !p->GetJumpCheck())
    {
        p->SetJumpCheck(true);
        p->SetApexCheck(false);
        p->PlayerSoundJump();
    }

    //Functionality for ascending or falling when the object is marked as airborne (jumpcheck).
    if(p->GetJumpCheck())
    {
        //The Object ascends at a decreasing rate when the object is marked as ascending (apexcheck).
        if(p->GetJumpSpeed() >= 0 && !p->GetApexCheck())
        {

            float pixelstomove = ((( p->GetJumpPower() * p->GetJumpSpeed() ) / p->GetGravity() ) * (delta));
            p->SetJumpSpeed(p->GetJumpSpeed() - 50*delta);
            //Martins carefull movement mechanism:
            //  -Makes the object check every pixel it moves for collision, prevents object in falling through the ground
            //  Bug: It might move 1 pixel into the ground, must investigate
            for(float i = pixelstomove; i >= 0; i -= 1)
            {
                if(Roofed(p, collidableArray))
                {
                    p->SetApexCheck(true);
                    break;
                }

                else
                    p->SetPositionY(p->GetPositionY() - 1);
            }
        }
            //The Object is descending (apexcheck) as well ass airborne (jumpcheck),
            //and therefore descends at an increasing rate because of how gravity works.
        else
        {
            p->SetApexCheck(true);
            float pixelstomove = (p->GetFallPower() * p->GetFallSpeed() / p->GetGravity() * (delta));

            //Martins carefull movement mechanism:
            //  -Makes the object check every pixel it moves for collision, prevents object in falling through the ground
            //  Bug: It might move 1 pixel into the ground, must investigate
            for(float i = pixelstomove; i >= 0; i -= 1)
            {
                if(Grounded(p, collidableArray))
                {
                    break;
                }

                else
                    p->SetPositionY(p->GetPositionY() + 1);
            }

            //Fallspeed cannot exceed the max fallspeed, the real life equivalent of wind resistance limiting
            //a falling objects fallspeed.
            if(p->GetFallSpeed() < p->GetMaxFallSpeed())
            {
                p->SetFallSpeed(p->GetFallSpeed() + 50*delta);
            }
        }
    }
    //Gravity End-------------------------------------------------------------------------------------------------------
}



/**
 * Function that checks if there is a horisontal collision
 * @param p the object
 * @param collidableArray 2D array with all collidable tiles
 * @return True if there is a block infront of object, false if not
 */
bool Physics::HorisontalCollision(PlayerObject* p, int** collidableArray)
{
    int upperPlayerYArrayCoord = (p->GetPositionY() / 32) >0? (p->GetPositionY()+17)/32 :0; //Experimental bug fix
    int lowerPlayerArrayCoord = upperPlayerYArrayCoord +1;
    int playerWestCoord = (p->GetPositionX() / 32);
    int playerEastCoord = playerWestCoord +1;

    return (p->GetMoveSpeedL() > 0 && (collidableArray[upperPlayerYArrayCoord][playerWestCoord] || collidableArray[lowerPlayerArrayCoord][playerWestCoord]))
           || (p->GetMoveSpeedR() > 0 && (collidableArray[upperPlayerYArrayCoord][playerEastCoord] || collidableArray[lowerPlayerArrayCoord][playerEastCoord]));
    /*
    if((p->GetMoveSpeedL() > 0 && (collidableArray[upperPlayerYArrayCoord][playerWestCoord] || collidableArray[lowerPlayerArrayCoord][playerWestCoord]))
       || (p->GetMoveSpeedR() > 0 && (collidableArray[upperPlayerYArrayCoord][playerEastCoord] || collidableArray[lowerPlayerArrayCoord][playerEastCoord])))
    {
        return true;
    }
    return false;*/
}


/*************************************************************
 * checks if object is in touch with a collidable tile beneath
 * @param p the object
 * @param collidableArray 2d array with all collidable tiles
 * @return true if on top of a tile, else false
 */
bool Physics::Grounded(PlayerObject* p, int** collidableArray)
{
    int playerArrayCoordX = (p->GetPositionX() + 17) / 32;
    int playerSouthCoord = ((p->GetPositionY() +50) / 32)>0? (p->GetPositionY()+50) / 32 :0;
    if(collidableArray[playerSouthCoord][playerArrayCoordX] == 142 || collidableArray[playerSouthCoord -1][playerArrayCoordX] == 142) //Hit lava
        p->health.Hit(1);

    return p->GetApexCheck() && collidableArray[playerSouthCoord][playerArrayCoordX];
}

/*******************************************************
 * Checks if object "head" is in touch with a tile above
 * @param p the object
 * @param collidableArray 2d array with all collidable tiles
 * @return true if in touch with tile above, else false
 */
bool Physics::Roofed(PlayerObject* p, int** collidableArray)
{
    int playerArrayCoordX = (p->GetPositionX() +17) / 32;
    int playerNorthCoord = (p->GetPositionY() / 32)>0?(p->GetPositionY()+30)/32 -1:0;

    if(collidableArray[playerNorthCoord][playerArrayCoordX] != 0)
    {
        return true;
    }
    return false;
}

/***************************************
 * AI PHYSICS
***************************************/

/**
 *  This function handles movement to the left or to the right. The object will gradually reach its max velocity
 * which it is limited by, and also gradually slow down at different rates wether one tries to move the
 * object to the other direction or not. It is limited by collision. Known bug: will skip through walls at high
 * velocities.
 * @param e The object going through AIMovement, an enemy in this case.
 * @param p The player character, to be used in collision detection.
 * @param AIVector A Vector containing all AI controlled objects. Used to check for AI on AI collision.
 * @param i Int containing the index of e. Used for excluding self collision when checking for AI on AI collision.
 * @param collidableArray Array containing all collidable tiles.
 * @param delta tick time.
 */
void Physics::AIMovement(AIEnemies* e, PlayerObject* p, std::vector<AIEnemies*>* AIVector, int i, int** collidableArray, float delta) {
    //This function handles movement to the left or to the right. The object will gradually reach its max velocity
    //which it is limited by, and also gradually slow down at different rates wether one tries to move the
    //object to the other direction or not.

    //Checks if object should be hurt or not
    Hurt(p, e, &i, AIVector);

    if(e->GetAIColliding() != 5)
    {
        if(e->GetAIColliding() == 2)
        {

            e->SetApexCheck(false);
            e->SetJumpSpeed(10);
            e->SetFallSpeed(0);
        }
        else if(e->GetAIColliding() == 0)
        {
            e->SetMoveSpeedR(0);
            e->SetMoveSpeedL(200);
        }
        else if(e->GetAIColliding() == 1)
        {
            e->SetMoveSpeedL(0);
            e->SetMoveSpeedR(200);
        }
        else
        {
            e->SetApexCheck(false);
            e->SetJumpSpeed(10);
            e->SetFallSpeed(0);
            e->SetMoveSpeedR(0);
            e->SetMoveSpeedL(200);
        }
        e->SetAIColliding(5);
    }

    //Checks for trying to move to the left
    if (e->GetLeftKey()) {
        e->SetMovDir(0); //Means to the left
        e->SetSlowDownL(false);
    }
    else
        e->SetSlowDownL(true); //Object does not accelerate to left, should slow down if moving to left

    //Check for trying to move to the right
    if (e->GetRightKey()) {
        e->SetMovDir(1); //Means to the right
        e->SetSlowDownR(false);
    }
    else
        //Object does not accelerate to the right, should slow down if moving to the right
        e->SetSlowDownR(true);

    /*********************
     * Left speed handler
     *********************/
    //If the object is moving to the left OR the movespeed to the left is greater than zero

    if (e->GetMovDir() == 0 || e->GetMoveSpeedL() > 0) {
        if (e->GetSlowDownL()) {
            if (e->GetMoveSpeedL() >= e->GetMovePower() * e->GetHorBrake() && e->GetMovDir() == 0)
            {
                //Trying to move the opposite direction of currently move direction, slows down pretty fast
                e->SetMoveSpeedL(e->GetMoveSpeedL() - e->GetMovePower() * e->GetHorBrake());
            }
            else if (e->GetMoveSpeedL() >= e->GetMovePower() * e->GetRegularBrake())
            {
                //Not accelerating nor trying to stop, slows down slowly
                e->SetMoveSpeedL(e->GetMoveSpeedL() - e->GetMovePower() * e->GetRegularBrake());
            }
            else
            {
                e->SetMoveSpeedL(0);
                e->SetMovDir(2);
                e->SetLastMoveDirection(0);
            }
            //Prevents the object from moving in the opposite direction if it tries to stop
        }
        else if (e->GetMoveSpeedL() < e->GetMaxMoveSpeed())
            //The current speed is increased if the object has not yet reached its max speed
            e->SetMoveSpeedL(e->GetMoveSpeedL() + e->GetMovePower());

        //The object is actually being moved
        if(!AIHorisontalCollision(e, collidableArray))
            e->SetPositionX(e->GetPositionX() - e->GetMoveSpeedL() * delta);
        else
            e->SetMoveSpeedL(0);
    }

    /*********************
     * Right speed handler
     *********************/
    //If the object is moving to the right OR the movespeed to the right is greater than zero
    if (e->GetMovDir() == 1 || e->GetMoveSpeedR() > 0)
    {
        if (e->GetSlowDownR())
        {
            if (e->GetMoveSpeedR() >= e->GetMovePower() * e->GetHorBrake() && e->GetMovDir() == 0)
            {
                //Trying to move the opposite direction of currently move direction, slows down pretty fast
                e->SetMoveSpeedR(e->GetMoveSpeedR() - e->GetMovePower() * e->GetHorBrake());
            }
            else if (e->GetMoveSpeedR() >= e->GetMovePower() * e->GetRegularBrake())
            {
                //Not accelerating nor trying to stop, slows down slowly
                e->SetMoveSpeedR(e->GetMoveSpeedR() - e->GetMovePower() * e->GetRegularBrake());
            }
            else
            {
                e->SetMoveSpeedR(0);
                e->SetMovDir(2);
                e->SetLastMoveDirection(1);
            }
            //Prevents the object from moving in the opposite direction if it tries to stop
        }
        else if (e->GetMoveSpeedR() < e->GetMaxMoveSpeed())
            //The current speed is increased if the object has not yet reached its max speed
            e->SetMoveSpeedR(e->GetMoveSpeedR() + e->GetMovePower());

        //The object is actually being moved
        if(!AIHorisontalCollision(e, collidableArray))
            e->SetPositionX(e->GetPositionX() + e->GetMoveSpeedR() * delta);


        else
            e->SetMoveSpeedR(0);
    }
}


/**
 * This function will make the object move vertically. The object is limited by its own
 * jumping strength (specified in the objects variables) and the gravity.
 * It will also stop accelerating beyond a specified maximum falling velocity,
 * which is meant to simulate the wind resistance we have here on earth.
 * The function checks every pixel it moves the object for collision, and
 * avoids moving it through anything collidable.
 * @param e The object going through the code, an enemy in this case.
 * @param collidableArray An array of all collidable tiles.
 * @param delta tick time.
 */
void Physics::AIGravity(AIEnemies* e, int** collidableArray, float delta)
{
    //Gravity Start-----------------------------------------------------------------------------------------------------

    //Checks if object is in contact with platform,
    //then marks the object as not airborne(jumpcheck) and current fallspeed as 0.
    //Jumpspeed are also reseted for later use.
    if(AIGrounded(e, collidableArray))
    {
        //Checks if object is landing at a harmfull velocity, if so, harms it.
        if(e->GetFallSpeed() > e->GetMaxFallSpeed()/2.4)
            e->health.Hit(e->health.GetOriginalLifePoints()/2);
        e->SetJumpCheck(false);
        e->SetFallSpeed(0);
        float temp = e->GetOrigJumpSpeed();
        e->SetJumpSpeed(temp);
    }

        //Check if falling off a platform
        // If the object is not in contact with a platform, and it was not caused by jumping, the object is marked
        // as airborne (jumpcheck) and falling (apexcheck, reached the apex, on the way down).
    else if(!e->GetJumpCheck())
    {
        e->SetJumpCheck(true);
        e->SetApexCheck(true);
    }

    //Tries to make the object jump when Up key is pressed,
    //but only succeeds if the object is not already airborne (jumpcheck).
    if (e->GetUpKey() && !e->GetJumpCheck())
    {
        e->SetJumpCheck(true);
        e->SetApexCheck(false);
    }

    //Functionality for ascending or falling when the object is marked as airborne (jumpcheck).
    if(e->GetJumpCheck())
    {
        //The Object ascends at a decreasing rate when the object is marked as ascending (apexcheck).
        if(e->GetJumpSpeed() >= 0 && !e->GetApexCheck())
        {

            float pixelstomove = ((( e->GetJumpPower() * e->GetJumpSpeed() ) / e->GetGravity() ) * (delta));
            e->SetJumpSpeed(e->GetJumpSpeed() - 50*delta);
            //Martins carefull movement mechanism:
            //  -Makes the object check every pixel it moves for collision, prevents object in falling through the ground
            //  Bug: It might move 1 pixel into the ground, must investigate
            for(float i = pixelstomove; i >= 0; i -= 1)
            {
                if(AIRoofed(e, collidableArray))
                {
                    e->SetApexCheck(true);
                    break;
                }

                else
                    e->SetPositionY(e->GetPositionY() - 1);
            }
        }
            //The Object is descending (apexcheck) as well ass airborne (jumpcheck),
            //and therefore descends at an increasing rate because of how gravity works.
        else
        {
            e->SetApexCheck(true);
            float pixelstomove = (e->GetJumpPower() * e->GetFallSpeed() / e->GetGravity() * (delta));

            //Martins carefull movement mechanism:
            //  -Makes the object check every pixel it moves for collision, prevents object in falling through the ground
            //  Bug: It might move 1 pixel into the ground, must investigate
            for(float i = pixelstomove; i >= 0; i -= 1)
            {
                if(AIGrounded(e, collidableArray))
                {
                    break;
                }

                else
                    e->SetPositionY(e->GetPositionY() + 1);
            }

            //Fallspeed cannot exceed the max fallspeed, the equivalent of real life air resistance limiting
            //a falling objects fallspeed.
            if(e->GetFallSpeed() < e->GetMaxFallSpeed())
            {
                e->SetFallSpeed(e->GetFallSpeed() + 50*delta);
            }
        }
    }
    e->SetUpKey(false);
    //Gravity End-------------------------------------------------------------------------------------------------------
}

/**
 * Function that checks if there is a horisontal collision, it also makes ai jump and informs miniboss of nearby pits.
 * @param e the object
 * @param collidabletiles Map with all collidable tiles
 * @return True if there is a block infront of object, false if not
 */
bool Physics::AIHorisontalCollision(AIEnemies* e, int** collidableArray)
{
    int upperObjectYArrayCoord = ((e->GetPositionY() -5) / 32) >0? (e->GetPositionY()-5)/32 :0; //Experimental bug fix
    if(e->GetBoss())
    {
        upperObjectYArrayCoord = ((e->GetPositionY() +5) / 32) >0? (e->GetPositionY() +5)/32 :0; //Experimental bug fix
    }
    int lowerObjectArrayCoord = upperObjectYArrayCoord +1;
    int ObjectWestCoord = (e->GetPositionX() / 32);
    int ObjectEastCoord = ObjectWestCoord +1;


    if((e->GetMoveSpeedL() > 0 && (collidableArray[upperObjectYArrayCoord][ObjectWestCoord] || collidableArray[lowerObjectArrayCoord][ObjectWestCoord]))
       || (e->GetMoveSpeedR() > 0 && (collidableArray[upperObjectYArrayCoord][ObjectEastCoord] || collidableArray[lowerObjectArrayCoord][ObjectEastCoord])))
    {
        e->SetUpKey(true);
        return true;
    }
    return false;
}


/*************************************************************
 * checks if object is in touch with a collidable tile beneath
 * @param e the object
 * @param collidableArray 2d array with all collidable tiles
 * @return true if on top of a tile, else false
 */
bool Physics::AIGrounded(AIEnemies* e, int** collidableArray)
{
    int ObjectArrayCoordX = (e->GetPositionX() + 16) / 32;
    int ObjectSouthCoord = (e->GetPositionY() + e->GetSizeHeight()/2 +3) / 32;

    if(e->GetBoss())
    {
        ObjectArrayCoordX = (e->GetPositionX() + (e->GetSizeWidth()/2)) / 32;
        ObjectSouthCoord = (e->GetPositionY() + 100) / 32;
        //Object southcord: +43 is underground, +44 is above ground. Need float.
    }



    if(e->GetApexCheck() == 1 && collidableArray[ObjectSouthCoord][ObjectArrayCoordX] != 0)
    {
        return true;
    }
    return false;
}

/*******************************************************||
 * Checks if objects "head" is in touch with a tile above
 * @param e the object
 * @param collidableArray 2d array with all collidable tiles
 * @return true if in touch with tile above, else false
 */
bool Physics::AIRoofed(AIEnemies* e, int** collidableArray)
{
    int ObjectArrayCoordX = (e->GetPositionX() + 17) / 32;
    int ObjectNorthCoord = (e->GetPositionY()+30 / 32)>0?(e->GetPositionY()+30)/32 -1:0;

    if(collidableArray[ObjectNorthCoord][ObjectArrayCoordX] != 0)
    {
        return true;
    }
    return false;
}


/**
 * The AI currently running physics checks wether it hurts the player or not.
 * The playerhurt variable will then be set appropriately as shown below.
 * It will then check if it collides with other AI.
 *
 *  * If player is being hurt, playerhurt will be:
 *  5 = Unharmed
 *  0 = From the left
 *  1 = From the right
 *  2 = From above
 *  3 = From beneath
 * @param p Player character
 * @param e Selected AI
 * @param i The index of the selected AI(to hinder self colliding)
 * @param AIVector The whole vector of all the AI
 */
void Physics::Hurt(PlayerObject*p, AIEnemies* e, int* i, std::vector<AIEnemies*>* AIVector)
{
    int px = p->GetPositionX() + p->GetSizeWidth()/2;
    int py = p->GetPositionY() + p->GetSizeHeight()/2;

    int ex = e->GetPositionX() + e->GetSizeWidth()/2;
    int ey = e->GetPositionY() + e->GetSizeHeight()/10;
    if(e->GetBoss())
    {
        int exright= e->GetPositionX() + e->GetSizeWidth()/2;
        int exleft = exright -e->GetSizeWidth()/2.5;
        ey = e->GetPositionY() + e->GetSizeHeight()/2;

        if((px - exleft < p->GetSizeWidth() && px - exleft > -p->GetSizeWidth()) &&
           (py - ey < p->GetSizeHeight() && py - ey > -p->GetSizeHeight()))
        {
            if(py < ey)
            {
                p->PlayerSoundEnemyLanded();
                p->SetPlayerHurt(2);
                e->health.Hit(100);
                e->GotHurt(e, p);
            }

            else if(px < ex &&  ex - px > ey - py)
                p->SetPlayerHurt(0);
            else if(px > ex && px - ex > ey - py)
                p->SetPlayerHurt(1);


            else
                p->SetPlayerHurt(3);

        }
        else if((px - exright < p->GetSizeWidth() && px - exright > -p->GetSizeWidth()) &&
                (py - ey < p->GetSizeHeight() && py - ey > -p->GetSizeHeight()))
        {
            if(px < ex &&  ex - px > ey - py)
                p->SetPlayerHurt(0);
            else if(px > ex && px - ex > ey - py)
                p->SetPlayerHurt(1);
            else if(py < ey)
            {
                p->PlayerSoundEnemyLanded();
                p->SetPlayerHurt(2);
                e->health.Hit(100);
                e->GotHurt(e, p);
            }

            else
                p->SetPlayerHurt(3);
        }
    }


    else if((px - ex < p->GetSizeWidth() && px - ex > -p->GetSizeWidth()) &&
            (py - ey < p->GetSizeHeight() && py - ey > -p->GetSizeHeight()))
    {
        if(px < ex &&  ex - px > ey - py)
            p->SetPlayerHurt(0);
        else if(px > ex && px - ex > ey - py)
            p->SetPlayerHurt(1);
        else if(py < ey)
        {
            p->PlayerSoundEnemyLanded();
            p->SetPlayerHurt(2);
            e->health.Hit(100);
            e->GotHurt(e, p);
        }

        else
            p->SetPlayerHurt(3);

    }
    AISelfCollision(e, i, AIVector);
}

/**
 * AI checks wether it collides with another AI or not.
 * If AI collides with other character, AIColliding will be:
 *  5 = Not colliding
 *  0 = From the left
 *  1 = From the right
 *  2 = From above
 *  3 = From beneath
 * @param e Selected AI
 * @param i The index of the selected AI(to hinder self colliding)
 * @param AIVector The whole vector of all the AI
 */
void Physics::AISelfCollision(AIEnemies* e, int* i, std::vector<AIEnemies*>* AIVector)
{
    int ex = e->GetPositionX() + e->GetSizeWidth()/2;
    int ey = e->GetPositionY() + e->GetSizeHeight()/2;

    for(unsigned int a = 0; a < AIVector->size(); a++)
    {
        if(!(*i==a))
        {
            int cx = AIVector->at(a)->GetPositionX() + AIVector->at(a)->GetSizeWidth()/2;
            int cy = AIVector->at(a)->GetPositionY() + AIVector->at(a)->GetSizeHeight()/2;

            if((ex - cx < e->GetSizeWidth() && ex - cx > -e->GetSizeWidth()) &&
               (ey - cy < e->GetSizeHeight() && ey - cy > -e->GetSizeHeight()))
            {
                {
                    if(ex < cx &&  cx - ex > cy - ey)
                    {
                        e->SetAIColliding(0);
                        AIVector->at(a)->SetAIColliding(1);
                    }

                    else if(ex > cx && ex - cx > cy - ey)
                    {
                        e->SetAIColliding(1);
                        AIVector->at(a)->SetAIColliding(0);
                    }

                    else if(ey < cy)
                    {
                        e->SetAIColliding(2);
                        AIVector->at(a)->SetAIColliding(3);
                    }
                    else
                        e->SetAIColliding(3);
                }
            }
        }
    }
}
