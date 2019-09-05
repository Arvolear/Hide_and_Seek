#include "openglmotionstate.hpp"
#include "physicsobject.hpp"
#include "weapon.hpp"

Weapon::Weapon(string name, btDynamicsWorld* world) : PhysicsObject(name, world) 
{
    storageBullets = 0;
    magazineSize = 0;
    magazineBullets = 0;

    shotSpeed = 0;
    shotPower = 0;
}

Weapon::Weapon(string name, btDynamicsWorld* world, btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation) : PhysicsObject(name, world, shape, mass, position, rotation) 
{
    storageBullets = 0;
    magazineSize = 0;
    magazineBullets = 0;

    shotSpeed = 0;
    shotPower = 0;
}

Weapon::Weapon(string name, btDynamicsWorld* world, CompoundShape* shape, float mass, btVector3 position, btQuaternion rotation) : PhysicsObject(name, world, shape, mass, position, rotation)
{
    storageBullets = 0;
    magazineSize = 0;
    magazineBullets = 0;

    shotSpeed = 0;
    shotPower = 0;
}

void Weapon::setStorageBullets(int storageBullets)
{
    this->storageBullets = storageBullets;
}

void Weapon::setMagazineSize(int magazineSize)
{
    this->magazineSize = magazineSize;

    magazineBullets = std::min(magazineSize, magazineBullets);
}

void Weapon::setMagazineBullets(int magazineBullets)
{
    this->magazineBullets = std::min(magazineBullets, magazineSize);
}

void Weapon::setShotSpeed(float shotSpeed)
{
    this->shotSpeed = shotSpeed;
}

void Weapon::setShotPower(float shotPower)
{
    this->shotPower = shotPower;
}

int Weapon::getStorageBullets() const
{
    return storageBullets;
}

int Weapon::getMagazineSize() const
{
    return magazineSize;
}

int Weapon::getMagazineBullets() const
{
    return magazineBullets;
}

float Weapon::getShotSpeed() const
{
    return shotSpeed;
}

float Weapon::getShotPower() const
{
    return shotPower;
}

Weapon::~Weapon() {}
