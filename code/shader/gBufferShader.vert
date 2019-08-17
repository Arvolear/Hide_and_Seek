#version 330 core

#define BONES_AMOUNT 6

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in float boneIDs[BONES_AMOUNT];
layout (location = 10) in float boneWeights[BONES_AMOUNT];

uniform mat4 localTransform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define MAX_BONES_AMOUNT 50
uniform mat4 bones[MAX_BONES_AMOUNT];
uniform int meshWithBones;
uniform int meshNormalMapped;

out vec3 fragmentPos;
out vec3 fragmentNorm;
out vec2 textureCoords;
out mat3 TBN;

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
    fragmentNorm = vec3(model * localTransform * bonesTransform * vec4(normal, 0.0));

    /* flip UV */
    textureCoords = vec2(uv.x, -uv.y);

    /* normal mapping */
    if (meshNormalMapped == 1)
    {
        vec3 T = normalize(vec3(model * localTransform * bonesTransform * vec4(tangent, 0.0)));
        vec3 N = normalize(vec3(model * localTransform * bonesTransform * vec4(normal, 0.0)));

        /* Gram-Schmidt re-orthogonalization process */
        T = normalize(T - dot(T, N) * N);
        
        vec3 B = cross(N, T);

        TBN = mat3(T, B, N);
    }
    else
    {
        TBN = mat3(0);
    }
}
