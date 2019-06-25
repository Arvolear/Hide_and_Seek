#pragma once

//native
#include <algorithm>
#include <vector>
#include <cmath>
#include <stdexcept>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class Player : public Camera
{
    protected:
        bool active;
        bool jumpAllowed;
        bool speedLock;

        RayTracer* rayTracer;
        
        GameObject* player;

        vec3 cameraOffset;
        vec3 modelOffset;
        vec3 modelForward;
        
        bool isGroundStanding();
        void jump();
        
        void moveAction() override;

        void calcCameraPosition();

        void moveGround();
        void moveAir();
        void speedHackControl();

        virtual void updateAnimation();
        void calcModelPosition();

        void movePhysics();

    public:
        Player(Window* window, vec3 playerPos, vec3 cameraForward, float speed = 1);
        Player(Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed = 1, bool active = false);
        
        virtual void setActive(bool active);
        void setRayTracer(RayTracer* tracer);
        void setGameObject(GameObject* player);
        void setCameraOffset(vec3 cameraOffset);
        void setModelOffset(vec3 modelOffset);

        void removeGameObject();
        bool isActive() const;

        virtual void update() override;

        GameObject* getGameObject() const;
        vec3 getCameraOffset() const;
        vec3 getModelOffset() const;

        virtual ~Player();
};
