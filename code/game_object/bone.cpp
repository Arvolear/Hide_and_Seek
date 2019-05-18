#include "animation.hpp"
#include "../global/convert.hpp"
#include "../global/shader.hpp"
#include "mesh.hpp"
#include "bone.hpp"

using namespace std;
using namespace glm;
using namespace Assimp;

Bone::Bone(){}

mat4 Bone::getParentTransforms()
{
    Bone* b = parentBone; //this bone`s parent

    vector < mat4 > matrices; //vector to remember matrices

    while (b) //loop through all parents to push_back all transformation matrices
    {
        mat4 tmp = aiMatrix4x4ToGlm(b->node->mTransformation); //get transformation matrix
        matrices.push_back(tmp); //remember it

        b = b->parentBone; //next parent
    }

    mat4 res; //all parent transformation matrix

    for (int i = matrices.size() - 1; i >= 0; i--) //loop backward! The last element of the array is the root bone 
    {
        res *= matrices[i]; //here we calculate the combined transformation matrix
    }
    
    return res;
}

vec3 Bone::calcInterpolatedPosition(int id, float time)
{
    vec3 out; //result

    if (animation.at(id)->nodeAnim->mNumPositionKeys == 1) //if amount of keyframes == 1 we dont have to iterpolate anything
    {
        aiVector3D help = animation.at(id)->nodeAnim->mPositionKeys[0].mValue; //get its value/position

        out = vec3(help.x, help.y, help.z);

        return out;
    }

    //if there is more than one keyframe
    int positionIndex = findPosition(id, time); //find index of the keyframe
    int nextPositionIndex = positionIndex + 1; //the next index

    float deltaTime = (float)(animation.at(id)->nodeAnim->mPositionKeys[nextPositionIndex].mTime - animation.at(id)->nodeAnim->mPositionKeys[positionIndex].mTime); //calculate time difference between keyframes 
    float factor = (time - (float)animation.at(id)->nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime; //calculate how far we are between keyframes in the interval of [0;1]

    aiVector3D begin = animation.at(id)->nodeAnim->mPositionKeys[positionIndex].mValue; //get the first keyframe value
    aiVector3D end = animation.at(id)->nodeAnim->mPositionKeys[nextPositionIndex].mValue; //get the second one

    vec3 p1(begin.x, begin.y, begin.z);
    vec3 p2(end.x, end.y, end.z);

    out = mix(p1, p2, factor); //using glm function to mix the values dependent on factor

    //cout << out.x << ' ' << out.y << ' ' << out.z << endl << endl;
    
    return out;
}

quat Bone::calcInterpolatedRotation(int id, float time)
{
    quat out; //result

    if (animation.at(id)->nodeAnim->mNumRotationKeys == 1) //if amount of keyframes == 1 we dont have to iterpolate anything
    {
        aiQuaternion help = animation.at(id)->nodeAnim->mRotationKeys[0].mValue; //get it`s value/rotation quaternion

        out = quat(help.w, help.x, help.y, help.z);
        return out;
    }

    //if there is more than one keyframe
    int rotationIndex = findRotation(id, time); //find index of the keyframe
    int nextRotationIndex = rotationIndex + 1; //the next index

    float deltaTime = (float)(animation.at(id)->nodeAnim->mRotationKeys[nextRotationIndex].mTime - animation.at(id)->nodeAnim->mRotationKeys[rotationIndex].mTime); //calculate time difference between keyframes
    float factor = (time - (float)animation.at(id)->nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;  //calculate how far we are between keyframes in the interval of [0;1]

    aiQuaternion begin = animation.at(id)->nodeAnim->mRotationKeys[rotationIndex].mValue;
    aiQuaternion end = animation.at(id)->nodeAnim->mRotationKeys[nextRotationIndex].mValue;

    quat r1(begin.w, begin.x, begin.y, begin.z);
    quat r2(end.w, end.x, end.y, end.z);

    //up to this point everything is the same as for position

    out = slerp(r1, r2, factor); //here we use another glm function to mix the quternion values dependen on factor

    //cout << out.x << ' ' << out.y << ' ' << out.z << endl << endl;
    
    return out;
}

int Bone::findPosition(int id, float time)
{
    int l = 0;
    int r = animation.at(id)->nodeAnim->mNumPositionKeys - 1;

    if (animation.at(id)->nodeAnim->mPositionKeys[r].mTime <= time) //if the given time is more than the last frame
    {
        return r;
    }

    while (l < r - 1) //binary search
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

    while (l < r - 1) //binary search
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
    if (!animation.at(id)->nodeAnim) //if there is no animations for this bone
    {
        return;
    }
    
    vec3 scal = vec3(1.0); //we don`t have scaling animations
    vec3 pos = calcInterpolatedPosition(id, time); //calculate interpolated position
    quat rot = calcInterpolatedRotation(id, time); //calculate interpolated rotation

    mat4 res(1.0);

    //here we construct the transformation matrix in STR order
    res *= scale(scal);
    res *= translate(pos);
    res *= mat4_cast(rot);

    node->mTransformation = glmToAiMatrix4x4(res); //mTransformation is public variable so we can store there our new transformation matrix
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
        
string Bone::getName()
{
    return name;
}

int Bone::getId()
{
    return id;
}

aiNode* Bone::getNode()
{
    return node;
}

AnimationData* Bone::getAnimation(int id)
{
    return animation.at(id);
}

Bone* Bone::getParent()
{
    return parentBone;
}
    
mat4 Bone::getOffset()
{
    return offset;
}

Bone::~Bone(){}
