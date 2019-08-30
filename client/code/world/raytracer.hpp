#pragma once

//openGL
#include <glm/glm.hpp>

using namespace glm;

class RayTracer
{
    private:
        Camera* camera;
        mat4 projection;

    public:
        RayTracer(Camera* camera, mat4 &projection);

        void setCamera(Camera* camera);
        void setProjection(mat4 &projection);

        vec3 getPickingRay(double posx, double posy) const;

        ~RayTracer();
};
