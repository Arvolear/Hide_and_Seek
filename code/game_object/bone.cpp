#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"

using namespace std;
using namespace glm;
using namespace Assimp;

Bone::Bone(){}

mat4 Bone::getFullTransform() const
{
    const Bone* b = this; 

    vector < mat4 > matrices; 

    while (b) 
    {
        mat4 tmp = aiMatrix4x4ToGlm(b->node->mTransformation); 
        matrices.push_back(tmp); 

        b = b->parentBone; 
    }

    mat4 res; 

    for (int i = matrices.size() - 1; i >= 0; i--) 
    {
        res *= matrices[i]; 
    }
    
    return res;
}

vec3 Bone::calcInterpolatedPosition(int id, float time)
{
    vec3 out; 

    if (animation.at(id)->nodeAnim->mNumPositionKeys == 1) 
    {
        aiVector3D help = animation.at(id)->nodeAnim->mPositionKeys[0].mValue; 

        out = vec3(help.x, help.y, help.z);

        return out;
    }
    
    int positionIndex = findPosition(id, time); 
    int nextPositionIndex = positionIndex + 1; 

    float deltaTime = (float)(animation.at(id)->nodeAnim->mPositionKeys[nextPositionIndex].mTime - animation.at(id)->nodeAnim->mPositionKeys[positionIndex].mTime); 
    float factor = (time - (float)animation.at(id)->nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime; 

    aiVector3D begin = animation.at(id)->nodeAnim->mPositionKeys[positionIndex].mValue; 
    aiVector3D end = animation.at(id)->nodeAnim->mPositionKeys[nextPositionIndex].mValue; 

    vec3 p1(begin.x, begin.y, begin.z);
    vec3 p2(end.x, end.y, end.z);

    out = mix(p1, p2, factor); 
 
    return out;
}

quat Bone::calcInterpolatedRotation(int id, float time)
{
    quat out; 

    if (animation.at(id)->nodeAnim->mNumRotationKeys == 1) 
    {
        aiQuaternion help = animation.at(id)->nodeAnim->mRotationKeys[0].mValue; 

        out = quat(help.w, help.x, help.y, help.z);
        return out;
    }
    
    int rotationIndex = findRotation(id, time); 
    int nextRotationIndex = rotationIndex + 1; 

    float deltaTime = (float)(animation.at(id)->nodeAnim->mRotationKeys[nextRotationIndex].mTime - animation.at(id)->nodeAnim->mRotationKeys[rotationIndex].mTime); 
    float factor = (time - (float)animation.at(id)->nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;  

    aiQuaternion begin = animation.at(id)->nodeAnim->mRotationKeys[rotationIndex].mValue;
    aiQuaternion end = animation.at(id)->nodeAnim->mRotationKeys[nextRotationIndex].mValue;

    quat r1(begin.w, begin.x, begin.y, begin.z);
    quat r2(end.w, end.x, end.y, end.z);

    out = slerp(r1, r2, factor); 
    
    return out;
}

int Bone::findPosition(int id, float time)
{
    int l = 0;
    int r = animation.at(id)->nodeAnim->mNumPositionKeys - 1;

    if (animation.at(id)->nodeAnim->mPositionKeys[r].mTime <= time) 
    {
        return r;
    }

    /* binsearch through keyframes */
    while (l < r - 1) 
    {
        int m = (l + r) / 2;

        if (animation.at(id)->nodeAnim->mPositionKeys[m].mTime <= time)
        {
            l = m;
        }
        else
        {
            r = m;
        }
    }
        
    return l;
}        

int Bone::findRotation(int id, float time)
{
    int l = 0;
    int r = animation.at(id)->nodeAnim->mNumRotationKeys - 1;

    if (animation.at(id)->nodeAnim->mRotationKeys[r].mTime <= time)
    {
        return r;
    }

    /* binsearch through keyframes */
    while (l < r - 1) 
    {
        int m = (l + r) / 2;

        if (animation.at(id)->nodeAnim->mRotationKeys[m].mTime <= time)
        {
            l = m;
        }
        else
        {
            r = m;
        }
    }
        
    return l;
}

void Bone::updateKeyframeTransform(int id, float time)
{
    if (!animation.at(id)->nodeAnim) 
    {
        return;
    }
    
    vec3 scal = vec3(1.0); 
    vec3 pos = calcInterpolatedPosition(id, time); 
    quat rot = calcInterpolatedRotation(id, time); 

    mat4 res(1.0);
    
    res *= scale(scal);
    res *= translate(pos);
    res *= mat4_cast(rot);

    node->mTransformation = glmToAiMatrix4x4(res); 
}

void Bone::setName(string &name)
{
    this->name = name;
}

void Bone::setId(int id)
{
    this->id = id;
}

void Bone::setNode(aiNode* node)
{
    this->node = node;
}
        
void Bone::setAnimation(vector < AnimationData* > animation)
{
    this->animation = animation;
}
        
void Bone::setParentBone(Bone* parent)
{
    parentBone = parent;
}

void Bone::setOffset(mat4 &offset)
{
    this->offset = offset;
}
        
string Bone::getName() const
{
    return name;
}

int Bone::getId() const
{
    return id;
}

aiNode* Bone::getNode() const
{
    return node;
}

AnimationData* Bone::getAnimation(int id) const
{
    return animation.at(id);
}

Bone* Bone::getParent() const
{
    return parentBone;
}
    
mat4 Bone::getOffset() const
{
    return offset;
}

Bone::~Bone(){}
