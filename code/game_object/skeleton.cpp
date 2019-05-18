#include "../global/shader.hpp"
#include "../global/convert.hpp"
#include "mesh.hpp"
#include "animation.hpp"
#include "bone.hpp"
#include "skeleton.hpp"

using namespace std;
using namespace glm;
using namespace Assimp;

Skeleton::Skeleton(map < string, Bone* > &bones)
{
    this->bones = bones; //set bones

    time = 0; //time is 0

    activeAnimation = nullptr;
}

void Skeleton::playAnimation(Animation* anim, bool reset)
{
    if (activeAnimation) //if something is playing
    {
        if (anim->getPriority() >= activeAnimation->getPriority()) //if new animation has greater or equal priority
        {
            activeAnimation = anim; //change the animation
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
            time = atof(activeAnimation->getStart().data()); //set timer to the beginning
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
    if (!bones.empty()) //if there are bones in the model
    {
        glUniform1i(glGetUniformLocation(shader->ID, "meshWithBones"), 1); //say the shader that there are some bones in the mesh

        bonesMatrices.clear(); //clear the values

        int index = 0;
        map < string, Bone* >::iterator it = bones.begin();

        for (int i = 0; i < MAX_BONES_AMOUNT; i++, it++) //loop through 100 bones
        {
            if (i >= int(bones.size())) //if we have run out of bones
            {
                bonesMatrices.push_back(mat4(1.0)); //there is no transformation matrix, so we leave it as 1.0
                index = i;
            }
            else
            {
                mat4 res = it->second->getParentTransforms() * aiMatrix4x4ToGlm(it->second->getNode()->mTransformation); //calculate transformation matrix for our bone

                bonesMatrices.push_back(res * it->second->getOffset()); //calculate full transformation matrix

                index = it->second->getId();
            }

            glUniformMatrix4fv(glGetUniformLocation(shader->ID, ("bones[" + to_string(index) + "]").c_str()), 1, GL_FALSE, value_ptr(bonesMatrices[i])); //send the matrix to the shader
        }
    }
    else //if there arent any
    {
        glUniform1i(glGetUniformLocation(shader->ID, "meshWithBones"), 0); //say the shader that there are no bones in the mesh
    }
}

void Skeleton::update(Shader *shader)
{ 
    renderBonesMatrices(shader); //send bones` matrices to the shader

    if (!activeAnimation) //if there is no animation
    {
        return; //do nothing
    }

    if (activeAnimation->getSpeed() == "default")
    {
        map < string, Bone* >::iterator it = bones.begin();
        time += it->second->getAnimation(activeAnimation->getId())->speed;
    }
    else
    {
        time += atof(activeAnimation->getSpeed().data()); //update the timer
    }

    if (activeAnimation->getStart() == "default" && activeAnimation->getEnd() == "default")
    {
        map < string, Bone* >::iterator it = bones.begin();

        if (time < 0) //if time is less than starting time of the animation 
        {
            time = 0; //set time to the start
        }

        if (time >= it->second->getAnimation(activeAnimation->getId())->duration) //if the time is grater than the end time
        {
            if (activeAnimation->getLoop()) //if animation is looped
            {
                time = 0; //set time to the start
            }
            else
            {
                stopAnimation(); //stop animation
            }
        }
    }
    else
    {
        if (time < atof(activeAnimation->getStart().data())) //if time is less than starting time of the animation 
        {
            time = atof(activeAnimation->getStart().data()); //set time to the start
        }

        if (time >= atof(activeAnimation->getEnd().data())) //if the time is grater than the end time
        {
            if (activeAnimation->getLoop()) //if animation is looped
            {
                time = atof(activeAnimation->getStart().data()); //set time to the start
            }
            else
            {
                stopAnimation(); //stop animation
            }
        }
    }

    for (auto& it : bones) //loop through each bone
    {
        it.second->updateKeyframeTransform(activeAnimation->getId(), time); //calculate it`s tranformation matrix
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
