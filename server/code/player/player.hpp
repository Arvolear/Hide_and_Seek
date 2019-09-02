#pragma once

//native
#include <algorithm>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <memory>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class Player
{
    protected:
        float speed;

        PhysicsObject* physicsObject;
        
    public:
        Player(float speed = 1);
        Player(PhysicsObject* physicsObject, float speed = 1);

        void setPhysicsObject(PhysicsObject* player);
        void removePhysicsObject();
        
        virtual void update();

        PhysicsObject* getPhysicsObject() const;
        float getSpeed() const;

        virtual ~Player();
};
