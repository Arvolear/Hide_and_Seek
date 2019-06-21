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
    activeAnimation = anim;
    
    if (reset)
    {
        /* default + preferred one */
        time = activeAnimation->getStartFrame(); 
    }
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

void Skeleton::stopAnimation()
{
    time = 0;
    activeAnimation = nullptr;
}
        
Animation* Skeleton::getAnimation() const
{
    return activeAnimation;
}

void Skeleton::update(Shader *shader)
{ 
    renderBonesMatrices(shader); 

    if (!activeAnimation) 
    {
        return; 
    }

    /* default */
    if (!activeAnimation->getSpeed())
    {
        map < string, Bone* >::iterator it = bones.begin();
        time += it->second->getAnimation(activeAnimation->getAnimId())->speed;
    }
    else
    {
        time += activeAnimation->getSpeed(); 
    }

    /* default */
    if (activeAnimation->getFramesRange() == vec2(0))
    {
        map < string, Bone* >::iterator it = bones.begin();

        if (time < 0) 
        {
            time = 0; 
        }

        if (time >= it->second->getAnimation(activeAnimation->getAnimId())->duration) 
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
        if (time < activeAnimation->getStartFrame()) 
        {
            time = activeAnimation->getStartFrame(); 
        }

        if (time >= activeAnimation->getEndFrame()) 
        {
            if (activeAnimation->getLoop()) 
            {
                time = activeAnimation->getStartFrame(); 
            }
            else
            {
                stopAnimation(); 
            }
        }
    }

    for (auto& it : bones) 
    {
        it.second->updateKeyframeTransform(activeAnimation->getAnimId(), time); 
    }
}

Skeleton::~Skeleton()
{
    for (auto& it : bones)
    {
        delete it.second;
    } 
}
