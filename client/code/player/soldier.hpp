#pragma once

//native
#include <deque>
#include <stdexcept>
#include <memory>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class Soldier : public Player
{
    private:
        int health;

        deque < Weapon* > weapons;

        pair < vec3, vec3 > pickRay;

        map < string, vector < pair < vec3, vec3 > > > fireInfo;

        bool dropTo;
        
        void weaponAction();

        void nextWeapon();
        
        void updateWeapon();
        
        void drop();
        void pick();
        void fire();

    public:
        Soldier(int id, Window* window, vec3 playerPos, vec3 cameraForward, float speed = 1);
        Soldier(int id, Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed = 1, bool active = false);

        void setActive(bool active) override;
        void setConnected(bool connected) override;

        void setHealth(int health);
        
        void drop(Weapon* weapon);
        void pick(Weapon* weapon);

        deque < Weapon* > getWeapons() const;
        Weapon* getWeapon(int index) const;

        pair < vec3, vec3 > getPickRay();
        bool isDrop();
        map < string, vector < pair < vec3, vec3 > > > getFire();

        int getHealth() const;

        void update(bool events = true) override;

        ~Soldier();
};
