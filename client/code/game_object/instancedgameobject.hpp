#pragma once

#include <vector> 

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

using namespace std;
using namespace glm;

class InstancedGameObject : public GameObject
{
    private:
        float radius;
        vec2 leftTop;
        vec2 rightBottom;
        
        vector < mat4 > transformations;
        PoissonDisk* poissonDisk;

    public:
        InstancedGameObject(string name);

        void setRadius(float radius);
        void setBorders(vec2 leftTop, vec2 rightBottom);

        void genInstances();

        void render(Shader* shader, bool cull = false) override;

        ~InstancedGameObject();        
};
