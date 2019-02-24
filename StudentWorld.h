#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Penelope;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool allEscaped() const {
        return (m_pplLeft == 0);
    }
    void decPeople() {
        m_pplLeft--;
    }
    void nowPassed() {
        m_passed = true;
    }
    bool determineOverlap(double x, double y, const Actor* act2) const;
    bool determineBlocking(double x, double y, const Actor* other) const;
    bool canMove(const Actor* requester, double x, double y) const;
    bool citizenEscapes(const Actor* exit);
    bool overlapPenelope(const Actor* requester);
    bool overlapMover(const Actor* requester);
    void awardGoodie(char type);
    void infectActors(const Actor* requester);
    void killActors(const Actor* requester);
    bool createActorAt(char type, double x, double y, int dir);
    bool detectVomitTarget(double x, double y);
    
private:
    std::list<Actor*> m_actors;
    Penelope* m_player;
    
    // KEEP TRACK OF CITIZENS, LEVEL
    int m_pplLeft;
    bool m_passed;
};

#endif // STUDENTWORLD_H_
