#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out float gMetallic;
layout (location = 4) out float gRoughness;
layout (location = 5) out float gAO;

struct Material
{
    sampler2D texture_normal1;
    sampler2D texture_diffuse1;
    sampler2D texture_metallic1;
    sampler2D texture_roughness1;
    sampler2D texture_ao1;
};

in vec3 fragmentPos;
in vec3 fragmentNorm;
in vec2 textureCoords;
in mat3 TBN;

uniform Material material;

void main()
{
    gPosition = fragmentPos;

    gNormal = normalize(fragmentNorm); 

    /* normal mapping */
    if (TBN != mat3(0))
    {
        gNormal = texture(material.texture_normal1, textureCoords).rgb;
        gNormal = normalize(gNormal * 2.0 - 1.0);
        gNormal = normalize(TBN * gNormal);
    }

    gAlbedo = texture(material.texture_diffuse1, textureCoords);
    gMetallic = texture(material.texture_metallic1, textureCoords).r;
    gRoughness = texture(material.texture_roughness1, textureCoords).r;
    gAO = texture(material.texture_ao1, textureCoords).r;

    /* alpha */
    if (gAlbedo.a < 0.1)
    {
        discard;
    }
}
