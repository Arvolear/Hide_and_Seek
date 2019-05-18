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
        string name; //the name of the bone
        int id; //bones id in the model

        aiNode* node; //the node this bone is attached to
        //aiNodeAnim* nodeAnim; //this bone animation node

        vector < AnimationData* > animation; 

        Bone* parentBone; //parent bone
        mat4 offset; //offset matrix

        vec3 calcInterpolatedPosition(int id, float time); //calculates interpolated position between two keyframes
        quat calcInterpolatedRotation(int id, float time); //calculates interpolated rotation between two keyframes
        
        int findPosition(int id, float time); //finds the index of the keyframe with the value less then time
        int findRotation(int id, float time); //same as above

    public:
        Bone();
        
        mat4 getParentTransforms(); //calculates the whole transformation matrix starting with the root bone 
        void updateKeyframeTransform(int id, float time); //updates this bone transformation matrix
        
        void setName(string &name);
        void setId(int id);
        void setNode(aiNode* node);
        void setAnimation(vector < AnimationData* > animation);
        void setParentBone(Bone* parent);
        void setOffset(mat4 &offset);

        string getName();
        int getId();
        aiNode* getNode();
        AnimationData* getAnimation(int id);
        Bone* getParent();
        mat4 getOffset();

        ~Bone();
};
