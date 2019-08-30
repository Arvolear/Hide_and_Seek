#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <assimp/scene.h>

using namespace std;
using namespace glm;
using namespace Assimp;

struct AnimationData
{
    string name = "";

    float duration = 0;
    float speed = 0;

    aiNodeAnim* nodeAnim = 0;
};

class Bone
{
    private:
        string name; 
        int id; 

        aiNode* node; 
        
        vector < AnimationData* > animation; 

        Bone* parentBone; 
        mat4 offset; 

        vec3 calcInterpolatedPosition(int id, float time); 
        quat calcInterpolatedRotation(int id, float time); 
        
        int findPosition(int id, float time); 
        int findRotation(int id, float time); 

    public:
        Bone();
        
        mat4 getFullTransform() const; 
        void updateKeyframeTransform(int id, float time); 
        
        void setName(string &name);
        void setId(int id);
        void setNode(aiNode* node);
        void setAnimation(vector < AnimationData* > animation);
        void setParentBone(Bone* parent);
        void setOffset(mat4 &offset);

        string getName() const;
        int getId() const;
        aiNode* getNode() const;
        AnimationData* getAnimation(int id) const;
        Bone* getParent() const;
        mat4 getOffset() const;

        ~Bone();
};
