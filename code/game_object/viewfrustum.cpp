#include "../shader/shader.hpp"

#include "../debug/debugdrawer.hpp"

#include "viewfrustum.hpp"

void Plane::setData(float a, float b, float c, float d)
{
    norm = vec3(a, b, c);

    this->d = d;
}

float Plane::distance(vec3 point) const
{
    float l = norm.length();

    vec3 local = norm * point;

    return (local.x + local.y + local.z + d) / l;
}

ViewFrustum::ViewFrustum()
{
    frustum.resize(6);
}

void ViewFrustum::updateFrustum(mat4 view, mat4 projection)
{
    this->view = view;
    this->projection = projection;

    mat4 clip = projection * view;

    //near plane
    frustum[0].setData(clip[0][3] + clip[0][2], clip[1][3] + clip[1][2], clip[2][3] + clip[2][2], clip[3][3] + clip[3][2]);
                                              
    //far plane                               
    frustum[1].setData(clip[0][3] - clip[0][2], clip[1][3] - clip[1][2], clip[2][3] - clip[2][2], clip[3][3] - clip[3][2]);
                                                                        
    //bottom                                                            
    frustum[2].setData(clip[0][3] + clip[0][1], clip[1][3] + clip[1][1], clip[2][3] + clip[2][1], clip[3][3] + clip[3][1]);
                                                                        
    //top                                                               
    frustum[3].setData(clip[0][3] - clip[0][1], clip[1][3] - clip[1][1], clip[2][3] - clip[2][1], clip[3][3] - clip[3][1]);
                                                                        
    //left                                                              
    frustum[4].setData(clip[0][3] + clip[0][0], clip[1][3] + clip[1][0], clip[2][3] + clip[2][0], clip[3][3] + clip[3][0]);
                                                                        
    //right                                                             
    frustum[5].setData(clip[0][3] - clip[0][0], clip[1][3] - clip[1][0], clip[2][3] - clip[2][0], clip[3][3] - clip[3][0]);
}

bool ViewFrustum::isPointInFrustum(vec3 point) const
{
    for (size_t i = 0; i < frustum.size(); i++)
    {
        if (frustum[i].distance(point) <= 0)
        {
            return false;
        }
    }

    return true;
}

bool ViewFrustum::isSphereInFrustum(vec3 center, float radius) const
{
    for (size_t i = 0; i < frustum.size(); i++)
    {
        if (frustum[i].distance(center) <= -radius)
        {
            return false;
        }
    }

    return true;
}

void ViewFrustum::render(DebugDrawer* debugDrawer)
{
    mat4 inv = inverse(projection * view);

    vec4 clipFrustum[8] =
    {
        // near face
        {1, 1, -1, 1},
        {-1, 1, -1, 1},
        {1, -1, -1, 1},
        {-1, -1, -1, 1},

        // far face
        {1, 1, 1, 1},
        {-1, 1, 1, 1},
        {1, -1, 1, 1},
        {-1, -1, 1, 1}
    };

    btVector3 globalFrustum[8];

    for (int i = 0; i < 8; i++)
    {
        vec4 globalVec = inv * clipFrustum[i];

        globalFrustum[i].setX(globalVec.x / globalVec.w);
        globalFrustum[i].setY(globalVec.y / globalVec.w);
        globalFrustum[i].setZ(globalVec.z / globalVec.w);
    }

    btVector3 color(0.1, 0.1, 1.0);

    debugDrawer->drawLine(globalFrustum[0], globalFrustum[1], color);
    debugDrawer->drawLine(globalFrustum[0], globalFrustum[2], color);
    debugDrawer->drawLine(globalFrustum[3], globalFrustum[1], color);
    debugDrawer->drawLine(globalFrustum[3], globalFrustum[2], color);

    debugDrawer->drawLine(globalFrustum[4], globalFrustum[5], color);
    debugDrawer->drawLine(globalFrustum[4], globalFrustum[6], color);
    debugDrawer->drawLine(globalFrustum[7], globalFrustum[5], color);
    debugDrawer->drawLine(globalFrustum[7], globalFrustum[6], color);

    debugDrawer->drawLine(globalFrustum[0], globalFrustum[4], color);
    debugDrawer->drawLine(globalFrustum[1], globalFrustum[5], color);
    debugDrawer->drawLine(globalFrustum[3], globalFrustum[7], color);
    debugDrawer->drawLine(globalFrustum[2], globalFrustum[6], color);
}

mat4 ViewFrustum::getView() const
{
    return view;
}

mat4 ViewFrustum::getProjection() const
{
    return projection;
}

ViewFrustum::~ViewFrustum(){}
