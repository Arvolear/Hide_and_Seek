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
    weapon->setCollidable(false);
}

void Soldier::drop(btVector3 direction)
{
    int power = 3;

    if (!weapons.empty())
    {
        weapons[0]->setCollidable(true);
        weapons[0]->getRigidBody()->applyCentralImpulse(direction * power);
        weapons.pop_front();
    }
}

void Soldier::newToOldWeapons()
{
    while (!new_weapons.empty())
    {
        weapons.push_front(new_weapons[new_weapons.size() - 1]);
        new_weapons.pop_back();
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

Soldier::~Soldier() {}
