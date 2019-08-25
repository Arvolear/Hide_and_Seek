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
        deque < Weapon* > weapons;
        
        void weaponAction();

        void nextWeapon();
        
        void updateWeapon();

    public:
        Soldier(Window* window, vec3 playerPos, vec3 cameraForward, float speed = 1);
        Soldier(Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed = 1, bool active = false);

        void setActive(bool active) override;

        void drop();
        void pick(Weapon* weapon);
        void pick();

        void update(bool events = true) override;
        
        ~Soldier();
};
