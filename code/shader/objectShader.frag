#version 330 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct GBuffer
{
    sampler2D texture_position;
    sampler2D texture_normal;
    sampler2D texture_albedo;
    sampler2D texture_specular;
    sampler2D texture_shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    sampler2D texture_shadow1;
};

struct DirLightMatrices
{
    mat4 shadowView;
    mat4 shadowProjection;
};

in vec2 UV;

uniform GBuffer gBuffer;

#define MAX_DIR_LIGHTS 1
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform DirLightMatrices dirLightsMatrices[MAX_DIR_LIGHTS];

uniform vec3 viewPos;

float calcDirShadow(DirLight light, vec4 shadowCoords)
{
    /* ESM shadows */
    vec3 projCoords = shadowCoords.xyz / shadowCoords.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec2 moments = texture(light.texture_shadow1, projCoords.xy).rg;

    if (currentDepth < moments.x)
    {
        return 1.0;    
    }

    if (currentDepth < 0 || currentDepth > 0.85)
    {
        return 1.0;    
    }

    float esmFactor = 80.0;

    float occluder = moments.y;
    float receiver = esmFactor * currentDepth;
    float shadow = smoothstep(0.4, 1.0, exp(occluder - receiver));

    return shadow;
}

vec4 calcDirLight(DirLight light, vec4 shadowCoords)
{
    vec3 fragmentPos = texture(gBuffer.texture_position, UV).rgb;
    vec3 normal = texture(gBuffer.texture_normal, UV).rgb;
    vec4 diffuseFrag = texture(gBuffer.texture_albedo, UV);
    vec4 specularFrag = texture(gBuffer.texture_specular, UV);
    float shininessFrag = texture(gBuffer.texture_shininess, UV).r * 255;

    vec3 viewDir = normalize(viewPos - fragmentPos);
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    /* Blinn-Phong */
    vec3 halfwayRay = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayRay), 0.0), shininessFrag); 

    vec4 ambient = vec4(light.ambient, 1.0) * diffuseFrag;
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * diffuseFrag;
    vec4 specular = vec4(light.specular, 1.0) * spec * specularFrag;

    float shadow = calcDirShadow(light, shadowCoords);

    return ambient + shadow * (diffuse + specular);
}

void main()
{
    vec4 result = vec4(0.0f);

    /* dir light */
    for (int i = 0; i < MAX_DIR_LIGHTS; i++)
    {
        /* calc shadow coords */
        vec3 fragmentPos = texture(gBuffer.texture_position, UV).rgb;
        vec4 dirShadowCoords = dirLightsMatrices[i].shadowProjection * dirLightsMatrices[i].shadowView * vec4(fragmentPos, 1.0);

        result += calcDirLight(dirLights[i], dirShadowCoords);
    }

    /* alpha */
    if (result.a < 0.1)
    {
        discard;    
    }

    /* bloom */
    fragColor = result;

    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0)
    {
        brightColor = vec4(fragColor.rgb, 1.0);
    }
    else
    {
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
