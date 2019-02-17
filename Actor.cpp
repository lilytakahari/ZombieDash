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
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                //move left
                break;
            case KEY_PRESS_RIGHT:
                //move right
                break;
            case KEY_PRESS_DOWN:
                // move
                break;
            case KEY_PRESS_UP:
                // move
                break;
        }
    }
}

