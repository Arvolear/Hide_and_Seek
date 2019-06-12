#pragma once

//native
#include <algorithm>
#include <vector>
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

        GameObject* player;

        RayTracer* rayTracer;
        
        bool isGroundStanding();
        void jump();
        
        void moveAction() override;

        void moveGround();
        void moveAir();
        void speedHackControl();

        void movePhysics();

    public:
        Player(Window* window, vec3 playerPos, vec3 cameraForward, float speed = 5);
        Player(Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed = 5);

        void setRayTracer(RayTracer* tracer);
        void setGameObject(GameObject* player);
        void removeGameObject();

        virtual void update() override;

        GameObject* getGameObject() const;

        virtual ~Player();
};
