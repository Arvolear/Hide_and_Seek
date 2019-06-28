#pragma once

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
        
        virtual void lookAction();
        virtual void moveAction();

    public:
        Camera(Window* window, vec3 cameraPos, vec3 cameraForward, float speed = 1);

        void resetPrevCoords();
        void setPosition(float worldX, float worldY, float worldZ);
        void setUp(vec3 Up);
        void setSpeed(float speed);
        void setSensitivity(vec2 sens);

        vec2 getWindowSize() const;
        vec3 getPosition() const;
        vec3 getForward() const;
        vec3 getLeft() const;
        vec3 getUp() const;
        
        float getSpeed() const;
        vec2 getSensitivity() const;
        
        mat3 getHorizontalViewRotation() const;
        mat3 getVerticalViewRotation() const;
        mat4 getView() const;

        virtual void update();

        virtual ~Camera();
};
