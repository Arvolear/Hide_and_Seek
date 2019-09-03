#pragma once

#include <mutex>
#include <condition_variable>

#define GLEW_STATIC
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Camera
{
    protected:
        Window* window;

        vec2 prevCoords;
        vec2 sensitivity;

        mat3 horizontalViewRotation;
        mat3 verticalViewRotation;

        float speed;

        vec3 Pos;

        vec3 moveDirection;
        vec3 Forward;
        vec3 Up;
        vec3 Left;

        /* multiplayer */
        bool ready;
        mutable mutex mtx;
        mutable condition_variable cv;
        
        virtual void lookAction();
        virtual void moveAction() = 0;

    public:
        Camera(Window* window, vec3 cameraPos, vec3 cameraForward, float speed = 1);

        void resetPrevCoords();
        void setPosition(float worldX, float worldY, float worldZ);
        void setUp(vec3 Up);
        void setForward(vec3 forward);
        void setMoveDirection(vec3 moveDirection);
        void setSpeed(float speed);
        void setSensitivity(vec2 sens);

        vec2 getWindowSize() const;
        vec3 getPosition() const;
        vec3 getForward() const;
        vec3 getLeft() const;
        vec3 getUp() const;
        vec3 getMoveDirection() const;
        
        float getSpeed() const;
        vec2 getSensitivity() const;
        
        mat3 getHorizontalViewRotation() const;
        mat3 getVerticalViewRotation() const;
        mat4 getView() const;

        virtual void update(bool events = true) = 0;

        virtual ~Camera();
};
