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
#include <cmath>

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

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

// init:
// 1. Load Level data
// 2. Based on the Level data, initialize actors
int StudentWorld::init()
{
    // 1
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
        return GWSTATUS_LEVEL_ERROR;
	
    // 2
    else if (result == Level::load_success)
    {
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
                        toAdd = new SmartZombie(this, screenX, screenY);
                        break;
                    case Level::dumb_zombie:
                        toAdd = new DumbZombie(this, screenX, screenY);
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
                        toAdd = new Pit(this, screenX, screenY);
                        break;
                    case Level::vaccine_goodie:
                        toAdd = new VaccineGoodie(this, screenX, screenY);
                        break;
                    case Level::gas_can_goodie:
                        toAdd = new GasCanGoodie(this, screenX, screenY);
                        break;
                    case Level::landmine_goodie:
                        toAdd = new LandmineGoodie(this, screenX, screenY);
                        break;
                    case Level::citizen:
                        m_pplLeft++;
                        toAdd = new Citizen(this, screenX, screenY);
                        break;
                }
                if (toAdd != nullptr)
                    m_actors.push_back(toAdd);
            }
        }
    }
        return GWSTATUS_CONTINUE_GAME;
}

// move:
// 1. Prompt actors to do something
// 2. Remove dead actors
// 3. Update game status line
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
    
    // 1. Prompt actors to do something
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
    
    // 2. Remove dead actors
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); )
    {
        if (!(*it)->stillAlive()) {
            delete (*it);
            it = m_actors.erase(it);
        } else
	    it++;
    }
    // 3. Update game status line
    ostringstream gameStatus;
    gameStatus << "Score: ";
    gameStatus.fill('0');
    int score = getScore();
    if (score < 0) {
        score = abs(score);
        gameStatus << "-";
    }
    gameStatus << setw(6) << score;
    gameStatus << "  Level: " << getLevel();
    gameStatus << "  Lives: " << getLives();
    gameStatus << "  Vaccines: " << m_player->getvacc();
    gameStatus << "  Flames: " << m_player->getfl();
    gameStatus << "  Mines: " << m_player->getlm();
    gameStatus << "  Infected: " << m_player->getInfectLvl();
    setGameStatText(gameStatus.str());
    
    return GWSTATUS_CONTINUE_GAME;
}

// cleanUp: delete all actors
void StudentWorld::cleanUp()
{
    delete m_player;
    m_player = nullptr;	
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); )
    {
        delete (*it);
        it = m_actors.erase(it);
    }
    m_pplLeft = 0;
    m_passed = false;
}

// determineOveralap: true if the passed in actor and the x and y overlap
bool StudentWorld::determineOverlap(double x, double y, const Actor* act2) const
{
    double centX1 = x + (SPRITE_WIDTH/2);
    double centY1 = y + (SPRITE_HEIGHT/2);
    double centX2 = act2->getX() + (SPRITE_WIDTH/2);
    double centY2 = act2->getY() + (SPRITE_HEIGHT/2);
    
    return (((centX1 - centX2)*(centX1 - centX2) + (centY1 - centY2)*(centY1 - centY2)) <= 100);
}

// determineBlocking: true if the passed in actor and the x and y are blocked
bool StudentWorld::determineBlocking(double x, double y, const Actor* other) const
{
    if (x < (other->getX() + SPRITE_WIDTH) &&
        (x + SPRITE_WIDTH) > other->getX() &&
        y < (other->getY() + SPRITE_HEIGHT) &&
        (y + SPRITE_HEIGHT) > other->getY()) {
        return true;
    }
    return false;
}

// canMove: true if the requester actor can move to (x, y)
bool StudentWorld::canMove(const Actor* requester, double x, double y) const
{
    if (m_player != requester && determineBlocking(x, y, m_player))
        return false;
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

// citizenEscapes: true if any citizen overlaps with the exit
bool StudentWorld::citizenEscapes(const Actor* exit)
{
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->canMove() && (*it)->notZombie() && (*it)->stillAlive())
        {
            if (determineOverlap(exit->getX(), exit->getY(), *it)) {
                (*it)->setDead();
                decPeople();
                return true;
            }
        }
    }
    return false;
}

// overlapPenelope: true if the requester overlaps with Penelope
bool StudentWorld::overlapPenelope(const Actor *requester) const
{
    if (determineOverlap(requester->getX(), requester->getY(), m_player))
        return true;
    return false;
}

// awardGoodie: tells the player to award the goodie
void StudentWorld::awardGoodie(char type)
{
    m_player->awardGoodie(type);
}

// infectActors: if any actor that can get infected overlaps with
// 		 the requester, that actor gets infected
void StudentWorld::infectActors(const Actor* requester)
{
    if (overlapPenelope(requester))
        m_player->getInfected();
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->stillAlive()
            && determineOverlap(requester->getX(), requester->getY(), *it))
        {
                if ((*it)->getInfected())
                    playSound(SOUND_CITIZEN_INFECTED);
        }
    }
}

// killActors: if any actor that can get killed overlaps with
// 		 the requester, that actor gets killed
void StudentWorld::killActors(const Actor* requester)
{
    if (overlapPenelope(requester))
        m_player->getKilled();
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it) != requester && (*it)->stillAlive()
            && determineOverlap(requester->getX(), requester->getY(), *it))
        {
            (*it)->getKilled();
        }
    }
}

// createActorAt: creates the requested actor type at (x, y) facing dir
bool StudentWorld::createActorAt(char type, double x, double y, int dir)
{
    if (type == 'f')
    {
        for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
        {
            if ((*it)->canBlock() && !(*it)->canMove() && (*it)->stillAlive())
            {
                if (determineOverlap(x, y, (*it)))
                    return false;
            }
        }
    }
    Actor* toAdd = nullptr;
    switch (type)
    {
        case 's':
            toAdd = new SmartZombie(this, x, y);
            break;
        case 'z':
            toAdd = new DumbZombie(this, x, y);
            break;
        case 'l':
            toAdd = new Landmine(this, x, y);
            break;
        case 'p':
            toAdd = new Pit(this, x, y);
            break;
        case 'f':
            toAdd = new Flame(this, x, y, dir);
            break;
        case 'v':
            toAdd = new Vomit(this, x, y, dir);
            break;
        case 'x':
            toAdd = new VaccineGoodie(this, x, y);
            break;
        default:
            return false;
            break;
    }
    if (toAdd != nullptr) {
        m_actors.push_back(toAdd);
        return true;
    } else
        return false;
}

// detectVomitTarget: true if there is a vomit target at (x,y)
bool StudentWorld::detectVomitTarget(double x, double y) const
{
    if (determineOverlap(x, y, m_player))
        return true;
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->notZombie() && (*it)->canMove() && (*it)->stillAlive())
        {
            if (determineOverlap(x, y, (*it)))
                return true;
        }
    }
    return false;
}

// overlapMover: true if the requester overlaps with a moving actor
bool StudentWorld::overlapMover(const Actor *requester) const
{
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->canMove() && (*it)->stillAlive()
            && determineOverlap(requester->getX(), requester->getY(), *it))
            return true;
    }
    return false;
}

// overlapAny: true if the requester overlaps any actor
bool StudentWorld::overlapAny(double x, double y) const
{
    if (determineOverlap(x, y, m_player))
        return true;
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if (determineOverlap(x, y, *it))
            return true;
    }
    return false;
}

// calculateDistance: returns the Euclidean distance betwen the two coords
double StudentWorld::calculateDistance(double x1, double y1, double x2, double y2) const
{
    double toRad = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
    return (sqrt(toRad));
}

// findNearestHuman: sets otherX and otherY to the coord of the closest human
// 		     to the requester, distance to the distance to that human
bool StudentWorld::findNearestHuman(Actor* requester, double& otherX, double& otherY, double& distance) const 
{
    if (!m_player->stillAlive())
        return false;
    double x = requester->getX();
    double y = requester->getY();
    double compX = m_player->getX();
    double compY = m_player->getY();
    double compDist = calculateDistance(x, y, compX, compY);
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->notZombie() && (*it)->canMove() && (*it)->stillAlive())
        {
            double currDist = calculateDistance(x, y, (*it)->getX(), (*it)->getY());
            if (currDist < compDist)
            {
                compX = (*it)->getX();
                compY = (*it)->getY();
                compDist = currDist;
            }
        }
    }
    otherX = compX;
    otherY = compY;
    distance = compDist;
    return true;
}

// findNearestZombie: sets otherX and otherY to the coord of the closest zombie
// 		      to the requester, distance to the distance to that zombie
bool StudentWorld::findNearestZombie(double x, double y, double &otherX,
                                     double &otherY, double &distance) const

{
    double compX = -1;
    double compY = -1;
    double compDist = VIEW_WIDTH*VIEW_HEIGHT;
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if (!(*it)->notZombie() && (*it)->canMove() && (*it)->stillAlive())
        {
            double currDist = calculateDistance(x, y, (*it)->getX(), (*it)->getY());
            if (currDist < compDist)
            {
                compX = (*it)->getX();
                compY = (*it)->getY();
                compDist = currDist;
            }
        }
    }
    otherX = compX;
    otherY = compY;
    distance = compDist;
    if (otherX == -1)
        return false;
    return true;
}

// distanceToPenelope: sets otherX, otherY, distance to coord of and distance to Penelope
bool StudentWorld::distanceToPenelope(Actor* requester, double& otherX, double& otherY, double& distance) const
{
    if (!m_player->stillAlive())
        return false;
    otherX = m_player->getX();
    otherY = m_player->getY();
    distance = calculateDistance(requester->getX(), requester->getY(), otherX, otherY);
    return true;
}
