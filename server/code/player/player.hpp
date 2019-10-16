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
        static set < int > globalIDs;

        bool connected;
        int id;
        
        float speed;

        PhysicsObject* physicsObject;

        btVector3 moveDirection;
        
    public:
        Player(int id, float speed = 1);
        Player(int id, PhysicsObject* physicsObject, float speed = 1);

        void setConnected(bool connected);
        void setPhysicsObject(PhysicsObject* player);
        void removePhysicsObject();

        void setMoveDirection(btVector3 moveDirection);
        
        virtual void update();

        bool isConnected() const;
        PhysicsObject* getPhysicsObject() const;
        btVector3 getMoveDirection() const;
        float getSpeed() const;
        int getID() const;

        virtual ~Player();
};
