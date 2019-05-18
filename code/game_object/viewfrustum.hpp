#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

struct Plane
{
    vec3 norm;
    float d;

    void setData(float a, float b, float c, float d);
    float distance(vec3 point);
};

class ViewFrustum
{
    private:
        vector < Plane > frustum;

        mat4 view;
        mat4 projection;

    public:
        ViewFrustum();

        void extractFrustum(mat4 &view, mat4 &projection);

        bool isPointInFrustum(vec3 point);
        bool isSphereInFrustum(vec3 center, float radius);

        mat4 getView();
        mat4 getProjection();

        ~ViewFrustum();
};
