#version 330 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct Material
{
    float shininess;
    
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    sampler2D texture_shadow1;
};

/*
struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 
 */

in vec3 fragmentPos;
in vec3 fragmentNorm;
in vec2 textureCoords;
in mat3 TBN;

uniform Material material;

#define MAX_DIR_LIGHTS 5
in vec4 dirShadowCoords[MAX_DIR_LIGHTS];
uniform DirLight dirLights[MAX_DIR_LIGHTS];

/*
#define MAX_POINT_LIGHTS 5
uniform PointLight pointLights[MAX_POINT_LIGHTS];
 */

uniform vec3 viewPos;

/*float calcDirShadow(DirLight light, vec3 normal, vec4 shadowCoords)
{
    vec3 projCoords = shadowCoords.xyz / shadowCoords.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec3 direction = normalize(-light.direction);
    
    float shadowValue = 0.0;

    // bias 
    vec2 texelSize = 1.0 / textureSize(light.texture_shadow1, 0);        
    float bias = max(0.01 * (1.0 - dot(normal, direction)), 0.005);

    // pcf 
    for (float i = -1.5; i <= 1.5; i += 1.0)
    {
        for (float j = -1.5; j <= 1.5; j += 1.0)
        {
            vec3 SC = vec3(projCoords.xy + vec2(i, j) * texelSize, currentDepth - bias);
            shadowValue += texture(light.texture_shadow1, SC);
        }
    }

    shadowValue /= 16.0;

    if (projCoords.z > 1.0)
    {
        shadowValue = 1.0;    
    }

    return shadowValue;
}*/

float calcDirShadow(DirLight light, vec3 normal, vec4 shadowCoords)
{
    vec3 projCoords = shadowCoords.xyz / shadowCoords.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec2 moments = texture2D(light.texture_shadow1, projCoords.xy).rg;

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
    vec3 normal = normalize(fragmentNorm); 
    vec3 viewDir = normalize(viewPos - fragmentPos);
    vec3 lightDir = normalize(-light.direction);

    /* normal mapping */
    if (TBN != mat3(0))
    {
        normal = texture(material.texture_normal1, textureCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    }

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayRay = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayRay), 0.0), material.shininess); // Blinn-Phong

    vec4 ambient = vec4(light.ambient, 1.0f) * texture(material.texture_diffuse1, textureCoords);
    vec4 diffuse = vec4(light.diffuse, 1.0f) * diff * texture(material.texture_diffuse1, textureCoords);
    vec4 specular = vec4(light.specular, 1.0f) * spec * texture(material.texture_specular1, textureCoords);

    float shadow = calcDirShadow(light, normal, shadowCoords);

    return ambient + shadow * (diffuse + specular);
}

/*
   vec4 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
   {
   vec3 direction = normalize(light.position - fragPos);

   float diff = max(dot(normal, direction), 0.0);

   vec3 reflectionRay = reflect(-direction, normal);
   float spec = pow(max(dot(viewDir, reflectionRay), 0.0), material.shininess);

   float distance = length(light.position - fragPos);
   float attenuation = 1.0 / (light.constant + (light.linear * distance) + light.quadratic * (distance * distance));

   vec4 ambient = vec4(light.ambient, 1.0f) * texture(material.texture_diffuse1, textureCoords);
   vec4 diffuse = vec4(light.diffuse, 1.0f) * diff * texture(material.texture_diffuse1, textureCoords);
   vec4 specular = vec4(light.specular, 1.0f) * spec * texture(material.texture_specular1, textureCoords);

   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;

   return (ambient + diffuse + specular);
   }
 */

void main()
{
    vec4 result = vec4(0.0f);

    /* dir light */
    for (int i = 0; i < MAX_DIR_LIGHTS; i++)
    {
        if (dirShadowCoords[i] != vec4(0, 0, 0, 0))
        {
            result += calcDirLight(dirLights[i], dirShadowCoords[i]);
        }
    }

    /* point light */
    /*
       for (int i = 0; i < MAX_POINT_LIGHTS; i++)
       {
       result += calcPointLight(pointLights[i], norm, fragmentPos, viewDir);
       }
     */

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
