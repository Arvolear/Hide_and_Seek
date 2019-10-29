#include "../shader/shader.hpp"

#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"
#include "boundsphere.hpp"

BoundSphere::BoundSphere(vector < Mesh* > &meshes)
{
    transform = mat4(1.0);
    centerAndBound = {vec3(0), vec3(0)};

    for (size_t i = 0; i < meshes.size(); i++)
    {
        vector < Mesh::Vertex > vertices = meshes[i]->getVertices();

        for (size_t j = 0; j < vertices.size(); j++)
        {
            allVertices.push_back(vertices[j].position);
        }
    }
}

BoundSphere::BoundSphere(vector < vec3 > &points)
{
    transform = mat4(1.0);
    centerAndBound = {vec3(0), vec3(0)};

    allVertices = points;
}

vec3 BoundSphere::findFurthest(vec3 point)
{
    vec3 curFur = allVertices[0];

    for (size_t i = 0; i < allVertices.size(); i++)
    {
        float radius = distance(point, curFur);
        float newRadius = distance(point, allVertices[i]);

        if (newRadius > radius)
        {
            curFur = allVertices[i];
        }
    }

    return curFur;
}

vec3 BoundSphere::findCenter()
{
    vec3 minAABB = allVertices[0];
    vec3 maxAABB = allVertices[0];

    for (size_t i = 0; i < allVertices.size(); i++)
    {
        /* MIN */
        if (allVertices[i].x < minAABB.x)
        {
            minAABB.x = allVertices[i].x;
        }

        if (allVertices[i].y < minAABB.y)
        {
            minAABB.y = allVertices[i].y;
        }

        if (allVertices[i].z < minAABB.z)
        {
            minAABB.z = allVertices[i].z;
        }

        /* MAX */
        if (allVertices[i].x > maxAABB.x)
        {
            maxAABB.x = allVertices[i].x;
        }

        if (allVertices[i].y > maxAABB.y)
        {
            maxAABB.y = allVertices[i].y;
        }

        if (allVertices[i].z > maxAABB.z)
        {
            maxAABB.z = allVertices[i].z;
        }
    }

    return (minAABB + maxAABB) / 2.0f; 
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
