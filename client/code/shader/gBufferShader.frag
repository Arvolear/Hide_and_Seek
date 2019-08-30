#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gSpecular;
layout (location = 4) out float gShininess;

struct Material
{
    float shininess;
    
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
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
    gSpecular = texture(material.texture_specular1, textureCoords);
    gShininess = material.shininess;

    /* alpha */
    if (gAlbedo.a < 0.1)
    {
        discard;
    }
}
