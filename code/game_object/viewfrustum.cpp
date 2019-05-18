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

    return (local.x + local.y + local.z - d) / l;
}

ViewFrustum::ViewFrustum()
{
    frustum.resize(6);
}

void ViewFrustum::updateFrustum(mat4 &view, mat4 &projection)
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
    bool inside = true;

    for (size_t i = 0; i < frustum.size(); i++)
    {
        if (frustum[i].distance(point) < 0)
        {
            inside = false;
            return inside;
        }
    }

    return inside;
}

bool ViewFrustum::isSphereInFrustum(vec3 center, float radius) const
{
    bool inside = true;

    for (size_t i = 0; i < frustum.size(); i++)
    {
        if (frustum[i].distance(center) < -radius)
        {
            inside = false;
            return inside;
        }
    }

    return inside;
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
