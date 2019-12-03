#include "../global/globaluse.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "player.hpp"
#include "soldier.hpp"

Soldier::Soldier(int maxHealth, int id, float speed) : Player(id, speed) 
{
    if (maxHealth <= 0)
    {
        throw(runtime_error("ERROR::Soldier max health <= 0"));
    }

    this->maxHealth = this->health = maxHealth;
    deathTime = -1;
    respawn = false;
}

Soldier::Soldier(int maxHealth, int id, PhysicsObject* physicsObject, float speed) : Player(id, physicsObject, speed) 
{
    if (maxHealth <= 0)
    {
        throw(runtime_error("ERROR::Soldier max health <= 0"));
    }

    this->maxHealth = this->health = maxHealth;
    deathTime = -1;
    respawn = false;
}

void Soldier::setConnected(bool connected)
{
    this->connected = connected;

    if (connected)
    {
        if (physicsObject)
        {
            physicsObject->setCollidable(true);
            physicsObject->setStatic(false);
            physicsObject->setKinematic(false);
        }
    }
    else
    {
        respawn = false;
        deathTime = -1;

        if (physicsObject)
        {
            physicsObject->setCollidable(false);
            physicsObject->setStatic(true);
            physicsObject->setKinematic(true);
        }

        while (!weapons.empty())
        {
            drop(physicsObject->getTransform());
        }

        oldToNothing();
    }
}

void Soldier::setMaxHealth(int maxHealth)
{
    if (maxHealth <= 0)
    {
        throw(runtime_error("ERROR::Soldier max health <= 0"));
    }

    this->maxHealth = maxHealth;

    health = std::min(health, maxHealth);
}

void Soldier::setHealth(int health)
{
    unique_lock < mutex > lk(mtx);
    ready = false;

    if (this->health && !health)
    {
        if (physicsObject)
        {
            physicsObject->setCollidable(false);
            physicsObject->setStatic(true);
            physicsObject->setKinematic(true);
        }
        
        deathTime = global.getTime();
    }
    else if (!this->health && health)
    {
        if (physicsObject)
        {
            physicsObject->setCollidable(true);
            physicsObject->setStatic(false);
            physicsObject->setKinematic(false);
        }
        
        deathTime = -1;
    }
    
    this->health = std::min(health, maxHealth);

    ready = true;
    cv.notify_all();
}

void Soldier::damage(int dmg)
{
    unique_lock < mutex > lk(mtx);
    ready = false;

    health = std::max(health - dmg, 0);
       
    if (!health)
    {
        if (physicsObject)
        {
            physicsObject->setCollidable(false);
            physicsObject->setStatic(true);
            physicsObject->setKinematic(true);
        }
    
        deathTime = global.getTime();
    }

    ready = true;
    cv.notify_all();
}

void Soldier::setDeathTime(int deathTime)
{
    this->deathTime = deathTime;
}
        
void Soldier::setRespawn(bool respawn)
{
    this->respawn = respawn;
}

void Soldier::pick(Weapon* weapon)
{
    if (find(new_weapons.begin(), new_weapons.end(), weapon) != new_weapons.end())
    {
        return;
    }

    if (find(weapons.begin(), weapons.end(), weapon) != weapons.end())
    {
        return;
    }

    weapon->setOwnerID(physicsObject->getOwnerID());
    weapon->setUserPointer(this);
    weapon->setCollidable(false);
    physicsObject->setKinematic(true);

    new_weapons.push_front(weapon);
}

void Soldier::drop(btScalar* model)
{
    if (!weapons.empty())
    {
        weapons[0]->setTransform(model);
        weapons[0]->setUserPointer(nullptr);
        weapons[0]->setCollidable(true);
        physicsObject->setKinematic(false);

        old_weapons.push_back(weapons[0]); 
        weapons.pop_front();
    }
}

void Soldier::newToWeapons()
{
    while (!new_weapons.empty())
    {
        weapons.push_front(new_weapons[new_weapons.size() - 1]);
        new_weapons.pop_back();
    }
}

void Soldier::oldToNothing()
{
    while (!old_weapons.empty())
    {
        old_weapons.pop_front();
    }
}

int Soldier::getMaxHealth() const
{
    return maxHealth;
}

int Soldier::getHealth() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return health;
}
        
bool Soldier::isRespawn() const
{
    return respawn;
}

int Soldier::getDeathTime() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return deathTime;
}

deque < Weapon* > Soldier::getWeapons() const
{
    return weapons;
}

deque < Weapon* > Soldier::getNewWeapons() const
{
    return new_weapons;
}

deque < Weapon* > Soldier::getOldWeapons() const
{
    return old_weapons;
}

Soldier::~Soldier() {}
