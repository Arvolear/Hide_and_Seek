#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "camera.hpp"

Camera::Camera(Window* window, vec3 cameraPos, vec3 cameraForward, float speed)
{
    this->window = window;

    sensitivity.x = 2 / window->getSize().x;
    sensitivity.y = (2 / (window->getSize().x / window->getSize().y)) / window->getSize().y;

    horizontalViewRotation = mat3(1.0);
    verticalViewRotation = mat3(1.0);

    Pos = cameraPos;
    Forward = normalize(cameraForward);
    Up = vec3(0, 1, 0);

    Left = normalize(cross(Up, Forward));

    this->speed = speed;

    prevCoords = window->getSize() / 2.0f;

    ready = true;
}

void Camera::lookAction()
{
    horizontalViewRotation = mat3(1.0);
    verticalViewRotation = mat3(1.0);

    if (!window->isMouseMoved())
    {
        return;
    }
        
    vec2 pos = window->getMousePosition();

    float xoffset = pos.x - prevCoords.x; 
    float yoffset = pos.y - prevCoords.y;

    xoffset *= sensitivity.x;
    yoffset *= sensitivity.y;

    /* around Up */
    Forward = mat3(rotate(-xoffset, normalize(Up))) * Forward;
    horizontalViewRotation *= mat3(rotate(-xoffset, normalize(Up)));

    Left = normalize(cross(Up, Forward));
       
    /* fix camera jumps */
    vec3 ForwardTmp = Forward;

    /* around Left */
    Forward = mat3(rotate(yoffset, normalize(Left))) * Forward;
    verticalViewRotation *= mat3(rotate(yoffset, normalize(Left)));
        
    if (dot(cross(Up, Forward), Left) < 0)
    {
        Forward = ForwardTmp;
        verticalViewRotation = mat3(1.0);
    }
    
    Forward = normalize(Forward);

    prevCoords = pos;
}
        
void Camera::resetViewRotation()
{
    horizontalViewRotation = verticalViewRotation = mat3(1.0);
}

void Camera::resetPrevCoords()
{
    prevCoords = window->getMousePosition();
}

void Camera::setPosition(float worldX, float worldY, float worldZ)
{
    Pos = vec3(worldX, worldY, worldZ);
}
        
void Camera::setUp(vec3 Up)
{
    this->Up = normalize(Up);
    this->Left = normalize(cross(Up, Forward));
}
        
void Camera::setForward(vec3 forward)
{
    this->Forward = normalize(forward);
    this->Left = normalize(cross(Up, Forward));
}

void Camera::setMoveDirection(vec3 moveDirection)
{
    this->moveDirection = moveDirection;
}
        
void Camera::setSpeed(float speed)
{
    this->speed = speed;
}
        
void Camera::setSensitivity(vec2 sens)
{
    sensitivity.x = sens.x / window->getSize().x;
    sensitivity.y = (sens.y / (window->getSize().x / window->getSize().y)) / window->getSize().y; 
}

vec2 Camera::getWindowSize() const
{
    return window->getSize();
}

vec3 Camera::getPosition() const
{
    return Pos;
}

vec3 Camera::getForward() const
{
    return Forward;
}

vec3 Camera::getLeft() const
{
    return Left;
}

vec3 Camera::getUp() const
{
    return Up;
}

vec3 Camera::getMoveDirection() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return moveDirection;
}

float Camera::getSpeed() const
{
    return speed; 
}

vec2 Camera::getSensitivity() const
{
    return sensitivity;
}

mat3 Camera::getHorizontalViewRotation() const
{
    return horizontalViewRotation;
}

mat3 Camera::getVerticalViewRotation() const
{
    return verticalViewRotation;
}

mat4 Camera::getView() const
{
    return lookAt(Pos, Pos + Forward, Up);
}

Camera::~Camera(){}
