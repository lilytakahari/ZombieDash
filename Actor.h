#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
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
    void decHealth() {
        m_HP--;
    }
    
    virtual void doSomething() = 0;
    
    virtual bool canBlock() const = 0;
    virtual bool canBeSteppedOn() const = 0;
    virtual bool canMove() const = 0;
    virtual bool notZombie() const = 0;
    virtual bool getInfected() = 0;
    virtual bool getKilled() = 0;
protected:
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
    bool isInfected() const {
        return m_infected;
    }
    virtual bool getInfected() {
        if (m_infected == false)
        {
            m_infected = true;
            return true;
        }
        return false;
    }
    void zombify() {
        m_infectLvl++;
    }
    bool isZombieNow() const {
        return (m_infectLvl >= 500);
    }
    int getInfectLvl() const {
        return m_infectLvl;
    }
protected:
    void cure() {
        m_infected = false;
        m_infectLvl = 0;
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
    virtual bool getKilled();
private:
    int m_landmines;
    int m_flames;
    int m_vaccines;
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

class Goodie: public Actor
{
public:
    Goodie(StudentWorld *world, int imageID, double startX, double startY)
    : Actor(world, 1, imageID, startX, startY,
                   right, 1)
    {}
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
    virtual void doSomething();
    virtual void tellWorld() = 0;
    virtual bool getInfected() {
        return false;
    }
    virtual bool getKilled() {
        setDead();
        return true;
    }
};

class VaccineGoodie: public Goodie
{
public:
    VaccineGoodie(StudentWorld *world, double startX, double startY)
    : Goodie(world, IID_VACCINE_GOODIE, startX, startY)
    {
        m_invinc = 2;
    }
    virtual void doSomething() {
        if (m_invinc > 0)
            m_invinc--;
        Goodie::doSomething();
    }
    virtual bool getKilled() {
        if (m_invinc != 0)
            return false;
        setDead();
        return true;
    }
    virtual void tellWorld();
private:
    int m_invinc;
};

class LandmineGoodie: public Goodie
{
public:
    LandmineGoodie(StudentWorld *world, double startX, double startY)
    : Goodie(world, IID_LANDMINE_GOODIE, startX, startY)
    {}
    virtual void tellWorld();
};

class GasCanGoodie: public Goodie
{
public:
    GasCanGoodie(StudentWorld *world, double startX, double startY)
    : Goodie(world, IID_GAS_CAN_GOODIE, startX, startY)
    {}
    virtual void tellWorld();
};

class TempDmgers: public Actor
{
public:
    TempDmgers(StudentWorld *world, int imageID, double startX, double startY,
                     int startDirection)
    : Actor(world, 2, imageID, startX, startY,
           startDirection, 0)
    {}
    virtual void doSomething();
    virtual void damage() = 0;
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
};

class Vomit: public TempDmgers
{
public:
    Vomit(StudentWorld *world, double startX, double startY,
          int startDirection)
    : TempDmgers(world, IID_VOMIT, startX, startY, startDirection)
    {}
    virtual void damage();
};

class Flame: public TempDmgers
{
public:
    Flame(StudentWorld *world, double startX, double startY,
          int startDirection)
    : TempDmgers(world, IID_FLAME, startX, startY, startDirection)
    {}
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
    virtual bool notZombie() const {
        return false;
    }
    virtual bool getInfected() {
        return false;
    }
    virtual bool getKilled();
    virtual int determineDirection() = 0;
protected:
    int randDir();
    
private:
    int m_moveplan;
    bool m_skipMove;
};

class DumbZombie: public Zombie
{
public:
    DumbZombie(StudentWorld *world, double startX, double startY)
    : Zombie(world, startX, startY)
    {}
    virtual int determineDirection()
    {
        return randDir();
    }
    virtual bool getKilled();
};
#endif // ACTOR_H_
