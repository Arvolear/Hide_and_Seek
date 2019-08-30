#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "raytracer.hpp"

RayTracer::RayTracer(Camera* camera, mat4 &projection)
{
    this->camera = camera;
    this->projection = projection;
}

void RayTracer::setCamera(Camera* camera)
{
    this->camera = camera;
}

void RayTracer::setProjection(mat4 &projection)
{
    this->projection = projection;
}        

vec3 RayTracer::getPickingRay(double posx, double posy) const
{
    if (!camera)
    {
        return vec3(0);
    }

    vec2 windowSize = camera->getWindowSize();

    vec2 mouseClip = vec2(posx * 2 / windowSize.x - 1, 1 - posy * 2 / windowSize.y); // calculate mouse clip coords [-1;1] from window coords

    vec3 cameraWorld = camera->getPosition();
    mat4 cameraTranslate = camera->getView(); // get view matrix

    vec4 mouseWorld = inverse(cameraTranslate) * inverse(projection) * vec4(mouseClip, 0, 1); // calculate mouse position on view near plane 

    mouseWorld /= mouseWorld.w; // normalize

    vec3 ray = normalize(vec3(mouseWorld) - cameraWorld); // calculate ray direction
    
    //cout << "Ray " << ray.x << ' ' << ray.y << ' ' << ray.z << endl << endl;

    return ray;
}
        
RayTracer::~RayTracer() {}
