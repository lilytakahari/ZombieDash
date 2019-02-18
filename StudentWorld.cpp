#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

#include "Actor.h"
#include "Level.h"
#include <list>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_player = nullptr;
    m_pplLeft = 0;
    m_passed = false;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    Level lev(assetPath());
    ostringstream fileName;
    fileName << "level";
    int num = getLevel();
    fileName.fill('0');
    fileName << setw(2) << num;
    fileName << ".txt";
    string levelFile = fileName.str();
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_PLAYER_WON;
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
                Actor* toAdd = nullptr;
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::smart_zombie:
                        break;
                    case Level::dumb_zombie:
                        break;
                    case Level::player:
                        m_player = new Penelope(this, screenX, screenY);
                        break;
                    case Level::exit:
                        toAdd = new Exit(this, screenX, screenY);
                        break;
                    case Level::wall:
                        toAdd = new Wall(screenX, screenY);
                        break;
                    case Level::pit:
                        break;
                    case Level::vaccine_goodie:
                        
                        break;
                    case Level::gas_can_goodie:
                        
                        break;
                    case Level::landmine_goodie:
                        
                        break;
                    case Level::citizen:
                        //m_pplLeft++;
                        break;
                }
                if (toAdd != nullptr)
                    m_actors.push_back(toAdd);
            }
        }
    }
        return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (m_player->stillAlive())
        m_player->doSomething();
    if (m_passed == true) {
        playSound(SOUND_LEVEL_FINISHED);
        return GWSTATUS_FINISHED_LEVEL;
    }
    if (!m_player->stillAlive()) {
        decLives();
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    
    // Prompt actors to do something.
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->stillAlive()) {
            (*it)->doSomething();
            if (m_passed == true) {
                playSound(SOUND_LEVEL_FINISHED);
                return GWSTATUS_FINISHED_LEVEL;
            }
            if (!m_player->stillAlive()) {
                decLives();
                playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
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
    ostringstream gameStatus;
    gameStatus << "Score:  ";
    gameStatus.fill('0');
    gameStatus << setw(6) << getScore();
    gameStatus << "  Level:  " << getLevel();
    gameStatus << "  Lives:  " << getLives();
    gameStatus << "  Vacc:  " << m_player->getvacc();
    gameStatus << "  Flames:  " << m_player->getfl();
    gameStatus << "  Mines:  " << m_player->getlm();
    gameStatus << "  Infected:  " << m_player->getInfectLvl();
    setGameStatText(gameStatus.str());
    
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
    m_pplLeft = 0;
    m_passed = false;
    m_player = nullptr;
}

bool StudentWorld::determineOverlap(const Actor* act1, const Actor* act2) const {
    double centX1 = act1->getX() + (SPRITE_WIDTH/2);
    double centY1 = act1->getY() + (SPRITE_HEIGHT/2);
    double centX2 = act2->getX() + (SPRITE_WIDTH/2);
    double centY2 = act2->getY() + (SPRITE_HEIGHT/2);
    
    return (((centX1 - centX2)*(centX1 - centX2) + (centY1 - centY2)*(centY1 - centY2)) <= 100);
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
        if (((*it) != requester) && (*it)->canBlock() &&
            !(*it)->canBeSteppedOn() &&(*it)->stillAlive())
        {
            if (determineBlocking(x, y, (*it)))
                return false;
        }
    }
    return true;
}

bool StudentWorld::citizenEscapes(const Actor* exit) {
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->canMove() && (*it)->notZombie() && (*it)->stillAlive())
        {
            if (determineOverlap(exit, *it)) {
                (*it)->setDead();
                decPeople();
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::overlapPenelope(const Actor *requester) {
    if (determineOverlap(requester, m_player))
        return true;
    return false;
}
