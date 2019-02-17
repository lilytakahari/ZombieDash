#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
#include "GameConstants.h"
Penelope::Penelope(StudentWorld *world, double startX, double startY)
: Human(world, IID_PLAYER, startX, startY,
        right, 0)
{
    m_landmines = 0;
    m_flames = 0;
    m_vaccines = 0;
}

void Penelope::doSomething()
{
    StudentWorld* myWorld = getWorld();
    if (!stillAlive())
        return;
    if (isInfected())
    {
        zombify();
        if (isZombieNow()) {
            setDead();
            getWorld()->playSound(SOUND_PLAYER_DIE);
            return;
        }
    }
    int ch;
    
    if (myWorld->getKey(ch))
    {
        double currX = getX();
        double currY = getY();
        
        switch (ch)
        {
            case KEY_PRESS_LEFT: {
                // move left
                setDirection(left);
                if (myWorld->canMove(this, currX-4, currY))
                    moveTo(currX-4, currY);
                break;
            }
            case KEY_PRESS_RIGHT: {
                //move right
                setDirection(right);
                if (myWorld->canMove(this, currX+4, currY))
                    moveTo(currX+4, currY);
                break;
            }
            case KEY_PRESS_DOWN: {
                // move down
                setDirection(down);
                if (myWorld->canMove(this, currX, currY-4))
                    moveTo(currX, currY-4);
                break;
            }
            case KEY_PRESS_UP: {
                // move up
                setDirection(up);
                if (myWorld->canMove(this, currX, currY+4))
                    moveTo(currX, currY+4);
                break;
            }
        }
    }
}

