#pragma once

#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

#define MAX_BONES_AMOUNT 50

class Skeleton
{
    private:
        bool meshWithBones;

        map < string, Bone* > bones; 
        vector < mat4 > bonesMatrices; 

        Animation* activeAnimation; 

        void renderBonesMatrices(Shader* shader); 
        
    public:
        Skeleton(map < string, Bone* > &bones);

        void playAnimation(Animation* anim, bool reset = true); 
        void stopAnimation(); 
        Animation* getAnimation() const;

        void setBonesMatrices(vector < mat4 > &bonesMatrices);

        vector < mat4 > getBonesMatrices() const;
        bool isMeshWithBones() const;

        void update(Shader* shader); 

        ~Skeleton();
};
