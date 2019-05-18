#pragma once

#include <vector>
#include <map>
#include <string>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

#define MAX_BONES_AMOUNT 50

class Skeleton
{
    private:
        map < string, Bone* > bones; //here we keep our bones
        vector < mat4 > bonesMatrices; //final transformation matrices that go to the shader

        float time; //little timer

        Animation* activeAnimation; //little Animation class to keep the animation data

        void renderBonesMatrices(Shader* shader); //update and send bones` matrices to the shader
        
    public:
        Skeleton(map < string, Bone* > &bones);

        void playAnimation(Animation* anim, bool reset = true); //play desired animation
        void stopAnimation(); //stop playing

        void update(Shader* shader); //here we update the animation and call updateSkeletonMatrices

        ~Skeleton();
};
