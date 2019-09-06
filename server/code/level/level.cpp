#include "../global/convert.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"

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

map < string, PhysicsObject* > Level::getPhysicsObjects() const
{
    return physicsObjects;
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
        set < btRigidBody* > touching;

        touching = physicsWorld->getTouchingWith(players[i]->getPhysicsObject()->getRigidBody());

        for (auto& j: touching)
        {
            PhysicsObject* physicsObject = static_cast < PhysicsObject* >(j->getUserPointer());

            physicsObject->setOwnerID(i);
        }
    }
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
