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
        map < string, Bone* > bones; 
        vector < mat4 > bonesMatrices; 

        Animation* activeAnimation; 

        void renderBonesMatrices(Shader* shader); 
        
    public:
        Skeleton(map < string, Bone* > &bones);

        void playAnimation(Animation* anim, bool reset = true); 
        void stopAnimation(); 
        Animation* getAnimation() const;

        void update(Shader* shader); 

        ~Skeleton();
};
