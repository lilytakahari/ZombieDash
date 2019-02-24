#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
#include "GameConstants.h"
#include <iostream>

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
            getKilled();
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
            }
                break;
            case KEY_PRESS_RIGHT: {
                //move right
                setDirection(right);
                canMoveTo(currX+4, currY);
            }
                break;
            case KEY_PRESS_DOWN: {
                // move down
                setDirection(down);
                canMoveTo(currX, currY-4);
            }
                break;
            case KEY_PRESS_UP: {
                // move up
                setDirection(up);
                canMoveTo(currX, currY+4);
            }
                 break;
            case KEY_PRESS_SPACE: {
                if (m_flames > 0) {
                    m_flames--;
                    myWorld->playSound(SOUND_PLAYER_FIRE);
                    int dir = getDirection();
                    for (int i = 1; i <= 3; i++) {
                        bool flameSuccess = true;
                        switch (dir)
                        {
                            case up: {
                                flameSuccess = myWorld->createActorAt('f', currX, currY + i * SPRITE_HEIGHT, up);
                                break;
                            }
                            case left: {
                                flameSuccess = myWorld->createActorAt('f', currX - i * SPRITE_WIDTH, currY , left);
                                break;
                            }
                            case down: {
                                flameSuccess = myWorld->createActorAt('f', currX, currY - i * SPRITE_HEIGHT, down);
                                break;
                            }
                            case right: {
                                flameSuccess = myWorld->createActorAt('f', currX + i * SPRITE_WIDTH, currY, right);
                                break;
                            }
                        }
                        if (!flameSuccess)
                            break;
                    }
                }
            }
                 break;
            case KEY_PRESS_ENTER: {
                if (m_vaccines > 0) {
                    cure();
                    std::cerr << "enter pressed" << std::endl;
                    m_vaccines--;
                }
            }
                break;
            case KEY_PRESS_TAB: {
                if (m_landmines > 0) {
                    if (getWorld()->createActorAt('l', currX, currY, right))
                        m_landmines--;
                }
            }
                break;
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

void Dmgers::doSomething() {
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

bool Penelope::getKilled() {
    getWorld()->playSound(SOUND_PLAYER_DIE);
    setDead();
    return true;
}

bool Zombie::getKilled()
{
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    setDead();
    return true;
}

void Zombie::doSomething()
{
    if (!stillAlive())
        return;
    if (m_skipMove) {
        m_skipMove = false;
        return;
    }
    m_skipMove = true;
    double vomitX, vomitY;
    computeThrowLoc(getDirection(), vomitX, vomitY);
    StudentWorld* myWorld = getWorld();
    if (myWorld->detectVomitTarget(vomitX, vomitY))
    {
        myWorld->createActorAt('v', vomitX, vomitY, getDirection());
        myWorld->playSound(SOUND_ZOMBIE_VOMIT);
        return;
    }
    if (m_moveplan == 0)
    {
        m_moveplan = randInt(3, 10);
        setDirection(determineDirection());
    }
    double currX = getX();
    double currY = getY();
    bool couldMove = false;
    switch (getDirection())
    {
        case up:
            couldMove = canMoveTo(currX, currY+1);
            break;
        case down:
            couldMove = canMoveTo(currX, currY-1);
            break;
        case left:
            couldMove = canMoveTo(currX-1, currY);
            break;
        case right:
            couldMove = canMoveTo(currX+1, currY);
            break;
    }
    if (!couldMove)
        m_moveplan = 0;
    else
        m_moveplan--;
}

int Zombie::randDir() {
    int dirs[4] = {up, down, left, right};
    return dirs[randInt(0, 3)];
}

void Zombie::computeThrowLoc(int dir, double& x, double& y)
{
    double throwX = getX();
    double throwY = getY();
    switch (dir)
    {
        case up:
            throwY += SPRITE_HEIGHT;
            break;
        case down:
            throwY -= SPRITE_HEIGHT;
            break;
        case right:
            throwX += SPRITE_WIDTH;
            break;
        case left:
            throwX -= SPRITE_WIDTH;
            break;
    }
    x = throwX;
    y = throwY;
}

bool DumbZombie::getKilled()
{
    Zombie::getKilled();
    getWorld()->increaseScore(1000);
    if (randInt(1, 10) == 1) {
        double vaccX, vaccY;
        computeThrowLoc(randDir(), vaccX, vaccY);
        if (getWorld()->overlapAny(vaccX, vaccY))
            return true;
        else
            getWorld()->createActorAt('x', vaccX, vaccY, right);
    }
    return true;
}

void Pit::damage() {
    getWorld()->killActors(this);
}

void Landmine::doSomething()
{
    if (!stillAlive())
        return;
    if (m_safety > 0) {
        std::cerr << m_safety << std::endl;
        m_safety--;
        return;
    }
    if (getWorld()->overlapPenelope(this)
        || getWorld()->overlapMover(this))
        explode();
}
bool Landmine::getKilled()
{
    explode();
    return true;
}

void Landmine::explode()
{
    setDead();
    StudentWorld* myWorld = getWorld();
    myWorld->playSound(SOUND_LANDMINE_EXPLODE);
    double currX = getX();
    double currY = getY();
    myWorld->createActorAt('f', currX, currY, up);
    myWorld->createActorAt('f', currX - SPRITE_WIDTH, currY, up);
    myWorld->createActorAt('f', currX + SPRITE_WIDTH, currY, up);
    myWorld->createActorAt('f', currX, currY - SPRITE_HEIGHT, up);
    myWorld->createActorAt('f', currX - SPRITE_WIDTH, currY - SPRITE_HEIGHT, up);
    myWorld->createActorAt('f', currX + SPRITE_WIDTH, currY + SPRITE_HEIGHT, up);
    myWorld->createActorAt('f', currX, currY + SPRITE_HEIGHT, up);
    myWorld->createActorAt('f', currX - SPRITE_WIDTH, currY + SPRITE_HEIGHT, up);
    myWorld->createActorAt('f', currX + SPRITE_WIDTH, currY - SPRITE_HEIGHT, up);
    myWorld->createActorAt('p', currX, currY, up);
}
