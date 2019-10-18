#pragma once

#include <string>
#include <vector>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class Weapon : public PhysicsObject
{
    private:
        int storageBullets;
        int magazineSize;
        int magazineBullets;

        float shotSpeed;
        float shotPower;

    public:
        Weapon(string name, World* physicsWorld);
        Weapon(string name, World* physicsWorld, btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation = btQuaternion(btVector3(0, 0, 1), 0));
        Weapon(string name, World* physicsWorld, CompoundShape* shape, float mass, btVector3 position, btQuaternion rotation = btQuaternion(btVector3(0, 0, 1), 0));

        void setStorageBullets(int storageBullets);
        void setMagazineSize(int magazineSize);
        void setMagazineBullets(int magazineBullets);

        void setShotSpeed(float shotSpeed);
        void setShotPower(float shotPower);

        int getStorageBullets() const;
        int getMagazineSize() const;
        int getMagazineBullets() const;

        float getShotSpeed() const;
        float getShotPower() const;

        ~Weapon();
};
