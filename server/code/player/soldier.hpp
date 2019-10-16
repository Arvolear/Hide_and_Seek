#pragma once

//native
#include <algorithm>
#include <deque>
#include <cmath>
#include <stdexcept>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class Soldier : public Player
{
    private:
        deque < Weapon* > weapons;
        deque < Weapon* > new_weapons;
        deque < Weapon* > old_weapons;

    public:
        Soldier(int id, float speed = 1);
        Soldier(int id, PhysicsObject* physicsObject, float speed = 1);

        void pick(Weapon* weapon);
        void drop(btScalar* model);
        
        void next();
        void prev();

        void newToWeapons();
        void oldToNothing();

        deque < Weapon* > getWeapons() const;
        deque < Weapon* > getNewWeapons() const;
        deque < Weapon* > getOldWeapons() const;
        
        ~Soldier();
};
