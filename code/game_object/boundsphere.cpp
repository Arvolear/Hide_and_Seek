#include "../shader/shader.hpp"

#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"
#include "boundsphere.hpp"

BoundSphere::BoundSphere(vector < Mesh* > &meshes)
{
    transform = mat4(1);
    centerAndBound = {vec3(0), vec3(0)};

    for (size_t i = 0; i < meshes.size(); i++)
    {
        allVertices.push_back(meshes[i]->getVertices());
    }
}

vec3 BoundSphere::findFurthest(vec3 point)
{
    vec3 curFur = allVertices[0][0].position;

    for (size_t i = 0; i < allVertices.size(); i++)
    {
        for (size_t j = 0; j < allVertices[i].size(); j++)
        {
            float radius = distance(point, curFur);
            float newRadius = distance(point, allVertices[i][j].position);

            if (newRadius > radius)
            {
                curFur = allVertices[i][j].position;
            }
        }
    }

    return curFur;
}

vec3 BoundSphere::findCenter()
{
    vec3 average = vec3(0);
    float counter = 0;

    for (size_t i = 0; i < allVertices.size(); i++)
    {
        for (size_t j = 0; j < allVertices[i].size(); j++)
        {
            average += allVertices[i][j].position;
            counter++;
        }
    }

    return average / counter; 
}

void BoundSphere::construct()
{
    centerAndBound.first = findCenter();
    centerAndBound.second = findFurthest(centerAndBound.first);
}

void BoundSphere::applyTransform(mat4 &transform)
{
    this->transform = transform;
}

vec3 BoundSphere::getCenter() const
{
    return centerAndBound.first;
}

float BoundSphere::getRadius() const
{
    return distance(getCenter(), centerAndBound.second);
}

vec3 BoundSphere::getTransformedCenter() const
{
    return vec3(transform * vec4(centerAndBound.first, 1.0));
}

float BoundSphere::getTransformedRadius() const
{
    return distance(getTransformedCenter(), vec3(transform * vec4(centerAndBound.second, 1.0)));
}

BoundSphere::~BoundSphere(){}
