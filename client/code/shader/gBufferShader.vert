#version 330 core

#define BONES_AMOUNT 6

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec2 boneIDs[BONES_AMOUNT / 2];
layout (location = 7) in vec2 boneWeights[BONES_AMOUNT / 2];
layout (location = 10) in mat4 instanceMatrix;

uniform mat4 localTransform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int meshInstanced;

#define MAX_BONES_AMOUNT 50
uniform mat4 bones[MAX_BONES_AMOUNT];
uniform int meshWithBones;
uniform int meshNormalMapped;

out vec2 textureCoords;

out vec3 fragmentPos;
out vec3 fragmentNorm;

out mat3 TBN;

out vec3 ssaoFragmentPos;
out vec3 ssaoFragmentNorm;

out mat3 ssaoTBN;

void main()
{
    mat4 instanceMat;
    mat4 bonesTransform;

    /* animated? */
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

    gl_Position = projection * view * model * localTransform * instanceMat * bonesTransform * vec4(position, 1.0);

    fragmentPos = vec3(model * localTransform * instanceMat * bonesTransform * vec4(position, 1.0));
    fragmentNorm = vec3(model * localTransform * instanceMat * bonesTransform * vec4(normal, 0.0));

    ssaoFragmentPos = vec3(view * vec4(fragmentPos, 1.0));
    ssaoFragmentNorm = vec3(view * vec4(fragmentNorm, 0.0));

    /* flip UV */
    textureCoords = vec2(uv.x, -uv.y);

    /* normal mapped? */
    if (meshNormalMapped == 1)
    {
        vec3 TT = vec3(model * localTransform * instanceMat * bonesTransform * vec4(tangent, 0.0));
        vec3 NN = vec3(model * localTransform * instanceMat * bonesTransform * vec4(normal, 0.0));

        /* SCENE */
        vec3 T = normalize(TT);
        vec3 N = normalize(NN);

        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);
        TBN = mat3(T, B, N);

        /* SSAO */
        vec3 ssaoT = normalize(vec3(view * vec4(TT, 0.0)));
        vec3 ssaoN = normalize(vec3(view * vec4(NN, 0.0)));

        ssaoT = normalize(ssaoT - dot(ssaoT, ssaoN) * ssaoN);
        vec3 ssaoB = cross(ssaoN, ssaoT);
        ssaoTBN = mat3(ssaoT, ssaoB, ssaoN);
    }
    else
    {
        ssaoTBN = TBN = mat3(0);
    }
}
