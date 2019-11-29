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
        int maxHealth;
        int health;

        deque < Weapon* > weapons;
        deque < Weapon* > new_weapons;
        deque < Weapon* > old_weapons;

    public:
        Soldier(int maxHealth, int id, float speed = 1);
        Soldier(int maxHealth, int id, PhysicsObject* physicsObject, float speed = 1);

        void setConnected(bool connected) override;

        void setMaxHealth(int maxHealth);
        void setHealth(int health);
        void damage(int dmg);

        void pick(Weapon* weapon);
        void drop(btScalar* model);
        
        void newToWeapons();
        void oldToNothing();

        int getMaxHealth() const;
        int getHealth() const;

        deque < Weapon* > getWeapons() const;
        deque < Weapon* > getNewWeapons() const;
        deque < Weapon* > getOldWeapons() const;
        
        ~Soldier();
};
