#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gMetRoughAOCos;

layout (location = 4) out vec4 gLightScattering;

layout (location = 5) out float ssaoPosition;
layout (location = 6) out vec3 ssaoNormal;

layout (location = 7) out float staticDepth;

struct Material
{
    sampler2D texture_normal1;
    sampler2D texture_diffuse1;
    sampler2D texture_metallic1;
    sampler2D texture_roughness1;
    sampler2D texture_ao1;
};

in vec2 textureCoords;

in vec3 fragmentPos;
in vec3 fragmentNorm;

in mat3 TBN;

in vec3 ssaoFragmentPos;
in vec3 ssaoFragmentNorm;

in mat3 ssaoTBN;

uniform Material material;
uniform float minNormalCosAngle;
uniform int isStatic;

void main()
{
    gPosition = fragmentPos;

    if (TBN == mat3(0))
    {
        gNormal = normalize(fragmentNorm); 
    }
    else /* normal mapping */
    {
        gNormal = texture(material.texture_normal1, textureCoords).rgb;
        gNormal = normalize(gNormal * 2.0 - 1.0);
        gNormal = normalize(TBN * gNormal);
    }

    gAlbedo = texture(material.texture_diffuse1, textureCoords);
    gMetRoughAOCos.x = texture(material.texture_metallic1, textureCoords).r;
    gMetRoughAOCos.y = texture(material.texture_roughness1, textureCoords).r;
    gMetRoughAOCos.z = texture(material.texture_ao1, textureCoords).r;
    gMetRoughAOCos.w = minNormalCosAngle;
    
    gLightScattering = vec4(0, 0, 0, texture(material.texture_diffuse1, textureCoords).a);

    /*** SSAO ***/

    ssaoPosition = ssaoFragmentPos.z;
    
    if (ssaoTBN == mat3(0))
    {
        ssaoNormal = normalize(ssaoFragmentNorm); 
    }
    else /* ssao normal mapping */
    {
        ssaoNormal = texture(material.texture_normal1, textureCoords).rgb;
        ssaoNormal = normalize(ssaoNormal * 2.0 - 1.0);
        ssaoNormal = normalize(ssaoTBN * ssaoNormal);
    }

    if (isStatic == 1)
    {
        staticDepth = gl_FragCoord.z;
    }
    else
    {
        staticDepth = 0.0; 
    }

    /* alpha */
    if (gAlbedo.a < 0.1)
    {
        discard;
    }
}
