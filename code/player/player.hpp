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
        bool jumpAllowed;
        bool speedLock;

        RayTracer* rayTracer;
        
        GameObject* player;
        vec3 modelForward;

        vec2 offset;
        
        bool isGroundStanding();
        void jump();
        
        void moveAction() override;

        void calcCameraPosition();

        void moveGround();
        void moveAir();
        void speedHackControl();

        void updateAnimation();
        void rotateModel();

        void movePhysics();

    public:
        Player(Window* window, vec3 playerPos, vec3 cameraForward, float speed = 1);
        Player(Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed = 1);

        void setRayTracer(RayTracer* tracer);
        void setGameObject(GameObject* player);
        void setOffset(vec2 offset);

        void removeGameObject();

        virtual void update() override;

        GameObject* getGameObject() const;
        vec2 getOffset() const;

        virtual ~Player();
};
