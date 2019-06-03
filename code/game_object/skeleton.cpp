#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "mesh.hpp"
#include "animation.hpp"
#include "bone.hpp"
#include "skeleton.hpp"

using namespace std;
using namespace glm;
using namespace Assimp;

Skeleton::Skeleton(map < string, Bone* > &bones)
{
    this->bones = bones; 

    time = 0; 

    activeAnimation = nullptr;
}

void Skeleton::playAnimation(Animation* anim, bool reset)
{
    if (activeAnimation) 
    {
        if (anim->getPriority() >= activeAnimation->getPriority()) 
        {
            activeAnimation = anim; 
        }
    }
    else
    {
        activeAnimation = anim;
    }
    
    if (reset)
    {
        if (activeAnimation->getStart() == "default")
        {
            time = 0;
        }
        else
        {
            time = atof(activeAnimation->getStart().data()); 
        }
    }
}
        
void Skeleton::stopAnimation()
{
    time = 0;
    activeAnimation = nullptr;
}

void Skeleton::renderBonesMatrices(Shader* shader)
{
    if (!bones.empty()) 
    {
        shader->setInt("meshWithBones", 1); 

        bonesMatrices.clear(); 

        int index = 0;
        map < string, Bone* >::iterator it = bones.begin();

        for (int i = 0; i < MAX_BONES_AMOUNT; i++, it++) 
        {
            if (i >= int(bones.size())) 
            {
                bonesMatrices.push_back(mat4(1.0)); 
                index = i;
            }
            else
            {
                mat4 res = it->second->getFullTransform() * it->second->getOffset(); 

                bonesMatrices.push_back(res); 

                index = it->second->getId();
            }

            shader->setMat4("bones[" + to_string(index) + "]", bonesMatrices[i]); 
        }
    }
    else 
    {
        shader->setInt("meshWithBones", 0); 
    }
}

void Skeleton::update(Shader *shader)
{ 
    renderBonesMatrices(shader); 

    if (!activeAnimation) 
    {
        return; 
    }

    if (activeAnimation->getSpeed() == "default")
    {
        map < string, Bone* >::iterator it = bones.begin();
        time += it->second->getAnimation(activeAnimation->getId())->speed;
    }
    else
    {
        time += atof(activeAnimation->getSpeed().data()); 
    }

    if (activeAnimation->getStart() == "default" && activeAnimation->getEnd() == "default")
    {
        map < string, Bone* >::iterator it = bones.begin();

        if (time < 0) 
        {
            time = 0; 
        }

        if (time >= it->second->getAnimation(activeAnimation->getId())->duration) 
        {
            if (activeAnimation->getLoop()) 
            {
                time = 0; 
            }
            else
            {
                stopAnimation(); 
            }
        }
    }
    else
    {
        if (time < atof(activeAnimation->getStart().data())) 
        {
            time = atof(activeAnimation->getStart().data()); 
        }

        if (time >= atof(activeAnimation->getEnd().data())) 
        {
            if (activeAnimation->getLoop()) 
            {
                time = atof(activeAnimation->getStart().data()); 
            }
            else
            {
                stopAnimation(); 
            }
        }
    }

    for (auto& it : bones) 
    {
        it.second->updateKeyframeTransform(activeAnimation->getId(), time); 
    }
}

Skeleton::~Skeleton()
{
    for (auto& it : bones)
    {
        delete it.second;
    } 

    delete activeAnimation;
}
