#pragma once

//native
#include <algorithm>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <memory>

using namespace std;

class Player : public Camera
{
    protected:
        bool active;

        GameObject* gameObject;

        vec3 headCenter;

        vec3 cameraOffset;
        vec3 modelOffset;
        vec3 modelForward;
        
        void moveAction() override;

        void calcCameraPosition();

        virtual void updateAnimation();
        void calcModelPosition();

        void moveGraphics();

    public:
        Player(Window* window, vec3 playerPos, vec3 cameraForward, float speed = 1);
        Player(Window* window, vec3 playerPos, vec3 cameraForward, GameObject* gameObject, float speed = 1, bool active = false);
        
        virtual void setActive(bool active);
        void setGameObject(GameObject* gameObject);
        void setHeadCenter(vec3 headCenter);
        void setCameraOffset(vec3 cameraOffset);
        void setModelOffset(vec3 modelOffset);
        void setModelForward(vec3 modelForward);

        void removeGameObject();
        bool isActive() const;

        virtual void update(bool events = true) override;

        GameObject* getGameObject() const;

        virtual ~Player();
};
