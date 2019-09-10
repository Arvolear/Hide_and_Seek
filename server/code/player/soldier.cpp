#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "player.hpp"
#include "soldier.hpp"

Soldier::Soldier(float speed) : Player(speed) {}

Soldier::Soldier(PhysicsObject* physicsObject, float speed) : Player(physicsObject, speed) {}

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

    new_weapons.push_front(weapon);
    
    weapon->setOwnerID(physicsObject->getOwnerID());
    weapon->setUserPointer(this);
    weapon->setCollidable(false);
}

void Soldier::drop(btScalar* model)
{
    if (!weapons.empty())
    {
        old_weapons.push_back(weapons[0]);
        
        weapons[0]->setTransform(model);
        weapons[0]->setUserPointer(nullptr);
        weapons[0]->setCollidable(true);

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
