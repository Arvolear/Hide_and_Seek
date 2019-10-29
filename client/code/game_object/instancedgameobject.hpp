#pragma once

#include <vector>
#include <map>

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
        vec3 aabbMin, aabbMax;
        
        map < int, float > radiuses;
        map < int, vec2 > leftTops;
        map < int, vec2 > rightBottoms;
        map < int, vector < vector < vec2 > > > withouts;
        
        vector < mat4 > transformations;
        PoissonDisk* poissonDisk;

    public:
        InstancedGameObject(string name);
        void createBoundSphere() override;

        void setRadius(int index, float radius);
        void setBorders(int index, vec2 leftTop, vec2 rightBottom);
        void setWithoutPolygons(int index, vector < vector < vec2 > > without);

        void genInstances();

        void render(Shader* shader, bool viewCull = true) override;

        ~InstancedGameObject();        
};
