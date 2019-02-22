#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
#include "GameConstants.h"

bool Movers::canMoveTo(double destX, double destY)
{
    if (getWorld()->canMove(this, destX, destY)) {
        moveTo(destX, destY);
        return true;
    }
    return false;
}

Penelope::Penelope(StudentWorld *world, double startX, double startY)
: Human(world, IID_PLAYER, startX, startY,
        right, 0)
{
    m_landmines = 0;
    m_flames = 0;
    m_vaccines = 0;
}
void Penelope::awardGoodie(char type)
{
    switch (type) {
        case 'f':
            m_flames += 5;
            break;
        case 'v':
            m_vaccines += 1;
            break;
        case 'l':
            m_landmines += 2;
            break;
    }
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
                canMoveTo(currX-4, currY);
                break;
            }
            case KEY_PRESS_RIGHT: {
                //move right
                setDirection(right);
                canMoveTo(currX+4, currY);
                break;
            }
            case KEY_PRESS_DOWN: {
                // move down
                setDirection(down);
                canMoveTo(currX, currY-4);
                break;
            }
            case KEY_PRESS_UP: {
                // move up
                setDirection(up);
                canMoveTo(currX, currY+4);
                break;
            }
        }
    }
}

void Exit::doSomething()
{
    StudentWorld* myWorld = getWorld();
    if (myWorld->citizenEscapes(this))
    {
        myWorld->increaseScore(500);
        myWorld->playSound(SOUND_CITIZEN_SAVED);
    }
    if (myWorld->overlapPenelope(this) && myWorld->allEscaped())
        myWorld->nowPassed();
}

void Goodie::doSomething()
{
    if (!stillAlive())
        return;
    StudentWorld* myWorld = getWorld();
    if (myWorld->overlapPenelope(this))
    {
        myWorld->increaseScore(50);
        myWorld->playSound(SOUND_GOT_GOODIE);
        setDead();
        tellWorld();
    }
}

void GasCanGoodie::tellWorld() {
    getWorld()->awardGoodie('f');
}
void LandmineGoodie::tellWorld() {
    getWorld()->awardGoodie('l');
}
void VaccineGoodie::tellWorld() {
    getWorld()->awardGoodie('v');
}

void TempDmgers::doSomething() {
    if (!stillAlive())
        return;
    damage();
    decHealth();
}

void Vomit::damage() {
    getWorld()->infectActors(this);
}

void Flame::damage() {
    getWorld()->killActors(this);
}
