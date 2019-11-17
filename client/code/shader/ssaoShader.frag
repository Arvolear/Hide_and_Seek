#version 330 core

layout (location = 0) out float ssaoColor;

struct GBuffer
{
    sampler2D texture_normalDepth;
};

in vec2 UV;

#define MAX_KERNEL_SIZE 64

uniform int kernelSize;
uniform int noiseSize;

uniform float radius;
uniform float bias;
uniform float power;

uniform mat4 invProjection;
uniform mat4 projection;

uniform GBuffer gBuffer;

uniform sampler2D texture_noise;

uniform vec3 sphereSamples[MAX_KERNEL_SIZE];

uniform vec2 renderSize;

vec4 reconstructViewPos(vec2 tc)
{
    float depth = texture(gBuffer.texture_normalDepth, tc).w;
    
    vec4 p = vec4(tc.x * 2.0 - 1.0, tc.y * 2.0 - 1.0, depth, 1.0);
    vec4 p_cs = invProjection * p;

    return p_cs / p_cs.w;
}

void main()
{
    vec2 noiseScale = renderSize / float(noiseSize);

    vec3 fragPos = reconstructViewPos(UV).rgb;
    vec3 N = normalize(texture(gBuffer.texture_normalDepth, UV).rgb);
    vec3 randomVec = normalize(texture(texture_noise, UV * noiseScale).rgb);

    vec3 T = normalize(randomVec - N * dot(randomVec, N));
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);

    float occlusion = 0.0;

    for (int i = 0; i < kernelSize; i++)
    {
        vec3 sphereSample = TBN * sphereSamples[i];
        sphereSample = fragPos + sphereSample * radius;

        vec4 sphereOffset = vec4(sphereSample, 1.0);
        sphereOffset = projection * sphereOffset;
        sphereOffset.xyz /= sphereOffset.w;
        sphereOffset = sphereOffset * 0.5 + 0.5;

        float sphereSampleDepth = reconstructViewPos(sphereOffset.xy).z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sphereSampleDepth));
        occlusion += (sphereSampleDepth >= sphereSample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    ssaoColor = pow(occlusion, power);
}
