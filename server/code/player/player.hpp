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
        bool active;
        bool jumpAllowed;
        bool speedLock;

        float speed;
        btVector3 Forward;
        btVector3 Left;
        btVector3 Up;

        string move;
        btVector3 moveDirection;

        btVector3 headCenter;
        
        RayTracer* rayTracer;
        
        PhysicsObject* physicsObject;
        
        bool isGroundStanding();
        void jump();
        
        void moveAction();
        
        void moveGround();
        void moveAir();
        void speedHackControl();

        void calcCameraPosition();

        void movePhysics();

    public:
        Player(float speed = 1);
        Player(RayTracer* tracer, PhysicsObject* physicsObject, float speed = 1);
        void setRayTracer(RayTracer* tracer);
        void setPhysicsObject(PhysicsObject* player);
        void setForward(btVector3 Forward);
        void setLeft(btVector3 Left);
        void setUp(btVector3 Up);
        void setMovement(string move);
        
        void removePhysicsObject();
        
        virtual void update();

        PhysicsObject* getPhysicsObject() const;
        btVector3 getHeadCenter() const;

        virtual ~Player();
};
