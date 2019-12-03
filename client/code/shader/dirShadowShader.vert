#version 330 core

#define MAX_BONES_AMOUNT 50
#define BONES_AMOUNT 6

layout (location = 0) in vec3 position;

layout (location = 4) in vec2 boneIDs[BONES_AMOUNT / 2];
layout (location = 7) in vec2 boneWeights[BONES_AMOUNT / 2];

layout (location = 10) in mat4 instanceMatrix;

uniform mat4 localTransform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int meshInstanced;

uniform mat4 bones[MAX_BONES_AMOUNT];
uniform int meshWithBones;

out vec4 fragmentPos;

void main()
{
    mat4 instanceMat;
    mat4 bonesTransform;

    if (meshWithBones == 1)
    {
        bonesTransform = mat4(0.0);

        for (int i = 0; i < BONES_AMOUNT / 2; i++)
        {
            bonesTransform += bones[int(boneIDs[i].x)] * boneWeights[i].x;
            bonesTransform += bones[int(boneIDs[i].y)] * boneWeights[i].y;
        }
    }
    else
    {
        bonesTransform = mat4(1.0);    
    }
    
    /* instansed? */
    if (meshInstanced == 1)
    {
        instanceMat = instanceMatrix;
    }
    else
    {
        instanceMat = mat4(1.0);
    }

    gl_Position = projection * view * model * localTransform * instanceMat * bonesTransform * vec4(position, 1.0f);

    fragmentPos = gl_Position;
    fragmentPos /= fragmentPos.w;
    fragmentPos = fragmentPos * 0.5 + 0.5;
}
