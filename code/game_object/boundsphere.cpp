#include "../global/shader.hpp"
#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"
#include "boundsphere.hpp"

BoundSphere::BoundSphere(vector < Mesh* > &meshes)
{
    transform = mat4(1);

    for (size_t i = 0; i < meshes.size(); i++)
    {
        allVertices.push_back(meshes[i]->getVertices());
    }
}

pair < vec3, vec3 > BoundSphere::findFurthest(pair < vec3, vec3 > centerAndBound)
{
    for (size_t i = 0; i < allVertices.size(); i++)
    {
        for (size_t j = 0; j < allVertices[i].size(); j++)
        {
            double radius = distance(centerAndBound.first, centerAndBound.second);
            double newRadius = distance(centerAndBound.first, allVertices[i][j].position);

            if (newRadius > radius)
            {
                centerAndBound.second = allVertices[i][j].position;
            }
        }
    }

    return centerAndBound;

    /* random vertices */
    /*srand(time(0));

    int a = rand() % allVertices.size();
    int b = rand() % allVertices[a].size();

    return {centerAndBound.first, allVertices[a][b].position};*/
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
    pair < vec3, vec3 > centerAndBound;

    centerAndBound.first = findCenter();

    centerAndBound.second = allVertices[0][0].position;
    
    points = findFurthest(centerAndBound);
}

void BoundSphere::applyTransform(mat4 &transform)
{
    this->transform = transform;
}

vec3 BoundSphere::getCenter()
{
    return points.first;
}

double BoundSphere::getRadius()
{
    return distance(points.first, points.second);
}

vec3 BoundSphere::getTransformedCenter()
{
    return vec3(transform * vec4(points.first, 1.0));
}

double BoundSphere::getTransformedRadius()
{
    return distance(vec3(transform * vec4(points.first, 1.0)), vec3(transform * vec4(points.second, 1.0)));
}

BoundSphere::~BoundSphere(){}
