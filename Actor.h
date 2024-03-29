#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor: public GraphObject
{
public:
    Actor(StudentWorld *world, int initHP, int imageID, double startX, double startY,
          int startDirection, int depth)
    : GraphObject(imageID, startX, startY,
                  startDirection, depth)
    {
        m_world = world;
        m_HP = initHP;
    }
    
    bool stillAlive() const {
        return (m_HP > 0);
    }
    void setDead() {
        m_HP = 0;
    }
    
    virtual void doSomething() = 0;
    virtual bool getKilled() = 0;
    
    virtual bool canBlock() const = 0;
    virtual bool canBeSteppedOn() const = 0;
    virtual bool canMove() const = 0;
    virtual bool notZombie() const = 0;
    virtual bool getInfected() = 0;
    
protected:
    void decHealth() {
        m_HP--;
    }
    StudentWorld* getWorld() const {
        return m_world;
    }
private:
    int m_HP;
    StudentWorld *m_world;
};

class Movers: public Actor
{
public:
    Movers(StudentWorld *world, int imageID, double startX, double startY,
          int startDirection, int depth)
    : Actor(world, 1, imageID, startX, startY,
            startDirection, depth)
    {}
    virtual bool canBlock() const {
        return true;
    }
    virtual bool canBeSteppedOn() const {
        return false;
    }
    virtual bool canMove() const {
        return true;
    }
protected:
    int determineFollowDirection(double x, double y, int &otherDir, bool &twoDir);
    bool canMoveTo(double destX, double destY);
};

class Human: public Movers
{
public:
    Human(StudentWorld *world, int imageID, double startX, double startY,
          int startDirection, int depth)
    : Movers(world, imageID, startX, startY,
            startDirection, depth)
    {
        m_infected = false;
        m_infectLvl = 0;
    }
    virtual bool notZombie() const {
        return true;
    }
    virtual bool getInfected() {
        if (m_infected == false)
        {
            m_infected = true;
            return true;
        }
        return false;
    }
    int getInfectLvl() const {
        return m_infectLvl;
    }
protected:
    bool isInfected() const {
        return m_infected;
    }
    bool isZombieNow() const {
        return (m_infectLvl >= 500);
    }
    void cure() {
        m_infected = false;
        m_infectLvl = 0;
    }
    void zombify() {
        m_infectLvl++;
    }
private:
    bool m_infected;
    int m_infectLvl;
};

class Penelope: public Human
{
public:
    Penelope(StudentWorld *world, double startX, double startY);
    virtual void doSomething();
    virtual bool getKilled();
    int getlm() const {
        return m_landmines;
    }
    int getfl() const {
        return m_flames;
    }
    int getvacc() const {
        return m_vaccines;
    }
    void awardGoodie(char type);
private:
    int m_landmines;
    int m_flames;
    int m_vaccines;
};

class Citizen: public Human
{
public:
    Citizen(StudentWorld* world, double startX, double startY)
    : Human(world, IID_CITIZEN, startX, startY, right, 0)
    {
        m_skipMove = false;
    }
    virtual void doSomething();
    virtual bool getKilled();
private:
    int reverseDirection(int dir);
    bool m_skipMove;
    bool moveInDir(int dir);
};

class Wall: public Actor
{
public:
    Wall(double startX, double startY)
    : Actor(nullptr, 1, IID_WALL, startX, startY, right, 0)
    {}
    virtual void doSomething() {}
    virtual bool canBlock() const {
        return true;
    }
    virtual bool canBeSteppedOn() const {
        return false;
    }
    virtual bool canMove() const {
        return false;
    }
    virtual bool notZombie() const {
        return true;
    }
    virtual bool getInfected() {
        return false;
    }
    virtual bool getKilled() {
        return false;
    }
};

class Exit: public Actor
{
public:
    Exit(StudentWorld *world, double startX, double startY)
    : Actor(world, 1, IID_EXIT, startX, startY, right, 1)
    {}
    virtual void doSomething();
    
    virtual bool canBlock() const {
        return true;
    }
    virtual bool canBeSteppedOn() const {
        return true;
    }
    virtual bool canMove() const {
        return false;
    }
    virtual bool notZombie() const {
        return true;
    }
    virtual bool getInfected() {
        return false;
    }
    virtual bool getKilled() {
        return false;
    }
};

class Landmine: public Actor
{
public:
    Landmine(StudentWorld* world, double startX, double startY)
    : Actor(world, 1, IID_LANDMINE, startX, startY, right, 1)
    {
        m_safety = 30;
    }
    virtual void doSomething();
    virtual bool getKilled();
    
    virtual bool canBlock() const {
        return false;
    }
    virtual bool canBeSteppedOn() const {
        return true;
    }
    virtual bool canMove() const {
        return false;
    }
    virtual bool notZombie() const {
        return true;
    }
    virtual bool getInfected() {
        return false;
    }
private:
    void explode();
    int m_safety;
};

class Goodie: public Actor
{
public:
    Goodie(StudentWorld *world, int imageID, double startX, double startY)
    : Actor(world, 1, imageID, startX, startY,
                   right, 1)
    {}
    virtual void doSomething();
    
    virtual bool canBlock() const {
        return false;
    }
    virtual bool canBeSteppedOn() const {
        return true;
    }
    virtual bool canMove() const {
        return false;
    }
    virtual bool notZombie() const {
        return true;
    }
    virtual bool getInfected() {
        return false;
    }
    virtual bool getKilled() {
        setDead();
        return true;
    }
private:
    virtual void tellWorld() = 0;
};

class VaccineGoodie: public Goodie
{
public:
    VaccineGoodie(StudentWorld *world, double startX, double startY)
    : Goodie(world, IID_VACCINE_GOODIE, startX, startY)
    {}
private:
    virtual void tellWorld();
};

class LandmineGoodie: public Goodie
{
public:
    LandmineGoodie(StudentWorld *world, double startX, double startY)
    : Goodie(world, IID_LANDMINE_GOODIE, startX, startY)
    {}
private:
    virtual void tellWorld();
};

class GasCanGoodie: public Goodie
{
public:
    GasCanGoodie(StudentWorld *world, double startX, double startY)
    : Goodie(world, IID_GAS_CAN_GOODIE, startX, startY)
    {}
private:
    virtual void tellWorld();
};

class Dmgers: public Actor
{
public:
    Dmgers(StudentWorld *world, int imageID, double startX, double startY,
                     int startDirection)
    : Actor(world, 2, imageID, startX, startY,
           startDirection, 0)
    {}
    virtual void doSomething();
    
    virtual bool getInfected() {
        return false;
    }
    virtual bool canBlock() const {
        return false;
    }
    virtual bool canBeSteppedOn() const {
        return true;
    }
    virtual bool canMove() const {
        return false;
    }
    virtual bool notZombie() const {
        return true;
    }
    virtual bool getKilled() {
        return false;
    }
private:
    virtual void damage() = 0;
};

class Vomit: public Dmgers
{
public:
    Vomit(StudentWorld *world, double startX, double startY,
          int startDirection)
    : Dmgers(world, IID_VOMIT, startX, startY, startDirection)
    {}
private:
    virtual void damage();
};

class Flame: public Dmgers
{
public:
    Flame(StudentWorld *world, double startX, double startY,
          int startDirection)
    : Dmgers(world, IID_FLAME, startX, startY, startDirection)
    {}
private:
    virtual void damage();
};

class Pit: public Dmgers
{
public:
    Pit(StudentWorld *world, double startX, double startY)
    : Dmgers(world, IID_PIT, startX, startY, right)
    {}
    virtual void doSomething() {
        if (!stillAlive())
            return;
        damage();
    }
private:
    virtual void damage();
};

class Zombie: public Movers
{
public:
    Zombie(StudentWorld *world, double startX, double startY)
    : Movers(world, IID_ZOMBIE, startX, startY, right, 0)
    {
        m_moveplan = 0;
        m_skipMove = false;
    }
    virtual void doSomething();
    virtual bool getKilled();
    
    virtual bool notZombie() const {
        return false;
    }
    virtual bool getInfected() {
        return false;
    }
protected:
    int randDir();
    void computeThrowLoc(int dir, double& x, double& y);
private:
    virtual int determineDirection() = 0;
    int m_moveplan;
    bool m_skipMove;
};

class DumbZombie: public Zombie
{
public:
    DumbZombie(StudentWorld *world, double startX, double startY)
    : Zombie(world, startX, startY)
    {}
    virtual bool getKilled();
private:
    virtual int determineDirection()
    {
        return randDir();
    }
};

class SmartZombie: public Zombie
{
public:
    SmartZombie(StudentWorld *world, double startX, double startY)
    : Zombie(world, startX, startY)
    {}
    virtual bool getKilled();
private:
    virtual int determineDirection();
};

#endif // ACTOR_H_
