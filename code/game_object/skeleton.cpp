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

    activeAnimation = nullptr;
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

vector < mat4 > Skeleton::getBonesMatrices() const
{
    return bonesMatrices;
}

void Skeleton::update(Shader *shader)
{ 
    renderBonesMatrices(shader); 

    if (!activeAnimation) 
    {
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
}

Skeleton::~Skeleton()
{
    for (auto& it : bones)
    {
        delete it.second;
    } 
}
