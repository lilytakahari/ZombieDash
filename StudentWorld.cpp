#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

#include "Actor.h"
#include "Level.h"
#include <iostream>
#include <list>

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    Level lev(assetPath());
    string levelFile = "level01.txt";
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
        cerr << "Cannot find level01.txt data file" << endl;
    else if (result == Level::load_fail_bad_format)
        cerr << "Your level was improperly formatted" << endl;
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        for (double x = 0; x < LEVEL_WIDTH; x++) {
            for (double y = 0; y < LEVEL_HEIGHT; y++) {
                Level::MazeEntry ge = lev.getContentsOf(x,y);
                double screenX = x * SPRITE_WIDTH;
                double screenY = y * SPRITE_HEIGHT;
                switch (ge) // so x=80 and y=160
                {
                    case Level::empty:
                        break;
                    case Level::smart_zombie:
                        break;
                    case Level::dumb_zombie:
                        break;
                    case Level::player: {
                        m_player = new Penelope(this, screenX, screenY);
                        break;
                    }
                    case Level::exit:
                        break;
                    case Level::wall: {
                        Wall* created = new Wall(screenX, screenY);
                        m_actors.push_back(created);
                        break;
                    }
                    case Level::pit:
                        break;
                        // etc…
    
                }
            }
        }
    }
        return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (m_player->stillAlive())
        m_player->doSomething();
    if (!m_player->stillAlive()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    // FINISHING LEVEL
    
    // Prompt actors to do something.
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->stillAlive()) {
            (*it)->doSomething();
            if (!m_player->stillAlive()) {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            // FINISHING LEVEL
        }
    }
    
    // Remove dead actors
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if (!(*it)->stillAlive()) {
            delete (*it);
            m_actors.erase(it);
        }
    }
    // Update game status line
    // TODO
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        delete (*it);
        m_actors.erase(it);
    }
}

bool StudentWorld::determineOverlap(double x, double y, const Actor* other) const {
    //TODO
    return false;
}
bool StudentWorld::determineBlocking(double x, double y, const Actor* other) const {
    if (x < (other->getX() + SPRITE_WIDTH) &&
        (x + (SPRITE_WIDTH - 1)) > other->getX() &&
        y < (other->getY() + SPRITE_HEIGHT) &&
        (y + (SPRITE_HEIGHT - 1)) > other->getY()) {
        return true;
    }
    return false;
}
bool StudentWorld::canMove(const Actor* requester, double x, double y) const {
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if (((*it) != requester) && (*it)->canBlock() && (*it)->stillAlive())
        {
            if (determineBlocking(x, y, (*it)))
                return false;
        }
    }
    return true;
}
