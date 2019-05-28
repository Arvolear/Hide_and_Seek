#version 330 core

struct Material
{
    float shininess;
    
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    sampler2DShadow texture_shadow1;
};

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

in vec3 fragmentPos;
in vec3 fragmentNorm;
in vec2 textureCoords;

uniform Material material;

#define MAX_DIR_LIGHTS 5
in vec4 dirShadowCoords[MAX_DIR_LIGHTS];
uniform DirLight dirLights[MAX_DIR_LIGHTS];

#define MAX_POINT_LIGHTS 5
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform vec3 viewPos;

out vec4 color;

float calcDirShadow(DirLight light, vec3 normal, vec4 shadowCoords)
{
    vec3 projCoords = shadowCoords.xyz / shadowCoords.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    vec3 direction = normalize(-light.direction);
    
    float shadowValue = 0.0;

    /* bias */
    vec2 texelSize = 1.0 / textureSize(light.texture_shadow1, 0);        
    float bias = max(0.005 * (1.0 - dot(normal, direction)), 0.005);

    for (float i = -1.5; i <= 1.5; i += 1.0)
    {
        for (float j = -1.5; j <= 1.5; j += 1.0)
        {
            vec3 SC = vec3(projCoords.xy + vec2(i, j) * texelSize, projCoords.z - bias);
            shadowValue += texture(light.texture_shadow1, SC);
        }
    }

    shadowValue /= 16.0;

    if (projCoords.z > 1.0)
    {
        shadowValue = 1.0;    
    }

    return shadowValue;
}

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 shadowCoords)
{
    vec3 direction = normalize(-light.direction);

    float diff = max(dot(normal, direction), 0.0);

    vec3 halfwayRay = normalize(light.direction + viewDir);
    float spec = pow(max(dot(viewDir, halfwayRay), 0.0), material.shininess); // Blinn-Phong

    vec4 ambient = vec4(light.ambient, 1.0f) * texture(material.texture_diffuse1, textureCoords);
    vec4 diffuse = vec4(light.diffuse, 1.0f) * diff * texture(material.texture_diffuse1, textureCoords);
    vec4 specular = vec4(light.specular, 1.0f) * spec * texture(material.texture_specular1, textureCoords);

    float shadow = calcDirShadow(light, normal, shadowCoords);

    return (ambient + shadow * (diffuse + specular));
}

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

void main()
{
    vec3 norm = normalize(fragmentNorm); 
    vec3 viewDir = normalize(viewPos - fragmentPos);

    vec4 result = vec4(0.0f);

    for (int i = 0; i < MAX_DIR_LIGHTS; i++)
    {
        result += calcDirLight(dirLights[i], norm, viewDir, dirShadowCoords[i]);
    }

    /*for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        result += calcPointLight(pointLights[i], norm, fragmentPos, viewDir);
    }*/

    if (result.a < 0.1)
    {
        discard;    
    }

    color = result;
}
