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
    float distance(vec3 point) const;
};

class ViewFrustum
{
    private:
        vector < Plane > frustum;

        mat4 view;
        mat4 projection;

    public:
        ViewFrustum();

        void updateFrustum(mat4 view, mat4 projection);

        bool isPointInFrustum(vec3 point) const;
        bool isSphereInFrustum(vec3 center, float radius) const;

        mat4 getView() const;
        mat4 getProjection() const;

        ~ViewFrustum();
};
