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

        int storageBullets;
        int magazineSize;
        int magazineBullets;

        vector < vec3 > recoil;

        /* animaiton duration? */
        float shotSpeed;
        float shotPower;

    public:
        Weapon(Window* window, string name);

        void setOffset(vec3 offset);
        void setTwist(quat twist);

        void setTotalBullets(int totalBullets);
        void setMagazineSize(int magazineSize);
        void setMagazineBullets(int magazineBullets);

        void setShotSpeed(float shotSpeed);
        void setShotPower(float shotPower);

        void reload();
        void drop();
        void pick();
        void choose();
        
        bool fire();

        vec3 getOffset() const;
        quat getTwist() const;
        
        int getStorageBullets() const;
        int getMagazineSize() const;
        int getMagazineBullets() const;

        float getShotSpeed() const;
        float getShotPower() const;

        void updateStatus();
        void updatePosition(vec3 center, vec3 left, vec3 up);

        ~Weapon();
};
