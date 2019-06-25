#pragma once

#include <string>
#include <vector>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Weapon : public GameObject
{
    protected:
        Window* window;

        vec3 offset;
        quat twist;

        float lastShotTime;

        int bulletsLeft;
        int magazineSize;
        int magazineBulletsLeft;

        vector < vec3 > recoil;

        float shotSpeed;
        float shotPower;

    public:
        Weapon(Window* window, string name);
        Weapon(Window* window, vec3 offset, quat twist, string name, bool visible, string path, PhysicsObject* physicsObject, ViewFrustum* viewFrustum);

        void setOffset(vec3 offset);
        void setTwist(quat twist);

        void reload();
        void drop();
        void pick();
        void choose();
        
        /* perform dynamic cast to differ pistols from rifles? 
         * or take info from the window?
         * */
        void fire();

        vec3 getOffset() const;
        quat getTwist() const;

        void updatePosition(vec3 center, vec3 left, vec3 up);

        ~Weapon();
};
