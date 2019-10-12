#include "../global/convert.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "levelloader.hpp"
#include "level.hpp"

Level::Level(World* physicsWorld)
{
    this->physicsWorld = physicsWorld;

    levelLoader = new LevelLoader(physicsWorld);

    levelName = "";
}

void Level::loadLevel(string level)
{
    levelName = level;

    levelLoader->loadLevel(path("levels/test1"));

    /*** GET LOADED DATA ***/
    levelLoader->getPlayersData(players);
    levelLoader->getPhysicsObjectsData(physicsObjects);
}
        
void Level::addPhysicsObject(PhysicsObject* physicsObject)
{
    if (physicsObjects.find(physicsObject->getName()) == physicsObjects.end())
    {
        physicsObjects.insert({physicsObject->getName(), physicsObject}); 
    }
}

PhysicsObject* Level::getPhysicsObject(string name) const
{
    if (physicsObjects.find(name) != physicsObjects.end())
    {
        return physicsObjects.find(name)->second;
    }

    return nullptr;
}

void Level::removePhysicsObject(PhysicsObject* physicsObject)
{
    if (physicsObjects.find(physicsObject->getName()) != physicsObjects.end())
    {
        auto it = physicsObjects.find(physicsObject->getName());

        delete it->second;
        physicsObjects.erase(it);
    }
}
        
void Level::removePhysicsObject(string name)
{
    if (physicsObjects.find(name) != physicsObjects.end())
    {
        auto it = physicsObjects.find(name);

        delete it->second;
        physicsObjects.erase(it);
    }
}

void Level::update()
{
    /* update owners */
    for (size_t i = 0; i < players.size(); i++)
    {
        if (!players[i]->isConnected())
        {
            continue;
        }

        set < btRigidBody* > touching;

        touching = physicsWorld->getTouchingWith(players[i]->getPhysicsObject()->getRigidBody());

        for (auto& j: touching)
        {
            PhysicsObject* physicsObject = static_cast < PhysicsObject* >(j->getUserPointer());

            /* if not picked || player object */
            if (!physicsObject->getUserPointer())
            {
                physicsObject->setOwnerID(i);
            }
        }
    }
}
        
void Level::clearNoPlayersAndTheirWeaponsOwner(int owner)
{
    map < string, PhysicsObject* > tmp = getNoPlayersAndTheirWeaponsPhysicsObjects();

    for (auto& it : tmp)
    {
        if (it.second->getOwnerID() == owner)
        {
            it.second->getRigidBody()->forceActivationState(ACTIVE_TAG);
            it.second->getRigidBody()->applyCentralImpulse(it.second->getRigidBody()->getGravity());
            
            it.second->setOwnerID(-1);
        }
    }
}

map < string, PhysicsObject* > Level::getPhysicsObjects() const
{
    return physicsObjects;
}

map < string, PhysicsObject* > Level::getNoPlayersPhysicsObjects() const
{
    map < string, PhysicsObject* > tmp = physicsObjects;

    for (size_t i = 0; i < players.size(); i++)
    {
        tmp.erase(players[i]->getPhysicsObject()->getName());
    }

    return tmp;
}
        
map < string, PhysicsObject* > Level::getNoPlayersAndTheirWeaponsPhysicsObjects() const
{
    map < string, PhysicsObject* > tmp = physicsObjects;

    for (size_t i = 0; i < players.size(); i++)
    {
        tmp.erase(players[i]->getPhysicsObject()->getName());

        Soldier* soldier = dynamic_cast < Soldier* >(players[i]);

        if (soldier)
        {
            deque < Weapon* > weapons = soldier->getWeapons();
            for (size_t j = 0; j < weapons.size(); j++)
            {
                tmp.erase(weapons[j]->getName());
            }
        }
    }

    return tmp;
}


Player* Level::getPlayer(int id) const
{
    if (id >= 0 && id < (int)players.size())
    {
        return players[id];
    }

    return nullptr;
}
        
vector < Player* > Level::getPlayers() const
{
    return players;
}

vector < Player* > Level::getPlayersExcept(int index) const
{
    vector < Player* > tmp = players;
    tmp.erase(tmp.begin() + index);

    return tmp;
}

string Level::getLevelName() const
{
    return levelName;
}

Level::~Level()
{
    delete levelLoader;

    for (auto& i : physicsObjects)
    {
        delete i.second;
    }

    for (size_t i = 0; i < players.size(); i++)
    {
        delete players[i];
    }
}
