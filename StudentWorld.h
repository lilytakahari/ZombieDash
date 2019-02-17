#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    bool determineOverlap(double x, double y, const Actor* other) const;
    bool determineBlocking(double x, double y, const Actor* other) const;
    bool canMove() const;
private:
    std::list<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
