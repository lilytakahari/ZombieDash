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
    if (myWorld->allEscaped() && myWorld->overlapPenelope(this))
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
        if (randInt(1, 3) == 1)
        {
            myWorld->createActorAt('v', vomitX, vomitY, getDirection());
            myWorld->playSound(SOUND_ZOMBIE_VOMIT);
            return;
        }
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

bool SmartZombie::getKilled()
{
    Zombie::getKilled();
    getWorld()->increaseScore(2000);
    return true;
}

int SmartZombie::determineDirection()
{
    double humanX, humanY, humanDist;
    
    if (!getWorld()->findNearestHuman(this, humanX, humanY, humanDist))
        return getDirection();
    if (humanDist > 80)
        return randDir();
    else
    {
        int dir;
        bool fubar;
        return determineFollowDirection(humanX, humanY, dir, fubar);
    }
}

bool Citizen::getKilled()
{
    setDead();
    getWorld()->increaseScore(-1000);
    getWorld()->playSound(SOUND_CITIZEN_DIE);
    getWorld()->decPeople();
    return true;
}

void Citizen::doSomething()
{
    if (!stillAlive())
        return;
    StudentWorld* myWorld = getWorld();
    if (isInfected())
    {
        zombify();
        if (isZombieNow()) {
            setDead();
            myWorld->playSound(SOUND_ZOMBIE_BORN);
            myWorld->increaseScore(-1000);
            getWorld()->decPeople();
            if (randInt(1, 10) <= 3)
                myWorld->createActorAt('s', getX(), getY(), right);
            else
                myWorld->createActorAt('z', getX(), getY(), right);
            return;
        }
    }
    if (m_skipMove)
    {
        m_skipMove = false;
        return;
    }
    m_skipMove = true;
    double pX, pY, pDist;
    double zX, zY, zDist;
    double currX = getX();
    double currY = getY();
    if (!myWorld->distanceToPenelope(this, pX, pY, pDist))
        return;
    bool zExists = myWorld->findNearestZombie(currX, currY, zX, zY, zDist);
    if ((!zExists || (pDist < zDist)) && pDist <= 80)
    {
        int otherDir;
        bool needOtherDir;
        int dir = determineFollowDirection(pX, pY, otherDir, needOtherDir);
        if (!needOtherDir)
        {
            if (moveInDir(dir)){
                return;
            }
            
        } else {
            if (moveInDir(dir))
                return;
            else if (moveInDir(otherDir))
                return;
        }
    } else if (!zExists)
        return;
    if (zDist <= 80)
    {
        
       
        double fubar = VIEW_HEIGHT*VIEW_WIDTH;
        double distAllDirZombies[4] = {fubar, fubar, fubar, fubar};
        double potX[4] = {currX, currX, currX-2, currX+2};
        double potY[4] = {currY+2, currY-2, currY, currY};
        double zombX[4], zombY[4];
        for (int i = 0; i < 4; i++)
        {
            if (getWorld()->canMove(this, potX[i], potY[i])) {
                getWorld()->findNearestZombie(potX[i], potY[i], zombX[i], zombY[i], distAllDirZombies[i]);
            }
        }
        bool stay = true;
        for (int i = 0; i < 4; i++)
        {
            if (zDist < distAllDirZombies[i]) {
                stay = false;
                break;
            }
        }
        if (stay)
            return;
        int otherDir;
        int moveDir = reverseDirection(determineFollowDirection(zX, zY, otherDir, stay));
        moveInDir(moveDir);
        
    }
}

bool Citizen::moveInDir(int dir)
{
    double currX = getX();
    double currY = getY();
    bool couldMove = false;
    switch (dir)
    {
        case up:
            couldMove = canMoveTo(currX, currY+2);
            break;
        case down:
            couldMove = canMoveTo(currX, currY-2);
            break;
        case left:
            couldMove = canMoveTo(currX-2, currY);
            break;
        case right:
            couldMove = canMoveTo(currX+2, currY);
            break;
    }
    if (couldMove)
        setDirection(dir);
    return couldMove;
}

int Movers::determineFollowDirection(double x, double y, int &otherDir, bool &twoDir)
{
    int dir[2];
    if (getX() == x)
    {
        twoDir = false;
        if (getY() < y)
            return up;
        else
            return down;
    }
    else if (getY() == y)
    {
        twoDir = false;
        if (getX() < x)
            return right;
        else
            return left;
    }
    else
    {
        if (getX() < x)
            dir[0] = right;
        else
            dir[0] = left;
        if (getY() < y)
            dir[1] = up;
        else
            dir[1] = down;
        
        twoDir = true;
        int i = randInt(0, 1);
        if (i == 0)
            otherDir = dir[1];
        else
            otherDir = dir[0];
        return dir[i];
    }
}

int Citizen::reverseDirection(int dir)
{
    switch (dir) {
        case up:
            return down;
            break;
        case down:
            return up;
            break;
        case left:
            return right;
            break;
        case right:
            return left;
            break;
    }
    return up;
}
