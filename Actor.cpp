#include "Actor.h"
#include "StudentWorld.h"

#include "GameConstants.h"
#include <iostream>

// canMoveTo: if the moving actor can move to the destination, move there
//            else return false
bool Movers::canMoveTo(double destX, double destY)
{
    if (getWorld()->canMove(this, destX, destY)) {
        moveTo(destX, destY);
        return true;
    }
    return false;
}

// determineFollowDirection: determines the direction that the current mover
//                           should move in, in order to get closer to (x, y)
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

// Penelope constructor: initiate with goodie charges set to 0
Penelope::Penelope(StudentWorld *world, double startX, double startY)
: Human(world, IID_PLAYER, startX, startY,
        right, 0)
{
    m_landmines = 0;
    m_flames = 0;
    m_vaccines = 0;
}

// Steps:
// 1. Check if alive
// 2. Infected
// 3. Player input - may generate new actors
void Penelope::doSomething()
{
    StudentWorld* myWorld = getWorld();
    // 1
    if (!stillAlive())
        return;
    // 2
    if (isInfected())
    {
        zombify();
        if (isZombieNow()) {
            getKilled();
            return;
        }
    }
    
    // 3
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

// getKilled: other actors may call this function
//            to tell Penelope to die
bool Penelope::getKilled() {
    getWorld()->playSound(SOUND_PLAYER_DIE);
    setDead();
    return true;
}

// awardGoodie: other actors may call this function
//              to tell Penelope to increase goodie charges
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

// Steps:
// 1. If overlap with citizen
// 2. If overlap with Penelope and all citizens escaped
void Exit::doSomething()
{
    // 1
    StudentWorld* myWorld = getWorld();
    if (myWorld->citizenEscapes(this))
    {
        myWorld->increaseScore(500);
        myWorld->playSound(SOUND_CITIZEN_SAVED);
    }
    // 2
    if (myWorld->allEscaped() && myWorld->overlapPenelope(this))
        myWorld->nowPassed();
}

// Steps:
// 1. Check if alive
// 2. If overlap Penelope
void Goodie::doSomething()
{
    // 1
    if (!stillAlive())
        return;
    // 2
    StudentWorld* myWorld = getWorld();
    if (myWorld->overlapPenelope(this))
    {
        myWorld->increaseScore(50);
        myWorld->playSound(SOUND_GOT_GOODIE);
        setDead();
        tellWorld();
    }
}

// tellWorld: variously implemented based on what the 
// specific type of goodie awards to Penelope
void GasCanGoodie::tellWorld() {
    getWorld()->awardGoodie('f');
}
void LandmineGoodie::tellWorld() {
    getWorld()->awardGoodie('l');
}
void VaccineGoodie::tellWorld() {
    getWorld()->awardGoodie('v');
}

// Steps:
// 1. Check if alive
// 2. Damage
// 3. Decrement health
void Dmgers::doSomething() {
    // 1 
    if (!stillAlive())
        return;
    // 2
    damage();
    // 3
    decHealth();
}

// damage: variously implemented based on what the 
// specific type of damager does to actors
void Vomit::damage() {
    getWorld()->infectActors(this);
}
void Flame::damage() {
    getWorld()->killActors(this);
}
void Pit::damage() {
    getWorld()->killActors(this);
}

// Steps:
// 1. Check if alive
// 2. Decrement safety ticks if not active yet
// 3. If overlaps movers or Penelope, becomes active by exploding
void Landmine::doSomething()
{
    // 1
    if (!stillAlive())
        return;
    // 2
    if (m_safety > 0) {
        m_safety--;
        return;
    }
    // 3
    if (getWorld()->overlapPenelope(this)
        || getWorld()->overlapMover(this))
        explode();
}

// getKilled: if hit by a flame, it will explode
bool Landmine::getKilled()
{
    explode();
    return true;
}

// explode: landmine dies and generates flames around it
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

// Steps:
// 1. Check if alive
// 2. If skip move or not
// 3. Try to vomit
// 4. Determine new movement plan if needed
// 5. Determine destination location
// 6. If not blocked, move there
// 7. Else determine new movement in next tick
void Zombie::doSomething()
{
    // 1
    if (!stillAlive())
        return;
    // 2
    if (m_skipMove) {
        m_skipMove = false;
        return;
    }
    m_skipMove = true;
    // 3
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
    // 4
    if (m_moveplan == 0)
    {
        m_moveplan = randInt(3, 10);
        setDirection(determineDirection());
    }
    // 5
    double currX = getX();
    double currY = getY();
    bool couldMove = false;
    switch (getDirection())
    {
        // 6
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
    // 7
    if (!couldMove)
        m_moveplan = 0;
    else
        m_moveplan--;
}

// randDir: returns a random direction
int Zombie::randDir() {
    int dirs[4] = {up, down, left, right};
    return dirs[randInt(0, 3)];
}

// computeThrowLoc: computes the location to spawn something at
//                  based on the passed in dir
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

// getKilled: shared dying actions among dumb
//            and smart zombies
bool Zombie::getKilled()
{
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    setDead();
    return true;
}

// getKilled: other actors may call this function
//            to tell the zombie to die
//            dumb zombies have the chance to spawn
//            a vaccine upon being killed
// NOTE: the spec says to spawn the vaccine at the
//       zombie's current location, but I think the intended
//       and correct implementation was to throw it out in a direction
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

// getKilled: other actors may call this function
//            to tell the zombie to die
bool SmartZombie::getKilled()
{
    Zombie::getKilled();
    getWorld()->increaseScore(2000);
    return true;
}

// determineDirection: goes toward the nearest human if they are close
//                     else choose a random direction
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

// Steps:
// 1. Check if alive
// 2. Maybe zombify
// 3. If skip move or not
// 4. Determine distance to Penelope
// 5. Determine distance to nearest zombie
// 6. Maybe move toward Penelope
// 7. Maybe run away from zombies
void Citizen::doSomething()
{   
    // 1
    if (!stillAlive())
        return;
    StudentWorld* myWorld = getWorld();
    // 2
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
    // 3
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
    // 4
    if (!myWorld->distanceToPenelope(this, pX, pY, pDist))
        return;
    // 5
    bool zExists = myWorld->findNearestZombie(currX, currY, zX, zY, zDist);
    
    // 6
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
    
    // 7
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

// moveInDir: move in the given dir
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

// reverseDirection: so that a Citizen can use
// determineFollowDirection, then reverse that given direction
// to yeet themself away from the zombie
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

// getKilled: citizen died, oh no
bool Citizen::getKilled()
{
    setDead();
    getWorld()->increaseScore(-1000);
    getWorld()->playSound(SOUND_CITIZEN_DIE);
    getWorld()->decPeople();
    return true;
}
