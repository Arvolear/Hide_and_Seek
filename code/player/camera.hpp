#pragma once

#define GLEW_STATIC
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Camera
{
    protected:
        Window* window;

        vec2 prevCoords;
        vec2 sensitivity;

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

        vec2 getWindowSize() const;
        virtual vec3 getPosition() const;
        virtual vec3 getForward() const;
        virtual vec3 getLeft() const;
        virtual vec3 getUp() const;

        mat4 getView() const;

        virtual void update();

        virtual ~Camera();
};
