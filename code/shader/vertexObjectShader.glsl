#version 330 core

#define BONES_AMOUNT 6
#define MAX_BONES_AMOUNT 50

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture;
layout (location = 3) in float boneIDs[BONES_AMOUNT];
layout (location = 9) in float boneWeights[BONES_AMOUNT];

uniform mat4 localTransform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 shadowView; ////////////////////////////
uniform mat4 shadowProjection; /////////////////////////////

uniform mat4 bones[MAX_BONES_AMOUNT];
uniform int meshWithBones;

out vec3 fragmentPos;
out vec3 fragmentNorm;
out vec2 textureCoords;

out vec4 shadowCoords;

void main()
{
    mat4 bonesTransform;

    if (meshWithBones == 1)
    {
        bonesTransform = mat4(0.0);

        for (int i = 0; i < BONES_AMOUNT; i++)
        {
            bonesTransform += bones[int(boneIDs[i])] * boneWeights[i];
        }
    }
    else
    {
        bonesTransform = mat4(1.0);    
    }

    gl_Position = projection * view * model * localTransform * bonesTransform * vec4(position, 1.0);

    fragmentPos = vec3(model * localTransform * bonesTransform * vec4(position, 1.0f));
    fragmentNorm = mat3(model) * mat3(localTransform) * mat3(bonesTransform) * normal;

    textureCoords = vec2(texture.x, -texture.y); //reverse textures

    shadowCoords = shadowProjection * shadowView * model * localTransform * bonesTransform * vec4(position, 1.0);
}
