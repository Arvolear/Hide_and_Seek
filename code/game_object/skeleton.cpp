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
    meshWithBones = !bones.empty();

    activeAnimation = nullptr;

    ready = false;
}

void Skeleton::playAnimation(Animation* anim, bool reset)
{
    activeAnimation = anim;
    
    if (reset)
    {
        activeAnimation->fromStart(); 
    }
}
        
void Skeleton::renderBonesMatrices(Shader* shader)
{
    shader->setInt("meshWithBones", meshWithBones); 

    if (!bones.empty()) 
    {
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
}

void Skeleton::stopAnimation()
{
    for (auto& it : bones) 
    {
        it.second->updateKeyframeTransform(activeAnimation->getAnimId(), 0); 
    }
    
    activeAnimation = nullptr;
}
        
Animation* Skeleton::getAnimation() const
{
    return activeAnimation;
}
        
void Skeleton::setBonesMatrices(vector < mat4 > &bonesMatrices)
{
    unique_lock < mutex > lck(mtx);
    
    while (!ready)
    {
        cv.wait(lck);
    }

    this->bonesMatrices = bonesMatrices;
    meshWithBones = !bonesMatrices.empty();
}

vector < mat4 > Skeleton::getBonesMatrices() const
{
    unique_lock < mutex > lck(mtx);
    
    while (!ready)
    {
        cv.wait(lck);
    }

    return bonesMatrices;
}

bool Skeleton::isMeshWithBones() const
{
    return meshWithBones;
}

void Skeleton::update(Shader *shader)
{ 
    unique_lock < mutex > lck(mtx);
    ready = false;

    renderBonesMatrices(shader); 

    if (!activeAnimation) 
    {
        ready = true;
        cv.notify_all();

        return; 
    }

    /* default speed */
    if (!activeAnimation->getSpeed())
    {
        map < string, Bone* >::iterator it = bones.begin();
        activeAnimation->setSpeed(it->second->getAnimation(activeAnimation->getAnimId())->speed);
    }
        
    /* default frames range */
    if (activeAnimation->getFramesRange().y == 0.0)
    {
        map < string, Bone* >::iterator it = bones.begin();

        activeAnimation->setFramesRange(vec2(activeAnimation->getFramesRange().x, it->second->getAnimation(activeAnimation->getAnimId())->duration));
        activeAnimation->fromStart();
    }
    
    for (auto& it : bones) 
    {
        it.second->updateKeyframeTransform(activeAnimation->getAnimId(), activeAnimation->getCurFrame()); 
    }
   
    /* next frame */
    if (!activeAnimation->nextFrame())
    {
        stopAnimation();
    }

    ready = true;
    cv.notify_all();
}

Skeleton::~Skeleton()
{
    for (auto& it : bones)
    {
        delete it.second;
    } 
}
